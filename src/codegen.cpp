#include "codegen.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <functional>

// Ensure all AST node types are visible
using std::unique_ptr;

CodeGenerator::CodeGenerator(const std::string& filename) 
    : next_label(0), indent_level(0), needs_math_h(false), 
      needs_string_h(false), needs_time_h(false), needs_sdl(false), needs_sound(false) {
    
    output.open(filename);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open output file: " + filename);
    }
}

CodeGenerator::~CodeGenerator() {
    if (output.is_open()) {
        output.close();
    }
}

std::string CodeGenerator::get_indent() const {
    return std::string(indent_level * 4, ' ');
}

void CodeGenerator::write(const std::string& code) {
    output << code;
}

void CodeGenerator::writeln(const std::string& code) {
    output << get_indent() << code << "\n";
}

// ============== VARIABLE COLLECTION ==============

void CodeGenerator::collect_expr_variables(const ASTNode* expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case NodeType::VARIABLE: {
            const VariableNode* var = static_cast<const VariableNode*>(expr);
            
            // Skip if this is an array
            bool is_array = (arrays.find(var->name) != arrays.end()) ||
                           (string_arrays.find(var->name) != string_arrays.end());
            
            if (!is_array) {
                if (var->is_string) {
                    string_variables.insert(var->name);
                } else {
                    numeric_variables.insert(var->name);
                }
            }
            break;
        }
        
        case NodeType::ARRAY_ACCESS: {
            const ArrayAccessNode* arr = static_cast<const ArrayAccessNode*>(expr);
            // Arrays are handled separately in DIM statements
            for (const auto& idx : arr->indices) {
                collect_expr_variables(idx.get());
            }
            break;
        }
        
        case NodeType::BINARY_OP: {
            const BinaryOpNode* binop = static_cast<const BinaryOpNode*>(expr);
            collect_expr_variables(binop->left.get());
            collect_expr_variables(binop->right.get());
            // MOD uses fmod(), ^ uses pow() which need math.h
            if (binop->op == "MOD" || binop->op == "^") {
                needs_math_h = true;
            }
            break;
        }
        
        case NodeType::UNARY_OP: {
            const UnaryOpNode* unary = static_cast<const UnaryOpNode*>(expr);
            collect_expr_variables(unary->operand.get());
            break;
        }
        
        case NodeType::FUNCTION_CALL: {
            const FunctionCallNode* func = static_cast<const FunctionCallNode*>(expr);
            for (const auto& arg : func->arguments) {
                collect_expr_variables(arg.get());
            }
            
            // Check if function requires special headers
            std::string fname = func->name;
            if (fname == "SIN" || fname == "COS" || fname == "TAN" || 
                fname == "ATN" || fname == "SQR" || fname == "LOG" || 
                fname == "EXP" || fname == "ABS" || fname == "INT" || fname == "SGN") {
                needs_math_h = true;
            }
            // String functions (with $ or special cases like TAB, SPACE$, STRING$)
            if (fname.find('$') != std::string::npos || 
                fname == "TAB" || fname == "SPACE$" || fname == "STRING$") {
                needs_string_h = true;
            }
            if (fname == "RND" || fname == "RANDOMIZE") {
                needs_time_h = true;
            }
            break;
        }
        
        default:
            break;
    }
}

// Helper function to recursively collect variables from statement bodies
void CodeGenerator::collect_stmt_body_variables(const std::vector<std::unique_ptr<Statement>>& body) {
    for (const auto& body_stmt : body) {
        if (!body_stmt->stmt) continue;
        
        switch (body_stmt->stmt->type) {
            case NodeType::LET_STMT: {
                const LetStmt* let = static_cast<const LetStmt*>(body_stmt->stmt.get());
                bool is_array = (arrays.find(let->variable) != arrays.end()) ||
                               (string_arrays.find(let->variable) != string_arrays.end());
                if (!is_array) {
                    if (!let->variable.empty() && let->variable.back() == '$') {
                        string_variables.insert(let->variable);
                    } else {
                        numeric_variables.insert(let->variable);
                    }
                }
                collect_expr_variables(let->expression.get());
                if (let->array_access) {
                    collect_expr_variables(let->array_access.get());
                }
                break;
            }
            
            case NodeType::PRINT_STMT: {
                const PrintStmt* print = static_cast<const PrintStmt*>(body_stmt->stmt.get());
                for (const auto& expr : print->expressions) {
                    collect_expr_variables(expr.get());
                }
                break;
            }
            
            case NodeType::IF_STMT: {
                const IfStmt* if_stmt = static_cast<const IfStmt*>(body_stmt->stmt.get());
                collect_expr_variables(if_stmt->condition.get());
                collect_stmt_body_variables(if_stmt->then_body);
                collect_stmt_body_variables(if_stmt->else_body);
                break;
            }
            
            case NodeType::FOR_STMT: {
                const ForStmt* nested_for = static_cast<const ForStmt*>(body_stmt->stmt.get());
                numeric_variables.insert(nested_for->variable);
                collect_expr_variables(nested_for->start.get());
                collect_expr_variables(nested_for->end.get());
                if (nested_for->step) {
                    collect_expr_variables(nested_for->step.get());
                }
                // Recursively collect from nested loop body
                collect_stmt_body_variables(nested_for->body);
                break;
            }
            
            case NodeType::WHILE_STMT: {
                const WhileStmt* while_stmt = static_cast<const WhileStmt*>(body_stmt->stmt.get());
                collect_expr_variables(while_stmt->condition.get());
                collect_stmt_body_variables(while_stmt->body);
                break;
            }
            
            case NodeType::DO_LOOP_STMT: {
                const DoLoopStmt* do_stmt = static_cast<const DoLoopStmt*>(body_stmt->stmt.get());
                if (do_stmt->condition) {
                    collect_expr_variables(do_stmt->condition.get());
                }
                collect_stmt_body_variables(do_stmt->body);
                break;
            }
            
            case NodeType::INPUT_STMT: {
                const InputStmt* input = static_cast<const InputStmt*>(body_stmt->stmt.get());
                for (const auto& var : input->variables) {
                    if (!var.empty() && var.back() == '$') {
                        string_variables.insert(var);
                    } else {
                        numeric_variables.insert(var);
                    }
                }
                break;
            }
            
            case NodeType::READ_STMT: {
                const ReadStmt* read = static_cast<const ReadStmt*>(body_stmt->stmt.get());
                for (const auto& target : read->variables) {
                    // Skip if it's an array (arrays handled by DIM)
                    if (target.is_array()) {
                        continue;
                    }
                    if (!target.name.empty() && target.name.back() == '$') {
                        string_variables.insert(target.name);
                    } else {
                        numeric_variables.insert(target.name);
                    }
                }
                break;
            }
            
            default:
                break;
        }
    }
}

// Detect if graphics or sound are used in the program
void CodeGenerator::detect_graphics_sound(const Program* program) {
    for (const auto& stmt : program->statements) {
        if (!stmt->stmt) continue;
        
        switch (stmt->stmt->type) {
            case NodeType::SCREEN_STMT:
            case NodeType::PSET_STMT:
            case NodeType::LINE_STMT:
            case NodeType::CIRCLE_STMT:
            case NodeType::PAINT_STMT:
                needs_sdl = true;
                break;
                
            case NodeType::SOUND_STMT:
            case NodeType::PLAY_STMT:
            case NodeType::BEEP_STMT:   // BEEP needs windows.h on Windows
            case NodeType::SLEEP_STMT:  // SLEEP needs windows.h on Windows
                needs_sound = true;
                break;
                
            default:
                break;
        }
        
        // Check if graphics cleared (CLS in graphics mode)
        if (stmt->stmt->type == NodeType::CLS_STMT && needs_sdl) {
            // CLS will use SDL
        }
    }
}

void CodeGenerator::collect_variables(const Program* program) {
    // First pass: collect DIM statements to know which are arrays
    for (const auto& stmt : program->statements) {
        if (stmt->stmt && stmt->stmt->type == NodeType::DIM_STMT) {
            const DimStmt* dim = static_cast<const DimStmt*>(stmt->stmt.get());
            
            // Handle new multi-array format
            if (!dim->arrays.empty()) {
                for (const auto& array_decl : dim->arrays) {
                    if (array_decl.is_string) {
                        string_arrays[array_decl.name] = array_decl.dimensions;
                    } else {
                        arrays[array_decl.name] = array_decl.dimensions;
                    }
                }
            } else {
                // Legacy single-array format (backward compatibility)
                if (dim->is_string) {
                    string_arrays[dim->name] = dim->dimensions;
                } else {
                    arrays[dim->name] = dim->dimensions;
                }
            }
        }
    }
    
    // Second pass: collect other variables, skipping array names
    for (const auto& stmt : program->statements) {
        if (!stmt->stmt) continue;
        
        switch (stmt->stmt->type) {
            case NodeType::LET_STMT: {
                const LetStmt* let = static_cast<const LetStmt*>(stmt->stmt.get());
                
                // Skip if this is an array
                bool is_array = (arrays.find(let->variable) != arrays.end()) ||
                               (string_arrays.find(let->variable) != string_arrays.end());
                
                if (!is_array) {
                    // Check if it's a string variable
                    if (!let->variable.empty() && let->variable.back() == '$') {
                        string_variables.insert(let->variable);
                    } else {
                        numeric_variables.insert(let->variable);
                    }
                }
                
                collect_expr_variables(let->expression.get());
                if (let->array_access) {
                    collect_expr_variables(let->array_access.get());
                }
                break;
            }
            
            case NodeType::PRINT_STMT: {
                const PrintStmt* print = static_cast<const PrintStmt*>(stmt->stmt.get());
                for (const auto& expr : print->expressions) {
                    collect_expr_variables(expr.get());
                }
                break;
            }
            
            case NodeType::IF_STMT: {
                const IfStmt* if_stmt = static_cast<const IfStmt*>(stmt->stmt.get());
                collect_expr_variables(if_stmt->condition.get());
                // Recursively collect from then and else bodies
                collect_stmt_body_variables(if_stmt->then_body);
                collect_stmt_body_variables(if_stmt->else_body);
                break;
            }
            
            case NodeType::FOR_STMT: {
                const ForStmt* for_stmt = static_cast<const ForStmt*>(stmt->stmt.get());
                numeric_variables.insert(for_stmt->variable);
                collect_expr_variables(for_stmt->start.get());
                collect_expr_variables(for_stmt->end.get());
                if (for_stmt->step) {
                    collect_expr_variables(for_stmt->step.get());
                }
                // Recursively collect from loop body (handles nested FOR loops too)
                collect_stmt_body_variables(for_stmt->body);
                break;
            }
            
            case NodeType::WHILE_STMT: {
                const WhileStmt* while_stmt = static_cast<const WhileStmt*>(stmt->stmt.get());
                collect_expr_variables(while_stmt->condition.get());
                // Recursively collect from loop body
                collect_stmt_body_variables(while_stmt->body);
                break;
            }
            
            case NodeType::DO_LOOP_STMT: {
                const DoLoopStmt* do_stmt = static_cast<const DoLoopStmt*>(stmt->stmt.get());
                if (do_stmt->condition) {
                    collect_expr_variables(do_stmt->condition.get());
                }
                // Recursively collect from loop body
                collect_stmt_body_variables(do_stmt->body);
                break;
            }
            
            case NodeType::INPUT_STMT: {
                const InputStmt* input = static_cast<const InputStmt*>(stmt->stmt.get());
                for (const auto& var : input->variables) {
                    if (!var.empty() && var.back() == '$') {
                        string_variables.insert(var);
                    } else {
                        numeric_variables.insert(var);
                    }
                }
                break;
            }
            
            case NodeType::LINE_INPUT_STMT: {
                const LineInputStmt* input = static_cast<const LineInputStmt*>(stmt->stmt.get());
                string_variables.insert(input->variable);
                break;
            }
            
            case NodeType::READ_STMT: {
                const ReadStmt* read = static_cast<const ReadStmt*>(stmt->stmt.get());
                for (const auto& target : read->variables) {
                    // Skip if it's an array (arrays handled by DIM)
                    if (target.is_array()) {
                        continue;
                    }
                    if (!target.name.empty() && target.name.back() == '$') {
                        string_variables.insert(target.name);
                    } else {
                        numeric_variables.insert(target.name);
                    }
                }
                break;
            }
            
            default:
                break;
        }
    }
}

void CodeGenerator::collect_line_labels(const Program* program) {
    int label_count = 0;
    
    // Helper function to recursively collect labels
    std::function<void(const std::vector<std::unique_ptr<Statement>>&)> collect_from_statements;
    collect_from_statements = [&](const std::vector<std::unique_ptr<Statement>>& statements) {
        for (const auto& stmt : statements) {
            if (stmt->line_num > 0 && line_labels.find(stmt->line_num) == line_labels.end()) {
                line_labels[stmt->line_num] = label_count++;
            }
            
            // Recursively collect from nested structures
            if (stmt->stmt) {
                switch (stmt->stmt->type) {
                    case NodeType::FOR_STMT: {
                        const ForStmt* for_stmt = static_cast<const ForStmt*>(stmt->stmt.get());
                        collect_from_statements(for_stmt->body);
                        // Also register the NEXT line number
                        if (for_stmt->next_line_num > 0 && line_labels.find(for_stmt->next_line_num) == line_labels.end()) {
                            line_labels[for_stmt->next_line_num] = label_count++;
                        }
                        break;
                    }
                    case NodeType::WHILE_STMT: {
                        const WhileStmt* while_stmt = static_cast<const WhileStmt*>(stmt->stmt.get());
                        collect_from_statements(while_stmt->body);
                        break;
                    }
                    case NodeType::DO_LOOP_STMT: {
                        const DoLoopStmt* do_stmt = static_cast<const DoLoopStmt*>(stmt->stmt.get());
                        collect_from_statements(do_stmt->body);
                        break;
                    }
                    case NodeType::IF_STMT: {
                        const IfStmt* if_stmt = static_cast<const IfStmt*>(stmt->stmt.get());
                        if (if_stmt->is_block) {
                            collect_from_statements(if_stmt->then_body);
                            collect_from_statements(if_stmt->else_body);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    };
    
    // Start with top-level statements
    collect_from_statements(program->statements);
}

void CodeGenerator::collect_data_values(const Program* program) {
    for (const auto& stmt : program->statements) {
        if (stmt->stmt && stmt->stmt->type == NodeType::DATA_STMT) {
            const DataStmt* data = static_cast<const DataStmt*>(stmt->stmt.get());
            for (const auto& val : data->values) {
                data_values.push_back(val);
            }
        }
    }
}

// ============== HEADER AND DECLARATIONS ==============

void CodeGenerator::generate_headers() {
    writeln("#include <stdio.h>");
    writeln("#include <stdlib.h>");
    writeln("#include <string.h>");
    
    if (needs_math_h) {
        writeln("#include <math.h>");
    }
    
    // Always include time.h now that we have DATE$, TIME$, TIMER, RANDOMIZE
    writeln("#include <time.h>");
    
    // SDL2 for graphics
    if (needs_sdl) {
        writeln("#include <SDL2/SDL.h>");
    }
    
    // Windows headers for sound and sleep  
    if (needs_sound) {
        writeln("#ifdef _WIN32");
        writeln("#include <windows.h>");
        writeln("#endif");
    }
    
    writeln("");
}

void CodeGenerator::generate_variable_declarations() {
    if (!numeric_variables.empty()) {
        writeln("// Numeric variables");
        for (const auto& var : numeric_variables) {
            // On Windows with SDL2, CY conflicts with Windows currency type
            // Prefix it with rb_ to avoid conflict
            std::string c_var = var;
            if (needs_sdl && c_var == "CY") {
                c_var = "rb_CY";
            }
            writeln("double " + c_var + " = 0.0;");
        }
        writeln("");
    }
    
    if (!string_variables.empty()) {
        writeln("// String variables");
        for (const auto& var : string_variables) {
            // Replace $ suffix with _str to avoid conflicts with numeric variables
            std::string c_var = var;
            if (!c_var.empty() && c_var.back() == '$') {
                c_var.pop_back();
                c_var += "_str";
            }
            // Skip DATE to avoid conflict with Windows DATE type
            if (c_var == "DATE_str" || c_var == "DATE") {
                continue;
            }
            writeln("char " + c_var + "[256] = \"\";");
        }
        writeln("");
    }
    
    // SDL2 Graphics variables
    if (needs_sdl) {
        writeln("// SDL2 Graphics");
        writeln("SDL_Window* window = NULL;");
        writeln("SDL_Renderer* renderer = NULL;");
        writeln("int screen_width = 640;");
        writeln("int screen_height = 480;");
        writeln("int screen_mode = 0;");
        writeln("int current_color = 15;  // Default white");
        writeln("");
        
        writeln("// EGA 16-color palette");
        writeln("SDL_Color palette[16] = {");
        writeln("    {0, 0, 0, 255},       // 0: Black");
        writeln("    {0, 0, 170, 255},     // 1: Blue");
        writeln("    {0, 170, 0, 255},     // 2: Green");
        writeln("    {0, 170, 170, 255},   // 3: Cyan");
        writeln("    {170, 0, 0, 255},     // 4: Red");
        writeln("    {170, 0, 170, 255},   // 5: Magenta");
        writeln("    {170, 85, 0, 255},    // 6: Brown");
        writeln("    {170, 170, 170, 255}, // 7: Light Gray");
        writeln("    {85, 85, 85, 255},    // 8: Dark Gray");
        writeln("    {85, 85, 255, 255},   // 9: Light Blue");
        writeln("    {85, 255, 85, 255},   // 10: Light Green");
        writeln("    {85, 255, 255, 255},  // 11: Light Cyan");
        writeln("    {255, 85, 85, 255},   // 12: Light Red");
        writeln("    {255, 85, 255, 255},  // 13: Light Magenta");
        writeln("    {255, 255, 85, 255},  // 14: Yellow");
        writeln("    {255, 255, 255, 255}  // 15: White");
        writeln("};");
        writeln("");
    }
}

void CodeGenerator::generate_array_declarations() {
    if (!arrays.empty()) {
        writeln("// Numeric arrays");
        for (const auto& arr_pair : arrays) {
            std::string arr_name = arr_pair.first;
            const auto& dims = arr_pair.second;
            
            std::string decl = "double " + arr_name;
            for (int dim : dims) {
                decl += "[" + std::to_string(dim + 1) + "]";
            }
            decl += " = {0};";
            writeln(decl);
        }
        writeln("");
    }
    
    if (!string_arrays.empty()) {
        writeln("// String arrays");
        for (const auto& arr_pair : string_arrays) {
            std::string arr_name = arr_pair.first;
            if (!arr_name.empty() && arr_name.back() == '$') {
                arr_name.pop_back();
                arr_name += "_str";
            }
            
            const auto& dims = arr_pair.second;
            
            std::string decl = "char " + arr_name;
            for (int dim : dims) {
                decl += "[" + std::to_string(dim + 1) + "]";
            }
            decl += "[256] = {0};";
            writeln(decl);
        }
        writeln("");
    }
}

void CodeGenerator::generate_data_initialization() {
    if (data_values.empty()) return;
    
    writeln("// DATA values");
    write(get_indent() + "const char* _data_values[] = {");
    
    for (size_t i = 0; i < data_values.size(); i++) {
        if (i > 0) write(", ");
        write("\"" + data_values[i] + "\"");
    }
    
    write("};\n");
    writeln("int _data_index = 0;");
    writeln("const int _data_count = " + std::to_string(data_values.size()) + ";");
    writeln("");
}

void CodeGenerator::generate_builtin_functions() {
    writeln("// Built-in helper functions");
    writeln("");
    
    // GOSUB/RETURN stack
    writeln("int _gosub_stack[100];");
    writeln("int _gosub_sp = 0;");
    writeln("");
    
    // RND implementation
    writeln("int _rnd_initialized = 0;");
    writeln("double RB_RND() {");
    writeln("    if (!_rnd_initialized) {");
    writeln("        srand((unsigned)time(NULL));");
    writeln("        _rnd_initialized = 1;");
    writeln("    }");
    writeln("    // Use 32767.0 (common RAND_MAX value) to ensure proper division");
    writeln("    return (double)rand() / 32767.0;");
    writeln("}");
    writeln("");
    
    // String functions
    if (needs_string_h || !string_variables.empty()) {
        writeln("void RB_LEFT(char* dest, const char* src, int len) {");
        writeln("    if (len <= 0) { dest[0] = '\\0'; return; }");
        writeln("    int src_len = strlen(src);");
        writeln("    int copy_len = (len < src_len) ? len : src_len;");
        writeln("    strncpy(dest, src, copy_len);");
        writeln("    dest[copy_len] = '\\0';");
        writeln("}");
        writeln("");
        
        writeln("void RB_RIGHT(char* dest, const char* src, int len) {");
        writeln("    int src_len = strlen(src);");
        writeln("    if (len <= 0) { dest[0] = '\\0'; return; }");
        writeln("    if (len >= src_len) { strcpy(dest, src); return; }");
        writeln("    strcpy(dest, src + (src_len - len));");
        writeln("}");
        writeln("");
        
        writeln("void RB_MID(char* dest, const char* src, int start, int len) {");
        writeln("    int src_len = strlen(src);");
        writeln("    start = start - 1;  // BASIC uses 1-based indexing");
        writeln("    if (start < 0) start = 0;");
        writeln("    if (start >= src_len) {");
        writeln("        dest[0] = '\\0';");
        writeln("        return;");
        writeln("    }");
        writeln("    int copy_len = (len < src_len - start) ? len : (src_len - start);");
        writeln("    strncpy(dest, src + start, copy_len);");
        writeln("    dest[copy_len] = '\\0';");
        writeln("}");
        writeln("");
        
        writeln("char* RB_CONCAT(char* dest, const char* s1, const char* s2) {");
        writeln("    strcpy(dest, s1);");
        writeln("    strcat(dest, s2);");
        writeln("    return dest;");
        writeln("}");
        writeln("");
        
        writeln("int RB_INSTR(const char* haystack, const char* needle) {");
        writeln("    const char* pos = strstr(haystack, needle);");
        writeln("    return pos ? (int)(pos - haystack) + 1 : 0;  // 1-based");
        writeln("}");
        writeln("");
        
        writeln("void RB_CHR(char* dest, int code) {");
        writeln("    dest[0] = (char)code;");
        writeln("    dest[1] = '\\0';");
        writeln("}");
        writeln("");
        
        writeln("void RB_STR(char* dest, double num) {");
        writeln("    sprintf(dest, \"%g\", num);");
        writeln("}");
        writeln("");
        
        writeln("void RB_UCASE(char* dest, const char* src) {");
        writeln("    strcpy(dest, src);");
        writeln("    for (int i = 0; dest[i]; i++) {");
        writeln("        if (dest[i] >= 'a' && dest[i] <= 'z') {");
        writeln("            dest[i] = dest[i] - 32;");
        writeln("        }");
        writeln("    }");
        writeln("}");
        writeln("");
        
        writeln("void RB_LCASE(char* dest, const char* src) {");
        writeln("    strcpy(dest, src);");
        writeln("    for (int i = 0; dest[i]; i++) {");
        writeln("        if (dest[i] >= 'A' && dest[i] <= 'Z') {");
        writeln("            dest[i] = dest[i] + 32;");
        writeln("        }");
        writeln("    }");
        writeln("}");
        writeln("");
    }
    
    // File handles
    writeln("FILE* _file_handles[256] = {NULL};");
    writeln("");
    
    // System functions
    writeln("// System time functions");
    writeln("void RB_GET_DATE(char* dest) {");
    writeln("    time_t now = time(NULL);");
    writeln("    struct tm* t = localtime(&now);");
    writeln("    sprintf(dest, \"%02d-%02d-%04d\", t->tm_mon + 1, t->tm_mday, t->tm_year + 1900);");
    writeln("}");
    writeln("");
    
    writeln("void RB_GET_TIME(char* dest) {");
    writeln("    time_t now = time(NULL);");
    writeln("    struct tm* t = localtime(&now);");
    writeln("    sprintf(dest, \"%02d:%02d:%02d\", t->tm_hour, t->tm_min, t->tm_sec);");
    writeln("}");
    writeln("");
    
    writeln("double RB_TIMER() {");
    writeln("    time_t now = time(NULL);");
    writeln("    struct tm* t = localtime(&now);");
    writeln("    return (double)(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec);");
    writeln("}");
    writeln("");
    
    // Keyboard input
    writeln("// Keyboard input");
    writeln("#ifdef _WIN32");
    writeln("#include <conio.h>");
    writeln("void RB_INKEY(char* dest) {");
    writeln("    if (_kbhit()) {");
    writeln("        int ch = _getch();");
    writeln("        dest[0] = (char)ch;");
    writeln("        dest[1] = '\\0';");
    writeln("    } else {");
    writeln("        dest[0] = '\\0';");
    writeln("    }");
    writeln("}");
    writeln("");
    writeln("void RB_INPUT_STR(char* dest, int count) {");
    writeln("    int i = 0;");
    writeln("    while (i < count && i < 255) {");
    writeln("        int ch = _getch();");
    writeln("        if (ch == '\\r' || ch == '\\n') break;");
    writeln("        dest[i++] = (char)ch;");
    writeln("        putchar(ch);  // Echo character");
    writeln("    }");
    writeln("    dest[i] = '\\0';");
    writeln("    fflush(stdout);");
    writeln("}");
    writeln("#else");
    writeln("void RB_INKEY(char* dest) { dest[0] = '\\0'; }  // Stub for non-Windows");
    writeln("");
    writeln("void RB_INPUT_STR(char* dest, int count) {");
    writeln("    int i = 0;");
    writeln("    while (i < count && i < 255 && (dest[i] = getchar()) != '\\n') {");
    writeln("        i++;");
    writeln("    }");
    writeln("    dest[i] = '\\0';");
    writeln("}");
    writeln("#endif");
    writeln("");
    
    // Number formatting
    writeln("// Number formatting");
    writeln("void RB_HEX(char* dest, int num) {");
    writeln("    sprintf(dest, \"%X\", num);");
    writeln("}");
    writeln("");
    
    writeln("void RB_OCT(char* dest, int num) {");
    writeln("    sprintf(dest, \"%o\", num);");
    writeln("}");
    writeln("");
    
    writeln("void RB_TAB(char* dest, int column) {");
    writeln("    // TAB(n) returns string of spaces to reach column n");
    writeln("    // Note: In classic BASIC, column position is tracked by cursor");
    writeln("    // Here we just return n spaces for simplicity");
    writeln("    if (column < 1) column = 1;");
    writeln("    if (column > 255) column = 255;");
    writeln("    for (int i = 0; i < column && i < 255; i++) {");
    writeln("        dest[i] = ' ';");
    writeln("    }");
    writeln("    dest[column] = '\\0';");
    writeln("}");
    writeln("");
    
    // Screen control (Windows console)
    writeln("// Screen control");
    writeln("#ifdef _WIN32");
    writeln("#include <windows.h>");
    writeln("void RB_CLS() {");
    writeln("    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);");
    writeln("    COORD coordScreen = {0, 0};");
    writeln("    DWORD cCharsWritten;");
    writeln("    CONSOLE_SCREEN_BUFFER_INFO csbi;");
    writeln("    DWORD dwConSize;");
    writeln("    GetConsoleScreenBufferInfo(hConsole, &csbi);");
    writeln("    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;");
    writeln("    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);");
    writeln("    GetConsoleScreenBufferInfo(hConsole, &csbi);");
    writeln("    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);");
    writeln("    SetConsoleCursorPosition(hConsole, coordScreen);");
    writeln("}");
    writeln("");
    
    writeln("void RB_LOCATE(int row, int col) {");
    writeln("    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);");
    writeln("    COORD pos = {(SHORT)(col - 1), (SHORT)(row - 1)};");
    writeln("    SetConsoleCursorPosition(hConsole, pos);");
    writeln("    fflush(stdout);  // Ensure cursor position is applied");
    writeln("}");
    writeln("");
    
    writeln("void RB_COLOR(int fg, int bg) {");
    writeln("    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);");
    writeln("    SetConsoleTextAttribute(hConsole, (WORD)((bg << 4) | fg));");
    writeln("    fflush(stdout);  // Ensure color is applied");
    writeln("}");
    writeln("#else");
    writeln("void RB_CLS() { printf(\"\\033[2J\\033[H\"); }");
    writeln("void RB_LOCATE(int row, int col) { printf(\"\\033[%d;%dH\", row, col); }");
    writeln("void RB_COLOR(int fg, int bg) { printf(\"\\033[%d;%dm\", 30 + fg, 40 + bg); }");
    writeln("#endif");
    writeln("");
    
    // SDL2 Graphics helper functions
    if (needs_sdl) {
        writeln("// SDL2 Graphics Functions");
        writeln("");
        
        // Initialize graphics
        writeln("void init_graphics(int mode) {");
        indent_level++;
        writeln("screen_mode = mode;");
        writeln("");
        writeln("// Set dimensions based on mode");
        writeln("switch(mode) {");
        indent_level++;
        writeln("case 1:  screen_width = 320; screen_height = 200; break;  // CGA");
        writeln("case 2:  screen_width = 640; screen_height = 200; break;  // CGA Hi-res");
        writeln("case 7:  screen_width = 320; screen_height = 200; break;  // EGA");
        writeln("case 9:  screen_width = 640; screen_height = 350; break;  // EGA Hi-res");
        writeln("case 12: screen_width = 640; screen_height = 480; break;  // VGA");
        writeln("case 13: screen_width = 320; screen_height = 200; break;  // VGA Mode X");
        writeln("default: screen_width = 640; screen_height = 480; break;");
        indent_level--;
        writeln("}");
        writeln("");
        writeln("if (SDL_Init(SDL_INIT_VIDEO) < 0) {");
        indent_level++;
        writeln("printf(\"SDL init failed: %s\\n\", SDL_GetError());");
        writeln("exit(1);");
        indent_level--;
        writeln("}");
        writeln("");
        writeln("window = SDL_CreateWindow(\"RB BASIC Graphics\",");
        writeln("                          SDL_WINDOWPOS_CENTERED,");
        writeln("                          SDL_WINDOWPOS_CENTERED,");
        writeln("                          screen_width, screen_height,");
        writeln("                          SDL_WINDOW_SHOWN);");
        writeln("if (!window) {");
        indent_level++;
        writeln("printf(\"Window creation failed: %s\\n\", SDL_GetError());");
        writeln("SDL_Quit();");
        writeln("exit(1);");
        indent_level--;
        writeln("}");
        writeln("");
        writeln("renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);");
        writeln("if (!renderer) {");
        indent_level++;
        writeln("printf(\"Renderer creation failed: %s\\n\", SDL_GetError());");
        writeln("SDL_DestroyWindow(window);");
        writeln("SDL_Quit();");
        writeln("exit(1);");
        indent_level--;
        writeln("}");
        writeln("");
        writeln("// Clear screen to black");
        writeln("SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);");
        writeln("SDL_RenderClear(renderer);");
        writeln("SDL_RenderPresent(renderer);");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Cleanup graphics
        writeln("void cleanup_graphics() {");
        indent_level++;
        writeln("if (renderer) SDL_DestroyRenderer(renderer);");
        writeln("if (window) SDL_DestroyWindow(window);");
        writeln("SDL_Quit();");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Wait for window close or ESC key
        writeln("void wait_for_close() {");
        indent_level++;
        writeln("if (!window) return;");
        writeln("SDL_Event event;");
        writeln("int running = 1;");
        writeln("while (running) {");
        indent_level++;
        writeln("while (SDL_PollEvent(&event)) {");
        indent_level++;
        writeln("if (event.type == SDL_QUIT) {");
        indent_level++;
        writeln("running = 0;");
        indent_level--;
        writeln("}");
        writeln("if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {");
        indent_level++;
        writeln("running = 0;");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("SDL_Delay(10);  // Small delay to prevent high CPU usage");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Get palette color
        writeln("SDL_Color get_palette_color(int color_index) {");
        indent_level++;
        writeln("if (color_index < 0) color_index = 0;");
        writeln("if (color_index > 15) color_index = 15;");
        writeln("return palette[color_index];");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Draw circle using Bresenham's algorithm
        writeln("void draw_circle(int cx, int cy, int radius, int color) {");
        indent_level++;
        writeln("SDL_Color c = get_palette_color(color);");
        writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
        writeln("");
        writeln("int x = 0;");
        writeln("int y = radius;");
        writeln("int d = 3 - 2 * radius;");
        writeln("");
        writeln("while (x <= y) {");
        indent_level++;
        writeln("// Draw 8 octants");
        writeln("SDL_RenderDrawPoint(renderer, cx + x, cy + y);");
        writeln("SDL_RenderDrawPoint(renderer, cx - x, cy + y);");
        writeln("SDL_RenderDrawPoint(renderer, cx + x, cy - y);");
        writeln("SDL_RenderDrawPoint(renderer, cx - x, cy - y);");
        writeln("SDL_RenderDrawPoint(renderer, cx + y, cy + x);");
        writeln("SDL_RenderDrawPoint(renderer, cx - y, cy + x);");
        writeln("SDL_RenderDrawPoint(renderer, cx + y, cy - x);");
        writeln("SDL_RenderDrawPoint(renderer, cx - y, cy - x);");
        writeln("");
        writeln("if (d < 0) {");
        indent_level++;
        writeln("d = d + 4 * x + 6;");
        indent_level--;
        writeln("} else {");
        indent_level++;
        writeln("d = d + 4 * (x - y) + 10;");
        writeln("y--;");
        indent_level--;
        writeln("}");
        writeln("x++;");
        indent_level--;
        writeln("}");
        writeln("SDL_RenderPresent(renderer);");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Get pixel color (for POINT function)
        writeln("int get_pixel_color(int x, int y) {");
        indent_level++;
        writeln("// For now, return current_color as SDL2 doesn't easily read back pixels");
        writeln("// A full implementation would need to maintain a pixel buffer");
        writeln("return current_color;");
        indent_level--;
        writeln("}");
        writeln("");
    }
}

void CodeGenerator::generate_user_subs_and_functions(const Program* program) {
    // Generate SUB definitions
    for (const auto& sub : program->subs) {
        writeln("// SUB " + sub->name);
        write("void SUB_" + sub->name + "(");
        
        for (size_t i = 0; i < sub->parameters.size(); i++) {
            if (i > 0) write(", ");
            std::string param = sub->parameters[i];
            if (!param.empty() && param.back() == '$') {
                write("char* " + param);
            } else {
                write("double* " + param);
            }
        }
        
        write(") {\n");
        indent_level++;
        
        for (const auto& stmt : sub->body) {
            generate_statement(stmt.get());
        }
        
        indent_level--;
        writeln("}");
        writeln("");
    }
    
    // Generate FUNCTION definitions
    for (const auto& func : program->functions) {
        writeln("// FUNCTION " + func->name);
        
        std::string ret_type = "double";
        if (!func->name.empty() && func->name.back() == '$') {
            ret_type = "const char*";
        }
        
        write(ret_type + " FUNC_" + func->name + "(");
        
        for (size_t i = 0; i < func->parameters.size(); i++) {
            if (i > 0) write(", ");
            std::string param = func->parameters[i];
            if (!param.empty() && param.back() == '$') {
                write("char* " + param);
            } else {
                write("double " + param);
            }
        }
        
        write(") {\n");
        indent_level++;
        
        writeln("static " + ret_type + " _result;");
        
        for (const auto& stmt : func->body) {
            generate_statement(stmt.get());
        }
        
        writeln("return _result;");
        
        indent_level--;
        writeln("}");
        writeln("");
    }
}

void CodeGenerator::generate_def_fns(const Program* program) {
    if (program->def_fns.empty()) return;
    
    writeln("// DEF FN definitions");
    
    for (const auto& def_fn : program->def_fns) {
        std::string fn_name = def_fn->name;
        
        // Determine return type
        bool is_string = (!fn_name.empty() && fn_name.back() == '$');
        std::string ret_type = is_string ? "const char*" : "double";
        
        // Generate function signature
        write(ret_type + " " + fn_name + "(");
        
        for (size_t i = 0; i < def_fn->parameters.size(); i++) {
            if (i > 0) write(", ");
            std::string param = def_fn->parameters[i];
            if (!param.empty() && param.back() == '$') {
                write("const char* " + param);
            } else {
                write("double " + param);
            }
        }
        
        write(") {\n");
        indent_level++;
        
        if (is_string) {
            writeln("static char _result[256];");
        }
        
        write(get_indent() + "return ");
        generate_expression(def_fn->expression.get());
        write(";\n");
        
        indent_level--;
        writeln("}");
        writeln("");
    }
}


// ============== EXPRESSION GENERATION ==============

void CodeGenerator::generate_function_call(const FunctionCallNode* func) {
    std::string fname = func->name;
    
    // Math functions
    if (fname == "SIN" || fname == "COS" || fname == "TAN" || fname == "LOG" || fname == "EXP") {
        write(fname == "TAN" ? "tan" : fname == "SIN" ? "sin" : fname == "COS" ? "cos" : 
              fname == "LOG" ? "log" : "exp");
        write("(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "ATN") {
        write("atan(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "SQR") {
        write("sqrt(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "ABS") {
        write("fabs(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "INT") {
        write("floor(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "SGN") {
        write("((double)((");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(") > 0 ? 1.0 : (");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(") < 0 ? -1.0 : 0.0))");
    }
    else if (fname == "RND") {
        write("RB_RND()");
    }
    
    // String functions
    else if (fname == "LEFT$") {
        // LEFT$(str$, n) -> needs temp variable
        write("(RB_LEFT(_temp_str, ");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get(), true);
        write(", ");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get());
        write("), _temp_str)");
    }
    else if (fname == "RIGHT$") {
        write("(RB_RIGHT(_temp_str, ");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get(), true);
        write(", ");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get());
        write("), _temp_str)");
    }
    else if (fname == "MID$") {
        write("(RB_MID(_temp_str, ");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get(), true);
        write(", ");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get());
        write(", ");
        if (func->arguments.size() >= 3) {
            generate_expression(func->arguments[2].get());
        } else {
            write("255");
        }
        write("), _temp_str)");
    }
    else if (fname == "LEN") {
        write("((double)strlen(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get(), true);
        }
        write("))");
    }
    else if (fname == "INSTR") {
        write("RB_INSTR(");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get(), true);
        write(", ");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get(), true);
        write(")");
    }
    else if (fname == "CHR$") {
        write("(RB_CHR(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write("), _temp_str)");
    }
    else if (fname == "ASC") {
        write("(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get(), true);
        }
        write("[0])");
    }
    else if (fname == "STR$") {
        write("(RB_STR(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write("), _temp_str)");
    }
    else if (fname == "VAL") {
        write("atof(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get(), true);
        }
        write(")");
    }
    else if (fname == "UCASE$") {
        write("(RB_UCASE(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get(), true);
        }
        write("), _temp_str)");
    }
    else if (fname == "LCASE$") {
        write("(RB_LCASE(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get(), true);
        }
        write("), _temp_str)");
    }
    
    // Type conversion functions
    else if (fname == "CINT") {
        write("(double)(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(" + 0.5)");  // Round to nearest, then cast to double
    }
    else if (fname == "FIX") {
        write("(double)(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");  // Truncate, then cast to double
    }
    
    // Number formatting functions
    else if (fname == "HEX$") {
        write("(RB_HEX(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write("), _temp_str)");
    }
    else if (fname == "OCT$") {
        write("(RB_OCT(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write("), _temp_str)");
    }
    else if (fname == "TAB") {
        // TAB(n) creates a string of spaces to move to column n
        write("(RB_TAB(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write("), _temp_str)");
    }
    
    // System functions
    else if (fname == "INPUT$") {
        // INPUT$(n) - reads n characters from keyboard
        write("(RB_INPUT_STR(_temp_str, ");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        } else {
            write("1");
        }
        write("), _temp_str)");
    }
    else if (fname == "INKEY$") {
        write("(RB_INKEY(_temp_str), _temp_str)");
    }
    else if (fname == "DATE$") {
        write("(RB_GET_DATE(_temp_str), _temp_str)");
    }
    else if (fname == "TIME$") {
        write("(RB_GET_TIME(_temp_str), _temp_str)");
    }
    else if (fname == "TIMER") {
        write("RB_TIMER()");
    }
    
    // Array bounds functions (simplified - would need array metadata in real implementation)
    else if (fname == "LBOUND") {
        write("0");  // GW-BASIC arrays start at 0 by default
    }
    else if (fname == "UBOUND") {
        write("/* UBOUND needs implementation */ 10");
    }
    
    // File functions
    else if (fname == "EOF") {
        write("feof(_file_handles[");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write("])");
    }
    else if (fname == "LOF") {
        write("/* LOF not fully implemented */ 0");
    }
    
    // Graphics functions
    else if (fname == "POINT") {
        needs_sdl = true;
        write("get_pixel_color(");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get());
        write(", ");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get());
        write(")");
    }
    
    // Unknown function - might be user-defined
    else {
        // Check if it's a DEF FN function (starts with FN)
        if (fname.substr(0, 2) == "FN") {
            write(fname + "(");
        } else {
            write("FUNC_" + fname + "(");
        }
        for (size_t i = 0; i < func->arguments.size(); i++) {
            if (i > 0) write(", ");
            generate_expression(func->arguments[i].get());
        }
        write(")");
    }
}

// Helper function to check if expression is a string type
bool CodeGenerator::is_string_expr(const ASTNode* expr) {
    if (!expr) return false;
    
    switch (expr->type) {
        case NodeType::STRING:
            return true;
        case NodeType::VARIABLE: {
            const VariableNode* var = static_cast<const VariableNode*>(expr);
            return var->is_string;
        }
        case NodeType::ARRAY_ACCESS: {
            const ArrayAccessNode* arr = static_cast<const ArrayAccessNode*>(expr);
            return arr->is_string;
        }
        case NodeType::FUNCTION_CALL: {
            const FunctionCallNode* func = static_cast<const FunctionCallNode*>(expr);
            // String functions end with $ OR special functions that return strings
            if (func->name.back() == '$') return true;
            // TAB(), SPACE$, STRING$ return strings
            if (func->name == "TAB" || func->name == "SPACE$" || func->name == "STRING$") {
                return true;
            }
            return false;
        }
        case NodeType::BINARY_OP: {
            const BinaryOpNode* binop = static_cast<const BinaryOpNode*>(expr);
            // String concatenation
            if (binop->op == "+") {
                return is_string_expr(binop->left.get()) || is_string_expr(binop->right.get());
            }
            return false;
        }
        default:
            return false;
    }
}

void CodeGenerator::generate_expression(const ASTNode* expr, bool is_string_context) {
    if (!expr) {
        write("0");
        return;
    }
    
    switch (expr->type) {
        case NodeType::NUMBER: {
            const NumberNode* num = static_cast<const NumberNode*>(expr);
            write(std::to_string(num->value));
            break;
        }
        
        case NodeType::STRING: {
            const StringNode* str = static_cast<const StringNode*>(expr);
            write("\"" + str->value + "\"");
            break;
        }
        
        case NodeType::VARIABLE: {
            const VariableNode* var = static_cast<const VariableNode*>(expr);
            std::string var_name = var->name;
            
            // Handle system functions that are used without parentheses
            if (var_name == "DATE$") {
                write("(RB_GET_DATE(_temp_str), _temp_str)");
                break;
            }
            if (var_name == "TIME$") {
                write("(RB_GET_TIME(_temp_str), _temp_str)");
                break;
            }
            if (var_name == "TIMER") {
                write("RB_TIMER()");
                break;
            }
            if (var_name == "INKEY$") {
                write("(RB_INKEY(_temp_str), _temp_str)");
                break;
            }
            
            // Replace $ with _str suffix for C variable names
            if (var->is_string && !var_name.empty() && var_name.back() == '$') {
                var_name.pop_back();
                var_name += "_str";
            }
            
            // Rename CY to rb_CY to avoid Windows conflict
            if (needs_sdl && var_name == "CY") {
                var_name = "rb_CY";
            }
            
            write(var_name);
            break;
        }
        
        case NodeType::ARRAY_ACCESS: {
            const ArrayAccessNode* arr = static_cast<const ArrayAccessNode*>(expr);
            std::string arr_name = arr->name;
            
            // Replace $ with _str suffix for C array names
            if (arr->is_string && !arr_name.empty() && arr_name.back() == '$') {
                arr_name.pop_back();
                arr_name += "_str";
            }
            
            write(arr_name);
            for (const auto& idx : arr->indices) {
                write("[");
                write("(int)(");
                generate_expression(idx.get());
                write(")");
                write("]");
            }
            break;
        }
        
        case NodeType::BINARY_OP: {
            const BinaryOpNode* binop = static_cast<const BinaryOpNode*>(expr);
            
            // Check if we're comparing strings
            bool left_is_string = is_string_expr(binop->left.get());
            bool right_is_string = is_string_expr(binop->right.get());
            bool is_string_comparison = (left_is_string || right_is_string) && 
                                       (binop->op == "=" || binop->op == "<>" || 
                                        binop->op == "<" || binop->op == ">" ||
                                        binop->op == "<=" || binop->op == ">=");
            
            write("(");
            
            // Special handling for string concatenation
            if (binop->op == "+" && is_string_context) {
                // Use RB_CONCAT helper with _temp_str3 as destination
                write("RB_CONCAT(_temp_str3, ");
                generate_expression(binop->left.get(), true);
                write(", ");
                generate_expression(binop->right.get(), true);
                write(")");
            }
            // String comparisons need strcmp()
            else if (is_string_comparison) {
                write("strcmp(");
                generate_expression(binop->left.get(), true);
                write(", ");
                generate_expression(binop->right.get(), true);
                write(")");
                
                // Map BASIC comparison to C comparison result
                if (binop->op == "=") {
                    write(" == 0");
                } else if (binop->op == "<>") {
                    write(" != 0");
                } else if (binop->op == "<") {
                    write(" < 0");
                } else if (binop->op == ">") {
                    write(" > 0");
                } else if (binop->op == "<=") {
                    write(" <= 0");
                } else if (binop->op == ">=") {
                    write(" >= 0");
                }
            }
            // Logical operators
            else if (binop->op == "AND") {
                generate_expression(binop->left.get());
                write(" && ");
                generate_expression(binop->right.get());
            }
            else if (binop->op == "OR") {
                generate_expression(binop->left.get());
                write(" || ");
                generate_expression(binop->right.get());
            }
            else if (binop->op == "MOD") {
                write("fmod(");
                generate_expression(binop->left.get());
                write(", ");
                generate_expression(binop->right.get());
                write(")");
            }
            else if (binop->op == "^") {
                write("pow(");
                generate_expression(binop->left.get());
                write(", ");
                generate_expression(binop->right.get());
                write(")");
                needs_math_h = true;
            }
            else if (binop->op == "<>") {
                generate_expression(binop->left.get());
                write(" != ");
                generate_expression(binop->right.get());
            }
            else if (binop->op == "=") {
                // BASIC = is comparison, C needs ==
                generate_expression(binop->left.get());
                write(" == ");
                generate_expression(binop->right.get());
            }
            else {
                generate_expression(binop->left.get());
                write(" " + binop->op + " ");
                generate_expression(binop->right.get());
            }
            
            write(")");
            break;
        }
        
        case NodeType::UNARY_OP: {
            const UnaryOpNode* unary = static_cast<const UnaryOpNode*>(expr);
            
            if (unary->op == "NOT") {
                write("!(");
                generate_expression(unary->operand.get());
                write(")");
            } else {
                write("(");
                write(unary->op);
                generate_expression(unary->operand.get());
                write(")");
            }
            break;
        }
        
        case NodeType::FUNCTION_CALL: {
            const FunctionCallNode* func = static_cast<const FunctionCallNode*>(expr);
            generate_function_call(func);
            break;
        }
        
        default:
            throw std::runtime_error("Unknown expression type in code generation");
    }
}


// ============== STATEMENT GENERATION ==============

void CodeGenerator::generate_print_stmt(const PrintStmt* stmt) {
    if (stmt->file_number > 0) {
        // File output
        for (size_t i = 0; i < stmt->expressions.size(); i++) {
            write(get_indent());
            
            const ASTNode* expr = stmt->expressions[i].get();
            
            // Determine if this is a string expression
            bool is_string = is_string_expr(expr);
            
            if (is_string) {
                write("fprintf(_file_handles[" + std::to_string(stmt->file_number) + "], \"%s\", ");
                generate_expression(expr, true);
                write(");\n");
            } else {
                write("fprintf(_file_handles[" + std::to_string(stmt->file_number) + "], \"%g\", ");
                generate_expression(expr);
                write(");\n");
            }
            
            if (i < stmt->use_semicolon.size() && !stmt->use_semicolon[i]) {
                writeln("fprintf(_file_handles[" + std::to_string(stmt->file_number) + "], \" \");");
            }
        }
        writeln("fprintf(_file_handles[" + std::to_string(stmt->file_number) + "], \"\\n\");");
        return;
    }
    
    // Console output
    if (stmt->expressions.empty()) {
        writeln("printf(\"\\n\");");
        writeln("fflush(stdout);");
        return;
    }
    
    for (size_t i = 0; i < stmt->expressions.size(); i++) {
        const ASTNode* expr = stmt->expressions[i].get();
        
        write(get_indent());
        if (expr->type == NodeType::STRING) {
            write("printf(\"%s\", ");
            generate_expression(expr);
            write(");\n");
        } else if (expr->type == NodeType::VARIABLE) {
            const VariableNode* var = static_cast<const VariableNode*>(expr);
            if (var->is_string) {
                write("printf(\"%s\", ");
                generate_expression(expr, true);
                write(");\n");
            } else {
                write("printf(\"%g\", ");
                generate_expression(expr);
                write(");\n");
            }
        } else if (expr->type == NodeType::ARRAY_ACCESS) {
            const ArrayAccessNode* arr = static_cast<const ArrayAccessNode*>(expr);
            if (arr->is_string) {
                write("printf(\"%s\", ");
                generate_expression(expr, true);
                write(");\n");
            } else {
                write("printf(\"%g\", ");
                generate_expression(expr);
                write(");\n");
            }
        } else if (expr->type == NodeType::FUNCTION_CALL) {
            // Use is_string_expr to properly detect string-returning functions
            if (is_string_expr(expr)) {
                write("printf(\"%s\", ");
                generate_expression(expr, true);
                write(");\n");
            } else {
                write("printf(\"%g\", ");
                generate_expression(expr);
                write(");\n");
            }
        } else {
            write("printf(\"%g\", ");
            generate_expression(expr);
            write(");\n");
        }
        
        if (i < stmt->use_semicolon.size() && !stmt->use_semicolon[i]) {
            if (i < stmt->expressions.size() - 1) {
                writeln("printf(\" \");");
            }
        }
    }
    
    writeln("printf(\"\\n\");");
    writeln("fflush(stdout);  // Ensure output appears immediately");
}

void CodeGenerator::generate_let_stmt(const LetStmt* stmt) {
    std::string var_name = stmt->variable;
    bool is_string = (!var_name.empty() && var_name.back() == '$');
    
    // Replace $ with _str suffix for C variable names
    if (is_string) {
        var_name.pop_back();
        var_name += "_str";
    }
    
    if (stmt->array_access) {
        // Array assignment
        const ArrayAccessNode* arr = static_cast<const ArrayAccessNode*>(stmt->array_access.get());
        std::string arr_name = arr->name;
        if (!arr_name.empty() && arr_name.back() == '$') {
            arr_name.pop_back();
            arr_name += "_str";
        }
        
        if (is_string) {
            // String array: strcpy(arr[i], value)
            write(get_indent() + "strcpy(" + arr_name);
            for (const auto& idx : arr->indices) {
                write("[(int)(");
                generate_expression(idx.get());
                write(")]");
            }
            write(", ");
            generate_expression(stmt->expression.get(), true);
            write(");\n");
        } else {
            // Numeric array
            write(get_indent() + arr_name);
            for (const auto& idx : arr->indices) {
                write("[(int)(");
                generate_expression(idx.get());
                write(")]");
            }
            write(" = ");
            generate_expression(stmt->expression.get());
            write(";\n");
        }
    } else {
        // Simple variable assignment
        if (is_string) {
            write(get_indent() + "strcpy(" + var_name + ", ");
            generate_expression(stmt->expression.get(), true);
            write(");\n");
        } else {
            // Rename CY to rb_CY to avoid Windows conflict
            if (needs_sdl && var_name == "CY") {
                var_name = "rb_CY";
            }
            write(get_indent() + var_name + " = ");
            generate_expression(stmt->expression.get());
            write(";\n");
        }
    }
}

void CodeGenerator::generate_if_stmt(const IfStmt* stmt) {
    if (!stmt->is_block) {
        // Old-style IF THEN linenum
        write(get_indent() + "if (");
        generate_expression(stmt->condition.get());
        write(") {\n");
        
        indent_level++;
        
        auto it = line_labels.find(stmt->target_line);
        if (it != line_labels.end()) {
            writeln("goto L" + std::to_string(it->second) + ";");
        } else {
            writeln("// Warning: Target line " + std::to_string(stmt->target_line) + " not found");
        }
        
        indent_level--;
        writeln("}");
    } else {
        // Block IF...END IF
        write(get_indent() + "if (");
        generate_expression(stmt->condition.get());
        write(") {\n");
        
        indent_level++;
        for (const auto& then_stmt : stmt->then_body) {
            generate_statement(then_stmt.get());
        }
        indent_level--;
        
        if (!stmt->else_body.empty()) {
            writeln("} else {");
            indent_level++;
            for (const auto& else_stmt : stmt->else_body) {
                generate_statement(else_stmt.get());
            }
            indent_level--;
        }
        
        writeln("}");
    }
}

void CodeGenerator::generate_goto_stmt(const GotoStmt* stmt) {
    auto it = line_labels.find(stmt->target_line);
    if (it != line_labels.end()) {
        writeln("goto L" + std::to_string(it->second) + ";");
    } else {
        writeln("// Warning: Target line " + std::to_string(stmt->target_line) + " not found");
    }
}

void CodeGenerator::generate_end_stmt(const EndStmt* /* stmt */) {
    if (needs_sdl) {
        writeln("wait_for_close();  // Wait for user to close window or press ESC");
        writeln("cleanup_graphics();");
    }
    writeln("exit(0);");
}

void CodeGenerator::generate_for_stmt(const ForStmt* stmt) {
    std::string var = stmt->variable;
    
    numeric_variables.insert(var);
    
    // Rename CY to rb_CY to avoid Windows conflict
    std::string c_var = var;
    if (needs_sdl && var == "CY") {
        c_var = "rb_CY";
    }
    
    write(get_indent() + "for (" + c_var + " = ");
    generate_expression(stmt->start.get());
    write("; ");
    
    if (stmt->step) {
        write("(");
        generate_expression(stmt->step.get());
        write(" >= 0 ? " + c_var + " <= ");
        generate_expression(stmt->end.get());
        write(" : " + c_var + " >= ");
        generate_expression(stmt->end.get());
        write(")");
    } else {
        write(c_var + " <= ");
        generate_expression(stmt->end.get());
    }
    write("; ");
    
    if (stmt->step) {
        write(c_var + " += ");
        generate_expression(stmt->step.get());
    } else {
        write(c_var + " += 1");
    }
    write(") {\n");
    
    indent_level++;
    
    for (const auto& body_stmt : stmt->body) {
        generate_statement(body_stmt.get());
    }
    
    // Generate label for NEXT if it has a line number
    // BUT only if it's different from the FOR's line number (to avoid duplicates)
    if (stmt->next_line_num > 0 && stmt->next_line_num != stmt->line_number) {
        auto it = line_labels.find(stmt->next_line_num);
        if (it != line_labels.end()) {
            indent_level--;
            writeln("L" + std::to_string(it->second) + ":;");
            indent_level++;
        }
    }
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_while_stmt(const WhileStmt* stmt) {
    write(get_indent() + "while (");
    generate_expression(stmt->condition.get());
    write(") {\n");
    
    indent_level++;
    
    for (const auto& body_stmt : stmt->body) {
        generate_statement(body_stmt.get());
    }
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_do_loop_stmt(const DoLoopStmt* stmt) {
    if (stmt->check_at_start) {
        // DO WHILE/UNTIL
        write(get_indent() + "while (");
        if (stmt->is_until) {
            write("!(");
            generate_expression(stmt->condition.get());
            write(")");
        } else {
            generate_expression(stmt->condition.get());
        }
        write(") {\n");
        
        indent_level++;
        for (const auto& body_stmt : stmt->body) {
            generate_statement(body_stmt.get());
        }
        indent_level--;
        writeln("}");
    } else {
        // DO...LOOP WHILE/UNTIL
        writeln("do {");
        indent_level++;
        
        for (const auto& body_stmt : stmt->body) {
            generate_statement(body_stmt.get());
        }
        
        indent_level--;
        write(get_indent() + "} while (");
        
        if (stmt->condition) {
            if (stmt->is_until) {
                write("!(");
                generate_expression(stmt->condition.get());
                write(")");
            } else {
                generate_expression(stmt->condition.get());
            }
        } else {
            write("1");  // Infinite loop
        }
        write(");\n");
    }
}

void CodeGenerator::generate_gosub_stmt(const GosubStmt* stmt) {
    auto it = line_labels.find(stmt->target_line);
    if (it != line_labels.end()) {
        writeln("_gosub_stack[_gosub_sp++] = " + std::to_string(next_label) + ";");
        writeln("goto L" + std::to_string(it->second) + ";");
        writeln("RETURN_" + std::to_string(next_label) + ":;");
        next_label++;
    } else {
        writeln("// Warning: Target line " + std::to_string(stmt->target_line) + " not found");
    }
}

void CodeGenerator::generate_return_stmt(const ReturnStmt* /* stmt */) {
    writeln("if (_gosub_sp > 0) {");
    writeln("    switch (_gosub_stack[--_gosub_sp]) {");
    
    // We'll need to track all RETURN labels - for now, generate a simple return
    for (int i = 0; i < next_label; i++) {
        writeln("        case " + std::to_string(i) + ": goto RETURN_" + std::to_string(i) + ";");
    }
    
    writeln("    }");
    writeln("}");
}

void CodeGenerator::generate_input_stmt(const InputStmt* stmt) {
    if (stmt->file_number > 0) {
        // File input
        for (const auto& var : stmt->variables) {
            std::string var_name = var;
            bool is_string = (!var_name.empty() && var_name.back() == '$');
            
            if (is_string) {
                var_name.pop_back();
                var_name += "_str";
            }
            
            // Rename CY to rb_CY to avoid Windows conflict
            if (needs_sdl && var_name == "CY") {
                var_name = "rb_CY";
            }
            
            if (is_string) {
                writeln("fscanf(_file_handles[" + std::to_string(stmt->file_number) + 
                       "], \"%255s\", " + var_name + ");");
            } else {
                writeln("fscanf(_file_handles[" + std::to_string(stmt->file_number) + 
                       "], \"%lf\", &" + var_name + ");");
            }
        }
        return;
    }
    
    // Console input
    if (!stmt->prompt.empty()) {
        writeln("printf(\"%s\", \"" + stmt->prompt + "\");");
        writeln("fflush(stdout);  // Ensure prompt displays immediately");
    }
    
    for (const auto& var : stmt->variables) {
        std::string var_name = var;
        bool is_string = (!var_name.empty() && var_name.back() == '$');
        
        if (is_string) {
            var_name.pop_back();
            var_name += "_str";
        }
        
        // Rename CY to rb_CY to avoid Windows conflict
        if (needs_sdl && var_name == "CY") {
            var_name = "rb_CY";
        }
        
        if (is_string) {
            // For string input, use fgets to read full line (not just one word)
            writeln("if (fgets(" + var_name + ", 255, stdin) != NULL) {");
            indent_level++;
            writeln(var_name + "[strcspn(" + var_name + ", \"\\n\")] = 0;  // Remove newline");
            indent_level--;
            writeln("}");
        } else {
            writeln("scanf(\"%lf\", &" + var_name + ");");
            // Clear the input buffer after reading a number
            writeln("while (getchar() != '\\n');  // Clear input buffer");
        }
    }
}

void CodeGenerator::generate_line_input_stmt(const LineInputStmt* stmt) {
    if (!stmt->prompt.empty()) {
        writeln("printf(\"%s\", \"" + stmt->prompt + "\");");
        writeln("fflush(stdout);  // Ensure prompt displays immediately");
    }
    
    std::string var_name = stmt->variable;
    if (!var_name.empty() && var_name.back() == '$') {
        var_name.pop_back();
        var_name += "_str";
    }
    
    if (stmt->file_number > 0) {
        // Read from file
        writeln("if (fgets(" + var_name + ", 255, _file_handles[" + std::to_string(stmt->file_number) + "]) != NULL) {");
        indent_level++;
        writeln(var_name + "[strcspn(" + var_name + ", \"\\n\")] = 0;  // Remove newline");
        indent_level--;
        writeln("}");
    } else {
        // Read from stdin
        writeln("fgets(" + var_name + ", 255, stdin);");
        writeln(var_name + "[strcspn(" + var_name + ", \"\\n\")] = 0;  // Remove newline");
    }
}

void CodeGenerator::generate_read_stmt(const ReadStmt* stmt) {
    for (const auto& target : stmt->variables) {
        std::string var_name = target.name;
        bool is_string = (!var_name.empty() && var_name.back() == '$');
        
        if (is_string) {
            var_name.pop_back();
            var_name += "_str";
        }
        
        writeln("if (_data_index < _data_count) {");
        indent_level++;
        
        // Build the target (variable or array element)
        if (target.is_array()) {
            // Array access: A(I) or A$(X,Y)
            // We need to write the assignment directly without building a string first
            if (is_string) {
                write(get_indent() + "strcpy(" + var_name);
                for (const auto& idx : target.indices) {
                    write("[(int)(");
                    generate_expression(idx.get());
                    write(")]");
                }
                write(", _data_values[_data_index++]);\n");
            } else {
                write(get_indent() + var_name);
                for (const auto& idx : target.indices) {
                    write("[(int)(");
                    generate_expression(idx.get());
                    write(")]");
                }
                write(" = atof(_data_values[_data_index++]);\n");
            }
        } else {
            // Simple variable
            if (is_string) {
                writeln("strcpy(" + var_name + ", _data_values[_data_index++]);");
            } else {
                writeln(var_name + " = atof(_data_values[_data_index++]);");
            }
        }
        
        indent_level--;
        writeln("}");
    }
}

void CodeGenerator::generate_restore_stmt(const RestoreStmt* /* stmt */) {
    writeln("_data_index = 0;");
}

void CodeGenerator::generate_dim_stmt(const DimStmt* /* stmt */) {
    // Arrays are already declared globally
    // This statement is essentially a no-op in the generated code
}

void CodeGenerator::generate_open_stmt(const OpenStmt* stmt) {
    std::string mode = "r";
    if (stmt->mode == "OUTPUT") {
        mode = "w";
    } else if (stmt->mode == "APPEND") {
        mode = "a";
    }
    
    writeln("_file_handles[" + std::to_string(stmt->file_number) + "] = fopen(\"" + 
            stmt->filename + "\", \"" + mode + "\");");
}

void CodeGenerator::generate_close_stmt(const CloseStmt* stmt) {
    if (stmt->file_numbers.empty()) {
        writeln("// Close all files");
        writeln("for (int i = 0; i < 256; i++) {");
        writeln("    if (_file_handles[i]) fclose(_file_handles[i]);");
        writeln("    _file_handles[i] = NULL;");
        writeln("}");
    } else {
        for (int fnum : stmt->file_numbers) {
            writeln("if (_file_handles[" + std::to_string(fnum) + "]) {");
            writeln("    fclose(_file_handles[" + std::to_string(fnum) + "]);");
            writeln("    _file_handles[" + std::to_string(fnum) + "] = NULL;");
            writeln("}");
        }
    }
}

void CodeGenerator::generate_call_stmt(const CallStmt* stmt) {
    write(get_indent() + "SUB_" + stmt->name + "(");
    
    for (size_t i = 0; i < stmt->arguments.size(); i++) {
        if (i > 0) write(", ");
        
        // Pass addresses for parameters
        const ASTNode* arg = stmt->arguments[i].get();
        if (arg->type == NodeType::VARIABLE) {
            const VariableNode* var = static_cast<const VariableNode*>(arg);
            if (var->is_string) {
                std::string var_name = var->name;
                if (!var_name.empty() && var_name.back() == '$') {
                    var_name.pop_back();
                }
                write(var_name);
            } else {
                write("&" + var->name);
            }
        } else {
            write("&/* expression */");
        }
    }
    
    write(");\n");
}

// ============== PHASE 4: SCREEN CONTROL STATEMENTS ==============

void CodeGenerator::generate_cls_stmt(const ClsStmt* stmt) {
    (void)stmt;  // unused
    
    // Only check for graphics mode if SDL is being used
    if (needs_sdl) {
        // If in graphics mode, clear the SDL renderer
        writeln("if (renderer != NULL) {");
        indent_level++;
        writeln("SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);");
        writeln("SDL_RenderClear(renderer);");
        writeln("SDL_RenderPresent(renderer);");
        indent_level--;
        writeln("} else {");
        indent_level++;
        writeln("RB_CLS();");
        indent_level--;
        writeln("}");
    } else {
        // Text mode only - just use console clear
        writeln("RB_CLS();");
    }
}

void CodeGenerator::generate_locate_stmt(const LocateStmt* stmt) {
    write(get_indent() + "RB_LOCATE(");
    if (stmt->row) {
        write("(int)(");
        generate_expression(stmt->row.get());
        write(")");
    } else {
        write("1");
    }
    write(", ");
    if (stmt->col) {
        write("(int)(");
        generate_expression(stmt->col.get());
        write(")");
    } else {
        write("1");
    }
    write(");\n");
}

void CodeGenerator::generate_color_stmt(const ColorStmt* stmt) {
    write(get_indent() + "RB_COLOR(");
    if (stmt->foreground) {
        write("(int)(");
        generate_expression(stmt->foreground.get());
        write(")");
    } else {
        write("7");  // Default white
    }
    write(", ");
    if (stmt->background) {
        write("(int)(");
        generate_expression(stmt->background.get());
        write(")");
    } else {
        write("0");  // Default black
    }
    write(");\n");
}

void CodeGenerator::generate_key_stmt(const KeyStmt* stmt) {
    (void)stmt;  // unused
    // KEY OFF/ON is a DOS command to hide/show function keys
    // We ignore it - it's a no-op in modern environments
    writeln("// KEY " + stmt->mode + " (ignored - DOS-only feature)");
}

void CodeGenerator::generate_beep_stmt(const BeepStmt* stmt) {
    (void)stmt;  // unused
    writeln("#ifdef _WIN32");
    writeln("MessageBeep(0xFFFFFFFF);  // MB_OK");
    writeln("#else");
    writeln("printf(\"\\a\");  // Bell character");
    writeln("#endif");
}

void CodeGenerator::generate_sleep_stmt(const SleepStmt* stmt) {
    writeln("#ifdef _WIN32");
    write(get_indent() + "Sleep((DWORD)(");
    generate_expression(stmt->duration.get());
    write(" * 1000));\n");  // Sleep takes milliseconds on Windows
    writeln("#else");
    write(get_indent() + "usleep((unsigned int)(");
    generate_expression(stmt->duration.get());
    write(" * 1000000));\n");  // usleep takes microseconds on Unix
    writeln("#endif");
}

void CodeGenerator::generate_randomize_stmt(const RandomizeStmt* stmt) {
    write(get_indent() + "srand(");
    if (stmt->seed) {
        write("(unsigned int)(");
        generate_expression(stmt->seed.get());
        write(")");
    } else {
        write("(unsigned)time(NULL)");
    }
    write(");\n");
    writeln("_rnd_initialized = 1;");
}

// ============== PHASE 4: ADVANCED CONTROL FLOW ==============

void CodeGenerator::generate_on_goto_stmt(const OnGotoStmt* stmt) {
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _on_index = (int)(");
    generate_expression(stmt->expression.get());
    write(");\n");
    
    writeln("switch(_on_index) {");
    indent_level++;
    
    for (size_t i = 0; i < stmt->target_lines.size(); i++) {
        int target_line = stmt->target_lines[i];
        writeln("case " + std::to_string(i + 1) + ":");
        indent_level++;
        
        auto it = line_labels.find(target_line);
        if (it != line_labels.end()) {
            writeln("goto L" + std::to_string(it->second) + ";");
        } else {
            writeln("// Warning: Target line " + std::to_string(target_line) + " not found");
        }
        writeln("break;");
        indent_level--;
    }
    
    writeln("default:");
    indent_level++;
    writeln("break;");
    indent_level--;
    
    indent_level--;
    writeln("}");
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_on_gosub_stmt(const OnGosubStmt* stmt) {
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _on_index = (int)(");
    generate_expression(stmt->expression.get());
    write(");\n");
    
    writeln("switch(_on_index) {");
    indent_level++;
    
    for (size_t i = 0; i < stmt->target_lines.size(); i++) {
        int target_line = stmt->target_lines[i];
        writeln("case " + std::to_string(i + 1) + ":");
        indent_level++;
        
        auto it = line_labels.find(target_line);
        if (it != line_labels.end()) {
            writeln("_gosub_stack[_gosub_sp++] = __LINE__;");
            writeln("goto L" + std::to_string(it->second) + ";");
        } else {
            writeln("// Warning: Target line " + std::to_string(target_line) + " not found");
        }
        writeln("break;");
        indent_level--;
    }
    
    writeln("default:");
    indent_level++;
    writeln("break;");
    indent_level--;
    
    indent_level--;
    writeln("}");
    
    indent_level--;
    writeln("}");
}

// ============== GRAPHICS STATEMENT GENERATORS ==============

void CodeGenerator::generate_screen_stmt(const ScreenStmt* stmt) {
    needs_sdl = true;
    
    write(get_indent() + "init_graphics((int)(");
    generate_expression(stmt->mode.get());
    write("));\n");
}

void CodeGenerator::generate_pset_stmt(const PsetStmt* stmt) {
    needs_sdl = true;
    
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _x = (int)(");
    generate_expression(stmt->x.get());
    write(");\n");
    
    write(get_indent() + "int _y = (int)(");
    generate_expression(stmt->y.get());
    write(");\n");
    
    if (stmt->color) {
        write(get_indent() + "int _color = (int)(");
        generate_expression(stmt->color.get());
        write(");\n");
        writeln("current_color = _color;");
    }
    
    writeln("SDL_Color c = get_palette_color(current_color);");
    writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
    writeln("SDL_RenderDrawPoint(renderer, _x, _y);");
    writeln("SDL_RenderPresent(renderer);");
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_line_stmt(const LineStmt* stmt) {
    needs_sdl = true;
    
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _x1 = (int)(");
    generate_expression(stmt->x1.get());
    write(");\n");
    
    write(get_indent() + "int _y1 = (int)(");
    generate_expression(stmt->y1.get());
    write(");\n");
    
    write(get_indent() + "int _x2 = (int)(");
    generate_expression(stmt->x2.get());
    write(");\n");
    
    write(get_indent() + "int _y2 = (int)(");
    generate_expression(stmt->y2.get());
    write(");\n");
    
    if (stmt->color) {
        write(get_indent() + "int _color = (int)(");
        generate_expression(stmt->color.get());
        write(");\n");
        writeln("current_color = _color;");
    }
    
    writeln("SDL_Color c = get_palette_color(current_color);");
    writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
    
    if (stmt->is_box) {
        if (stmt->is_filled) {
            // Draw filled box
            writeln("SDL_Rect rect = {_x1, _y1, _x2 - _x1, _y2 - _y1};");
            writeln("SDL_RenderFillRect(renderer, &rect);");
        } else {
            // Draw box outline
            writeln("SDL_RenderDrawLine(renderer, _x1, _y1, _x2, _y1);");
            writeln("SDL_RenderDrawLine(renderer, _x2, _y1, _x2, _y2);");
            writeln("SDL_RenderDrawLine(renderer, _x2, _y2, _x1, _y2);");
            writeln("SDL_RenderDrawLine(renderer, _x1, _y2, _x1, _y1);");
        }
    } else {
        // Draw simple line
        writeln("SDL_RenderDrawLine(renderer, _x1, _y1, _x2, _y2);");
    }
    
    writeln("SDL_RenderPresent(renderer);");
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_circle_stmt(const CircleStmt* stmt) {
    needs_sdl = true;
    
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _cx = (int)(");
    generate_expression(stmt->x.get());
    write(");\n");
    
    write(get_indent() + "int _cy = (int)(");
    generate_expression(stmt->y.get());
    write(");\n");
    
    write(get_indent() + "int _radius = (int)(");
    generate_expression(stmt->radius.get());
    write(");\n");
    
    if (stmt->color) {
        write(get_indent() + "int _color = (int)(");
        generate_expression(stmt->color.get());
        write(");\n");
    } else {
        writeln("int _color = current_color;");
    }
    
    writeln("draw_circle(_cx, _cy, _radius, _color);");
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_paint_stmt(const PaintStmt* stmt) {
    needs_sdl = true;
    
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _x = (int)(");
    generate_expression(stmt->x.get());
    write(");\n");
    
    write(get_indent() + "int _y = (int)(");
    generate_expression(stmt->y.get());
    write(");\n");
    
    write(get_indent() + "int _fill_color = (int)(");
    generate_expression(stmt->paint_color.get());
    write(");\n");
    
    if (stmt->border_color) {
        write(get_indent() + "int _border_color = (int)(");
        generate_expression(stmt->border_color.get());
        write(");\n");
    } else {
        writeln("int _border_color = _fill_color;");
    }
    
    writeln("// PAINT flood fill - simplified version");
    writeln("SDL_Color c = get_palette_color(_fill_color);");
    writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
    writeln("// Full flood fill implementation would go here");
    writeln("SDL_RenderDrawPoint(renderer, _x, _y);");
    writeln("SDL_RenderPresent(renderer);");
    
    indent_level--;
    writeln("}");
}

// ============== SOUND STATEMENT GENERATORS ==============

void CodeGenerator::generate_sound_stmt(const SoundStmt* stmt) {
    needs_sound = true;
    
    writeln("{");
    indent_level++;
    
    write(get_indent() + "int _freq = (int)(");
    generate_expression(stmt->frequency.get());
    write(");\n");
    
    write(get_indent() + "int _duration = (int)(");
    generate_expression(stmt->duration.get());
    write(");\n");
    
    writeln("#ifdef _WIN32");
    writeln("Beep(_freq, (_duration * 1000) / 18);  // Convert clock ticks to ms");
    writeln("#else");
    writeln("// TODO: Linux sound implementation");
    writeln("#endif");
    
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_play_stmt(const PlayStmt* stmt) {
    needs_sound = true;
    
    writeln("{");
    indent_level++;
    
    writeln("// PLAY statement - MML parser would go here");
    write(get_indent() + "// Music string: ");
    generate_expression(stmt->music_string.get());
    write("\n");
    writeln("// Simplified: just beep for now");
    writeln("#ifdef _WIN32");
    writeln("Beep(440, 250);");
    writeln("#endif");
    
    indent_level--;
    writeln("}");
}


void CodeGenerator::generate_statement(const Statement* stmt) {
    if (stmt->line_num > 0) {
        auto it = line_labels.find(stmt->line_num);
        if (it != line_labels.end()) {
            if (indent_level > 0) indent_level--;
            writeln("L" + std::to_string(it->second) + ":;");
            if (indent_level < 2) indent_level++;
        }
    }
    
    if (!stmt->stmt) return;
    
    // Store current statement line number for use by nested structures
    int current_stmt_line = stmt->line_num;
    
    switch (stmt->stmt->type) {
        case NodeType::PRINT_STMT:
            generate_print_stmt(static_cast<const PrintStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::REM_STMT: {
            const RemStmt* rem = static_cast<const RemStmt*>(stmt->stmt.get());
            writeln("// " + rem->comment);
            break;
        }
        
        case NodeType::LET_STMT:
            generate_let_stmt(static_cast<const LetStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::IF_STMT:
            generate_if_stmt(static_cast<const IfStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::GOTO_STMT:
            generate_goto_stmt(static_cast<const GotoStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::FOR_STMT: {
            // Pass current line number to FOR loop for duplicate detection
            ForStmt* for_stmt = static_cast<ForStmt*>(stmt->stmt.get());
            for_stmt->line_number = current_stmt_line;  // Set the line number
            generate_for_stmt(for_stmt);
            break;
        }
            
        case NodeType::WHILE_STMT:
            generate_while_stmt(static_cast<const WhileStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::DO_LOOP_STMT:
            generate_do_loop_stmt(static_cast<const DoLoopStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::GOSUB_STMT:
            generate_gosub_stmt(static_cast<const GosubStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::RETURN_STMT:
            generate_return_stmt(static_cast<const ReturnStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::INPUT_STMT:
            generate_input_stmt(static_cast<const InputStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::LINE_INPUT_STMT:
            generate_line_input_stmt(static_cast<const LineInputStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::READ_STMT:
            generate_read_stmt(static_cast<const ReadStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::RESTORE_STMT:
            generate_restore_stmt(static_cast<const RestoreStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::DIM_STMT:
            generate_dim_stmt(static_cast<const DimStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::OPEN_STMT:
            generate_open_stmt(static_cast<const OpenStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::CLOSE_STMT:
            generate_close_stmt(static_cast<const CloseStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::CALL_STMT:
            generate_call_stmt(static_cast<const CallStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::CLS_STMT:
            generate_cls_stmt(static_cast<const ClsStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::LOCATE_STMT:
            generate_locate_stmt(static_cast<const LocateStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::COLOR_STMT:
            generate_color_stmt(static_cast<const ColorStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::KEY_STMT:
            generate_key_stmt(static_cast<const KeyStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::RANDOMIZE_STMT:
            generate_randomize_stmt(static_cast<const RandomizeStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::BEEP_STMT:
            generate_beep_stmt(static_cast<const BeepStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::SLEEP_STMT:
            generate_sleep_stmt(static_cast<const SleepStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::ON_GOTO_STMT:
            generate_on_goto_stmt(static_cast<const OnGotoStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::ON_GOSUB_STMT:
            generate_on_gosub_stmt(static_cast<const OnGosubStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::SCREEN_STMT:
            generate_screen_stmt(static_cast<const ScreenStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::PSET_STMT:
            generate_pset_stmt(static_cast<const PsetStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::LINE_STMT:
            generate_line_stmt(static_cast<const LineStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::CIRCLE_STMT:
            generate_circle_stmt(static_cast<const CircleStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::PAINT_STMT:
            generate_paint_stmt(static_cast<const PaintStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::SOUND_STMT:
            generate_sound_stmt(static_cast<const SoundStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::PLAY_STMT:
            generate_play_stmt(static_cast<const PlayStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::END_STMT:
            generate_end_stmt(static_cast<const EndStmt*>(stmt->stmt.get()));
            break;
            
        case NodeType::DATA_STMT:
            // DATA statements are collected and processed separately
            break;
            
        case NodeType::DEF_FN_STMT:
            // DEF FN statements are generated as macros at the top
            break;
            
        default:
            throw std::runtime_error("Unknown statement type in code generation: " + 
                                   std::to_string(static_cast<int>(stmt->stmt->type)));
    }
}

void CodeGenerator::generate(const Program* program) {
    // First, detect if graphics or sound are used
    detect_graphics_sound(program);
    
    // Collect all necessary information
    collect_variables(program);
    collect_line_labels(program);
    collect_data_values(program);
    
    // Generate code
    generate_headers();
    
    // Generate variable declarations FIRST (SDL helpers need these globals)
    generate_variable_declarations();
    generate_array_declarations();
    generate_data_initialization();
    
    // Generate helper functions (after variables so SDL globals exist)
    generate_builtin_functions();
    
    // Generate DEF FN definitions
    generate_def_fns(program);
    
    // Generate user-defined SUBs and FUNCTIONs
    generate_user_subs_and_functions(program);
    
    // Main function
    writeln("int main(int argc, char* argv[]) {");
    writeln("    (void)argc;");
    writeln("    (void)argv;");
    indent_level++;
    
    // Always declare temp string if we have string variables, string arrays, or use string functions
    if (!string_variables.empty() || !string_arrays.empty() || needs_string_h) {
        writeln("char _temp_str[256];");
        writeln("char _temp_str2[256];");  // For nested string operations
        writeln("char _temp_str3[256];");  // For deeply nested operations
        writeln("");
    }
    
    // Generate main program statements
    for (const auto& stmt : program->statements) {
        generate_statement(stmt.get());
    }
    
    writeln("");
    writeln("return 0;");
    
    indent_level--;
    writeln("}");
    
    std::cout << "Code generation complete." << std::endl;
}