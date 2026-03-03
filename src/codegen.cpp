#include "codegen.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <functional>

// Ensure all AST node types are visible
using std::unique_ptr;

CodeGenerator::CodeGenerator(const std::string& filename)
    : error_check_counter(0), has_error_handling(false), current_basic_line(0),
      next_label(0), indent_level(0), needs_math_h(false),
      needs_string_h(false), needs_time_h(false), needs_sdl(false), needs_sound(false),
      has_display_stmt(false), needs_sdl_image(false), needs_sdl_mixer(false), needs_console(false) {
    
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

std::string CodeGenerator::escape_string(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);  // Reserve space for potential escapes
    
    for (char c : str) {
        if (c == '\\') {
            result += "\\\\";  // Escape backslash
        } else if (c == '"') {
            result += "\\\"";  // Escape double quote
        } else if (c == '\n') {
            result += "\\n";   // Escape newline
        } else if (c == '\r') {
            result += "\\r";   // Escape carriage return
        } else if (c == '\t') {
            result += "\\t";   // Escape tab
        } else {
            result += c;
        }
    }
    
    return result;
}

// Sanitize variable names to avoid C/Windows reserved words
std::string CodeGenerator::sanitize_varname(const std::string& name) {
    // List of C/Windows reserved words that conflict
    static const std::unordered_set<std::string> reserved = {
        "SIZE", "POINT", "RECT", "LINE", "WINDOW", "ERROR", "STATUS",
        "FILE", "TEXT", "COLOR", "BOOL", "CHAR", "SHORT", "LONG",
        "FLOAT", "DOUBLE", "VOID", "INT", "CONST", "STATIC", "EXTERN",
        "AUTO", "REGISTER", "VOLATILE", "SIGNED", "UNSIGNED",
        "STRUCT", "UNION", "ENUM", "TYPEDEF", "SIZEOF",
        "IF", "ELSE", "SWITCH", "CASE", "DEFAULT", "FOR", "WHILE",
        "DO", "BREAK", "CONTINUE", "RETURN", "GOTO"
    };
    
    // Check if this is a reserved word
    if (reserved.find(name) != reserved.end()) {
        return "BAS_" + name;  // Prefix with BAS_ to avoid conflicts
    }
    
    return name;
}

// ============== VARIABLE COLLECTION ==============

void CodeGenerator::collect_expr_variables(const ASTNode* expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case NodeType::VARIABLE: {
            const VariableNode* var = static_cast<const VariableNode*>(expr);

            // Skip struct member access (handled by typed variable)
            if (var->name.find('.') != std::string::npos) break;

            // Skip built-in pseudo-variables
            if (var->name == "ERR" || var->name == "ERL") break;

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
            
            case NodeType::OPEN_STMT: {
                const OpenStmt* open_stmt = static_cast<const OpenStmt*>(body_stmt->stmt.get());
                // Collect variables from filename expression
                if (open_stmt->filename) {
                    collect_expr_variables(open_stmt->filename.get());
                }
                break;
            }

            case NodeType::SELECT_CASE_STMT: {
                const SelectCaseStmt* sel = static_cast<const SelectCaseStmt*>(body_stmt->stmt.get());
                collect_expr_variables(sel->test_expression.get());
                for (const auto& clause : sel->cases) {
                    for (const auto& test : clause.tests) {
                        if (test.value) collect_expr_variables(test.value.get());
                        if (test.range_end) collect_expr_variables(test.range_end.get());
                    }
                    collect_stmt_body_variables(clause.body);
                }
                break;
            }

            case NodeType::DRAW_STMT: {
                const DrawStmt* draw = static_cast<const DrawStmt*>(body_stmt->stmt.get());
                collect_expr_variables(draw->command_string.get());
                break;
            }

            case NodeType::PALETTE_STMT: {
                const PaletteStmt* pal = static_cast<const PaletteStmt*>(body_stmt->stmt.get());
                if (pal->attribute) collect_expr_variables(pal->attribute.get());
                if (pal->color) collect_expr_variables(pal->color.get());
                break;
            }

            case NodeType::VIEW_STMT: {
                const ViewStmt* view = static_cast<const ViewStmt*>(body_stmt->stmt.get());
                if (view->x1) collect_expr_variables(view->x1.get());
                if (view->y1) collect_expr_variables(view->y1.get());
                if (view->x2) collect_expr_variables(view->x2.get());
                if (view->y2) collect_expr_variables(view->y2.get());
                break;
            }

            case NodeType::WINDOW_STMT: {
                const WindowStmt* win = static_cast<const WindowStmt*>(body_stmt->stmt.get());
                if (win->x1) collect_expr_variables(win->x1.get());
                if (win->y1) collect_expr_variables(win->y1.get());
                if (win->x2) collect_expr_variables(win->x2.get());
                if (win->y2) collect_expr_variables(win->y2.get());
                break;
            }

            case NodeType::PCOPY_STMT: {
                const PcopyStmt* pc = static_cast<const PcopyStmt*>(body_stmt->stmt.get());
                collect_expr_variables(pc->src_page.get());
                collect_expr_variables(pc->dest_page.get());
                break;
            }

            case NodeType::GET_GFX_STMT: {
                const GetGfxStmt* gg = static_cast<const GetGfxStmt*>(body_stmt->stmt.get());
                collect_expr_variables(gg->x1.get());
                collect_expr_variables(gg->y1.get());
                collect_expr_variables(gg->x2.get());
                collect_expr_variables(gg->y2.get());
                break;
            }

            case NodeType::PUT_GFX_STMT: {
                const PutGfxStmt* pg = static_cast<const PutGfxStmt*>(body_stmt->stmt.get());
                collect_expr_variables(pg->x.get());
                collect_expr_variables(pg->y.get());
                break;
            }

            case NodeType::SHELL_STMT: {
                const ShellStmt* sh = static_cast<const ShellStmt*>(body_stmt->stmt.get());
                if (sh->command) collect_expr_variables(sh->command.get());
                break;
            }

            case NodeType::POKE_STMT: {
                const PokeStmt* pk = static_cast<const PokeStmt*>(body_stmt->stmt.get());
                collect_expr_variables(pk->address.get());
                collect_expr_variables(pk->value.get());
                break;
            }

            case NodeType::DEF_SEG_STMT: {
                const DefSegStmt* ds = static_cast<const DefSegStmt*>(body_stmt->stmt.get());
                if (ds->segment) collect_expr_variables(ds->segment.get());
                break;
            }

            // Phase 6: QB64 Extensions
            case NodeType::LIMIT_STMT: {
                const LimitStmt* ls = static_cast<const LimitStmt*>(body_stmt->stmt.get());
                collect_expr_variables(ls->fps.get());
                break;
            }
            case NodeType::FREEIMAGE_STMT: {
                const FreeImageStmt* fi = static_cast<const FreeImageStmt*>(body_stmt->stmt.get());
                collect_expr_variables(fi->handle.get());
                break;
            }
            case NodeType::PUTIMAGE_STMT: {
                const PutImageStmt* pi = static_cast<const PutImageStmt*>(body_stmt->stmt.get());
                if (pi->dx) collect_expr_variables(pi->dx.get());
                if (pi->dy) collect_expr_variables(pi->dy.get());
                collect_expr_variables(pi->source_handle.get());
                if (pi->dest_handle) collect_expr_variables(pi->dest_handle.get());
                break;
            }
            case NodeType::PRINTSTRING_STMT: {
                const PrintStringStmt* ps = static_cast<const PrintStringStmt*>(body_stmt->stmt.get());
                collect_expr_variables(ps->x.get());
                collect_expr_variables(ps->y.get());
                collect_expr_variables(ps->text.get());
                break;
            }
            case NodeType::SNDPLAY_STMT: {
                const SndPlayStmt* sp = static_cast<const SndPlayStmt*>(body_stmt->stmt.get());
                collect_expr_variables(sp->handle.get());
                break;
            }
            case NodeType::SNDSTOP_STMT: {
                const SndStopStmt* ss = static_cast<const SndStopStmt*>(body_stmt->stmt.get());
                collect_expr_variables(ss->handle.get());
                break;
            }
            case NodeType::SNDVOL_STMT: {
                const SndVolStmt* sv = static_cast<const SndVolStmt*>(body_stmt->stmt.get());
                collect_expr_variables(sv->handle.get());
                collect_expr_variables(sv->volume.get());
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
            case NodeType::SCREEN_STMT: {
                // SCREEN 0 is text mode, not graphics
                // Only modes 1-13 require SDL graphics
                const ScreenStmt* screen = static_cast<const ScreenStmt*>(stmt->stmt.get());
                if (screen->mode && screen->mode->type == NodeType::NUMBER) {
                    const NumberNode* num = static_cast<const NumberNode*>(screen->mode.get());
                    if (num->value > 0) {
                        needs_sdl = true;
                    }
                }
                break;
            }
            case NodeType::PSET_STMT:
            case NodeType::LINE_STMT:
            case NodeType::CIRCLE_STMT:
            case NodeType::PAINT_STMT:
            case NodeType::DRAW_STMT:
            case NodeType::PALETTE_STMT:
            case NodeType::VIEW_STMT:
            case NodeType::WINDOW_STMT:
            case NodeType::PCOPY_STMT:
            case NodeType::GET_GFX_STMT:
            case NodeType::PUT_GFX_STMT:
                needs_sdl = true;
                break;
                
            case NodeType::SOUND_STMT:
            case NodeType::PLAY_STMT:
            case NodeType::BEEP_STMT:   // BEEP needs windows.h on Windows
            case NodeType::SLEEP_STMT:  // SLEEP needs windows.h on Windows
                needs_sound = true;
                break;

            // Phase 6: QB64 Extensions
            case NodeType::DISPLAY_STMT:
                needs_sdl = true;
                has_display_stmt = true;
                break;
            case NodeType::LIMIT_STMT:
            case NodeType::FREEIMAGE_STMT:
            case NodeType::PUTIMAGE_STMT:
            case NodeType::PRINTSTRING_STMT:
                needs_sdl = true;
                break;
            case NodeType::SNDPLAY_STMT:
            case NodeType::SNDSTOP_STMT:
            case NodeType::SNDVOL_STMT:
                needs_sdl_mixer = true;
                break;

            default:
                break;
        }

        // Check if graphics cleared (CLS in graphics mode)
        if (stmt->stmt->type == NodeType::CLS_STMT && needs_sdl) {
            // CLS will use SDL
        }
    }

    // Also scan function calls for QB64 functions that need libraries
    for (const auto& stmt : program->statements) {
        if (!stmt->stmt) continue;
        // Check LET statements for function calls
        if (stmt->stmt->type == NodeType::LET_STMT) {
            const LetStmt* let_stmt = static_cast<const LetStmt*>(stmt->stmt.get());
            if (let_stmt->expression && let_stmt->expression->type == NodeType::FUNCTION_CALL) {
                const FunctionCallNode* func = static_cast<const FunctionCallNode*>(let_stmt->expression.get());
                if (func->name == "_LOADIMAGE") {
                    needs_sdl = true;
                    needs_sdl_image = true;
                } else if (func->name == "_NEWIMAGE" || func->name == "_RGB" ||
                           func->name == "_RGB32" || func->name == "_MOUSEX" ||
                           func->name == "_MOUSEY" || func->name == "_MOUSEBUTTON" ||
                           func->name == "_KEYDOWN" || func->name == "_MOUSEWHEEL") {
                    needs_sdl = true;
                } else if (func->name == "_SNDOPEN") {
                    needs_sdl_mixer = true;
                }
            }
        }
    }

    // Detect console I/O usage (INPUT, LINE INPUT, PRINT without file number)
    // Needed to allocate a console window for graphics programs that also need text I/O
    for (const auto& stmt : program->statements) {
        if (!stmt->stmt) continue;
        if (stmt->stmt->type == NodeType::INPUT_STMT ||
            stmt->stmt->type == NodeType::LINE_INPUT_STMT ||
            stmt->stmt->type == NodeType::PRINT_STMT) {
            needs_console = true;
            break;
        }
    }
    if (!needs_console) {
        for (const auto& sub : program->subs) {
            for (const auto& stmt : sub->body) {
                if (!stmt || !stmt->stmt) continue;
                if (stmt->stmt->type == NodeType::INPUT_STMT ||
                    stmt->stmt->type == NodeType::LINE_INPUT_STMT ||
                    stmt->stmt->type == NodeType::PRINT_STMT) {
                    needs_console = true;
                    break;
                }
            }
            if (needs_console) break;
        }
    }
    if (!needs_console) {
        for (const auto& func : program->functions) {
            for (const auto& stmt : func->body) {
                if (!stmt || !stmt->stmt) continue;
                if (stmt->stmt->type == NodeType::INPUT_STMT ||
                    stmt->stmt->type == NodeType::LINE_INPUT_STMT ||
                    stmt->stmt->type == NodeType::PRINT_STMT) {
                    needs_console = true;
                    break;
                }
            }
            if (needs_console) break;
        }
    }

    // Also scan SUB and FUNCTION bodies for graphics/sound usage
    for (const auto& sub : program->subs) {
        for (const auto& stmt : sub->body) {
            if (!stmt || !stmt->stmt) continue;
            switch (stmt->stmt->type) {
                case NodeType::SCREEN_STMT: {
                    // SCREEN 0 is text mode, not graphics
                    const ScreenStmt* screen = static_cast<const ScreenStmt*>(stmt->stmt.get());
                    if (screen->mode && screen->mode->type == NodeType::NUMBER) {
                        const NumberNode* num = static_cast<const NumberNode*>(screen->mode.get());
                        if (num->value > 0) {
                            needs_sdl = true;
                        }
                    }
                    break;
                }
                case NodeType::PSET_STMT:
                case NodeType::LINE_STMT:
                case NodeType::CIRCLE_STMT:
                case NodeType::PAINT_STMT:
                case NodeType::DRAW_STMT:
                case NodeType::PALETTE_STMT:
                case NodeType::VIEW_STMT:
                case NodeType::WINDOW_STMT:
                case NodeType::PCOPY_STMT:
                case NodeType::GET_GFX_STMT:
                case NodeType::PUT_GFX_STMT:
                case NodeType::DISPLAY_STMT:
                case NodeType::LIMIT_STMT:
                case NodeType::FREEIMAGE_STMT:
                case NodeType::PUTIMAGE_STMT:
                case NodeType::PRINTSTRING_STMT:
                    needs_sdl = true;
                    if (stmt->stmt->type == NodeType::DISPLAY_STMT) has_display_stmt = true;
                    break;
                case NodeType::SOUND_STMT:
                case NodeType::PLAY_STMT:
                case NodeType::BEEP_STMT:
                case NodeType::SLEEP_STMT:
                    needs_sound = true;
                    break;
                case NodeType::SNDPLAY_STMT:
                case NodeType::SNDSTOP_STMT:
                case NodeType::SNDVOL_STMT:
                    needs_sdl_mixer = true;
                    break;
                default:
                    break;
            }
            // Check LET statements in SUBs for QB64 function calls
            if (stmt->stmt->type == NodeType::LET_STMT) {
                const LetStmt* let_stmt = static_cast<const LetStmt*>(stmt->stmt.get());
                if (let_stmt->expression && let_stmt->expression->type == NodeType::FUNCTION_CALL) {
                    const FunctionCallNode* func = static_cast<const FunctionCallNode*>(let_stmt->expression.get());
                    if (func->name == "_LOADIMAGE") {
                        needs_sdl = true;
                        needs_sdl_image = true;
                    } else if (func->name == "_NEWIMAGE" || func->name == "_RGB" ||
                               func->name == "_RGB32") {
                        needs_sdl = true;
                    } else if (func->name == "_SNDOPEN") {
                        needs_sdl_mixer = true;
                    }
                }
            }
        }
    }
    for (const auto& func : program->functions) {
        for (const auto& stmt : func->body) {
            if (!stmt || !stmt->stmt) continue;
            switch (stmt->stmt->type) {
                case NodeType::SCREEN_STMT: {
                    const ScreenStmt* screen = static_cast<const ScreenStmt*>(stmt->stmt.get());
                    if (screen->mode && screen->mode->type == NodeType::NUMBER) {
                        const NumberNode* num = static_cast<const NumberNode*>(screen->mode.get());
                        if (num->value > 0) {
                            needs_sdl = true;
                        }
                    }
                    break;
                }
                case NodeType::PSET_STMT:
                case NodeType::LINE_STMT:
                case NodeType::CIRCLE_STMT:
                case NodeType::PAINT_STMT:
                case NodeType::DISPLAY_STMT:
                case NodeType::LIMIT_STMT:
                case NodeType::FREEIMAGE_STMT:
                case NodeType::PUTIMAGE_STMT:
                case NodeType::PRINTSTRING_STMT:
                    needs_sdl = true;
                    break;
                case NodeType::SNDPLAY_STMT:
                case NodeType::SNDSTOP_STMT:
                case NodeType::SNDVOL_STMT:
                    needs_sdl_mixer = true;
                    break;
                default:
                    break;
            }
            if (stmt->stmt->type == NodeType::LET_STMT) {
                const LetStmt* let_stmt = static_cast<const LetStmt*>(stmt->stmt.get());
                if (let_stmt->expression && let_stmt->expression->type == NodeType::FUNCTION_CALL) {
                    const FunctionCallNode* fn = static_cast<const FunctionCallNode*>(let_stmt->expression.get());
                    if (fn->name == "_LOADIMAGE") {
                        needs_sdl = true;
                        needs_sdl_image = true;
                    } else if (fn->name == "_NEWIMAGE" || fn->name == "_RGB" ||
                               fn->name == "_RGB32") {
                        needs_sdl = true;
                    } else if (fn->name == "_SNDOPEN") {
                        needs_sdl_mixer = true;
                    }
                }
            }
        }
    }
}

void CodeGenerator::collect_variables(const Program* program) {
    // First pass: collect DIM statements to know which are arrays
    for (const auto& stmt : program->statements) {
        if (stmt->stmt && stmt->stmt->type == NodeType::DIM_STMT) {
            const DimStmt* dim = static_cast<const DimStmt*>(stmt->stmt.get());
            
            // Handle new multi-array format with expression-based dimensions
            for (const auto& array_decl : dim->arrays) {
                // Skip typed variable declarations (DIM var AS TypeName)
                if (!array_decl.type_name.empty()) {
                    typed_variables[sanitize_varname(array_decl.name)] = array_decl.type_name;
                    continue;
                }

                ArrayInfo info;
                info.is_dynamic = false;

                // Check if any dimension is non-constant
                for (const auto& dim_expr : array_decl.dimension_exprs) {
                    info.dimension_exprs.push_back(dim_expr.get());

                    // Mark as dynamic if not a simple number literal
                    if (dim_expr->type != NodeType::NUMBER) {
                        info.is_dynamic = true;
                    }
                }

                if (array_decl.is_string) {
                    string_arrays[array_decl.name] = info;
                } else {
                    arrays[array_decl.name] = info;
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

                // Skip struct member access (handled by typed variable)
                if (let->variable.find('.') != std::string::npos) {
                    collect_expr_variables(let->expression.get());
                    break;
                }

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
            
            case NodeType::OPEN_STMT: {
                const OpenStmt* open_stmt = static_cast<const OpenStmt*>(stmt->stmt.get());
                // Collect variables from filename expression
                if (open_stmt->filename) {
                    collect_expr_variables(open_stmt->filename.get());
                }
                break;
            }
            
            case NodeType::LINE_STMT: {
                const LineStmt* line_stmt = static_cast<const LineStmt*>(stmt->stmt.get());
                collect_expr_variables(line_stmt->x1.get());
                collect_expr_variables(line_stmt->y1.get());
                collect_expr_variables(line_stmt->x2.get());
                collect_expr_variables(line_stmt->y2.get());
                if (line_stmt->color) {
                    collect_expr_variables(line_stmt->color.get());
                }
                break;
            }
            
            case NodeType::CIRCLE_STMT: {
                const CircleStmt* circle_stmt = static_cast<const CircleStmt*>(stmt->stmt.get());
                collect_expr_variables(circle_stmt->x.get());
                collect_expr_variables(circle_stmt->y.get());
                collect_expr_variables(circle_stmt->radius.get());
                if (circle_stmt->color) {
                    collect_expr_variables(circle_stmt->color.get());
                }
                if (circle_stmt->start_angle) {
                    collect_expr_variables(circle_stmt->start_angle.get());
                }
                if (circle_stmt->end_angle) {
                    collect_expr_variables(circle_stmt->end_angle.get());
                }
                if (circle_stmt->aspect) {
                    collect_expr_variables(circle_stmt->aspect.get());
                }
                break;
            }
            
            case NodeType::PSET_STMT: {
                const PsetStmt* pset_stmt = static_cast<const PsetStmt*>(stmt->stmt.get());
                collect_expr_variables(pset_stmt->x.get());
                collect_expr_variables(pset_stmt->y.get());
                if (pset_stmt->color) {
                    collect_expr_variables(pset_stmt->color.get());
                }
                break;
            }
            
            case NodeType::PAINT_STMT: {
                const PaintStmt* paint_stmt = static_cast<const PaintStmt*>(stmt->stmt.get());
                collect_expr_variables(paint_stmt->x.get());
                collect_expr_variables(paint_stmt->y.get());
                collect_expr_variables(paint_stmt->paint_color.get());
                if (paint_stmt->border_color) {
                    collect_expr_variables(paint_stmt->border_color.get());
                }
                break;
            }
            
            case NodeType::LOCATE_STMT: {
                const LocateStmt* loc_stmt = static_cast<const LocateStmt*>(stmt->stmt.get());
                if (loc_stmt->row) {
                    collect_expr_variables(loc_stmt->row.get());
                }
                if (loc_stmt->col) {
                    collect_expr_variables(loc_stmt->col.get());
                }
                if (loc_stmt->cursor) {
                    collect_expr_variables(loc_stmt->cursor.get());
                }
                break;
            }
            
            case NodeType::COLOR_STMT: {
                const ColorStmt* col_stmt = static_cast<const ColorStmt*>(stmt->stmt.get());
                if (col_stmt->foreground) {
                    collect_expr_variables(col_stmt->foreground.get());
                }
                if (col_stmt->background) {
                    collect_expr_variables(col_stmt->background.get());
                }
                if (col_stmt->border) {
                    collect_expr_variables(col_stmt->border.get());
                }
                break;
            }
            
            case NodeType::SCREEN_STMT: {
                const ScreenStmt* screen_stmt = static_cast<const ScreenStmt*>(stmt->stmt.get());
                if (screen_stmt->mode) {
                    collect_expr_variables(screen_stmt->mode.get());
                }
                break;
            }
            
            case NodeType::SOUND_STMT: {
                const SoundStmt* sound_stmt = static_cast<const SoundStmt*>(stmt->stmt.get());
                collect_expr_variables(sound_stmt->frequency.get());
                collect_expr_variables(sound_stmt->duration.get());
                break;
            }
            
            case NodeType::PLAY_STMT: {
                const PlayStmt* play_stmt = static_cast<const PlayStmt*>(stmt->stmt.get());
                collect_expr_variables(play_stmt->music_string.get());
                break;
            }
            
            case NodeType::SLEEP_STMT: {
                const SleepStmt* sleep_stmt = static_cast<const SleepStmt*>(stmt->stmt.get());
                if (sleep_stmt->duration) {
                    collect_expr_variables(sleep_stmt->duration.get());
                }
                break;
            }

            case NodeType::RANDOMIZE_STMT: {
                const RandomizeStmt* rand_stmt = static_cast<const RandomizeStmt*>(stmt->stmt.get());
                if (rand_stmt->seed) {
                    collect_expr_variables(rand_stmt->seed.get());
                }
                break;
            }

            case NodeType::SELECT_CASE_STMT: {
                const SelectCaseStmt* sel = static_cast<const SelectCaseStmt*>(stmt->stmt.get());
                collect_expr_variables(sel->test_expression.get());
                for (const auto& clause : sel->cases) {
                    for (const auto& test : clause.tests) {
                        if (test.value) collect_expr_variables(test.value.get());
                        if (test.range_end) collect_expr_variables(test.range_end.get());
                    }
                    collect_stmt_body_variables(clause.body);
                }
                break;
            }

            case NodeType::CONST_STMT: {
                const ConstStmt* cst = static_cast<const ConstStmt*>(stmt->stmt.get());
                // Register as a variable so it gets declared
                if (!cst->name.empty() && cst->name.back() == '$') {
                    string_variables.insert(cst->name);
                } else {
                    numeric_variables.insert(cst->name);
                }
                collect_expr_variables(cst->value.get());
                break;
            }

            case NodeType::SWAP_STMT: {
                const SwapStmt* sw = static_cast<const SwapStmt*>(stmt->stmt.get());
                // Register both variables
                auto register_var = [this](const std::string& name) {
                    bool is_arr = (arrays.find(name) != arrays.end()) ||
                                 (string_arrays.find(name) != string_arrays.end());
                    if (!is_arr) {
                        if (!name.empty() && name.back() == '$') {
                            string_variables.insert(name);
                        } else {
                            numeric_variables.insert(name);
                        }
                    }
                };
                register_var(sw->var1);
                register_var(sw->var2);
                break;
            }

            case NodeType::ON_ERROR_STMT: {
                const OnErrorStmt* onerr = static_cast<const OnErrorStmt*>(stmt->stmt.get());
                if (onerr->target_line > 0) {
                    has_error_handling = true;
                    on_error_targets.insert(onerr->target_line);
                }
                break;
            }

            case NodeType::WRITE_STMT: {
                const WriteStmt* ws = static_cast<const WriteStmt*>(stmt->stmt.get());
                for (const auto& expr : ws->expressions) {
                    collect_expr_variables(expr.get());
                }
                break;
            }

            case NodeType::SEEK_STMT: {
                const SeekStmt* sk = static_cast<const SeekStmt*>(stmt->stmt.get());
                collect_expr_variables(sk->position.get());
                break;
            }

            case NodeType::GET_FILE_STMT: {
                const GetFileStmt* gf = static_cast<const GetFileStmt*>(stmt->stmt.get());
                if (gf->record_number) collect_expr_variables(gf->record_number.get());
                if (!gf->variable.empty()) {
                    if (!gf->variable.empty() && gf->variable.back() == '$') {
                        string_variables.insert(gf->variable);
                    } else {
                        numeric_variables.insert(gf->variable);
                    }
                }
                break;
            }

            case NodeType::PUT_FILE_STMT: {
                const PutFileStmt* pf = static_cast<const PutFileStmt*>(stmt->stmt.get());
                if (pf->record_number) collect_expr_variables(pf->record_number.get());
                break;
            }

            case NodeType::MID_ASSIGN_STMT: {
                const MidAssignStmt* ma = static_cast<const MidAssignStmt*>(stmt->stmt.get());
                string_variables.insert(ma->variable);
                collect_expr_variables(ma->start.get());
                if (ma->length) collect_expr_variables(ma->length.get());
                collect_expr_variables(ma->replacement.get());
                break;
            }

            case NodeType::REDIM_STMT: {
                const RedimStmt* rd = static_cast<const RedimStmt*>(stmt->stmt.get());
                for (const auto& arr_decl : rd->arrays) {
                    for (const auto& dim_expr : arr_decl.dimension_exprs) {
                        collect_expr_variables(dim_expr.get());
                    }
                    // Register or upgrade array to dynamic
                    ArrayInfo info;
                    info.is_dynamic = true;
                    for (const auto& dim_expr : arr_decl.dimension_exprs) {
                        info.dimension_exprs.push_back(dim_expr.get());
                    }
                    if (arr_decl.is_string) {
                        // Force dynamic even if DIM declared it as static
                        string_arrays[arr_decl.name] = info;
                    } else {
                        arrays[arr_decl.name] = info;
                    }
                }
                break;
            }

            case NodeType::DRAW_STMT: {
                const DrawStmt* draw = static_cast<const DrawStmt*>(stmt->stmt.get());
                collect_expr_variables(draw->command_string.get());
                break;
            }

            case NodeType::PALETTE_STMT: {
                const PaletteStmt* pal = static_cast<const PaletteStmt*>(stmt->stmt.get());
                if (pal->attribute) collect_expr_variables(pal->attribute.get());
                if (pal->color) collect_expr_variables(pal->color.get());
                break;
            }

            case NodeType::VIEW_STMT: {
                const ViewStmt* view = static_cast<const ViewStmt*>(stmt->stmt.get());
                if (view->x1) collect_expr_variables(view->x1.get());
                if (view->y1) collect_expr_variables(view->y1.get());
                if (view->x2) collect_expr_variables(view->x2.get());
                if (view->y2) collect_expr_variables(view->y2.get());
                break;
            }

            case NodeType::WINDOW_STMT: {
                const WindowStmt* win = static_cast<const WindowStmt*>(stmt->stmt.get());
                if (win->x1) collect_expr_variables(win->x1.get());
                if (win->y1) collect_expr_variables(win->y1.get());
                if (win->x2) collect_expr_variables(win->x2.get());
                if (win->y2) collect_expr_variables(win->y2.get());
                break;
            }

            case NodeType::PCOPY_STMT: {
                const PcopyStmt* pc = static_cast<const PcopyStmt*>(stmt->stmt.get());
                collect_expr_variables(pc->src_page.get());
                collect_expr_variables(pc->dest_page.get());
                break;
            }

            case NodeType::GET_GFX_STMT: {
                const GetGfxStmt* gg = static_cast<const GetGfxStmt*>(stmt->stmt.get());
                collect_expr_variables(gg->x1.get());
                collect_expr_variables(gg->y1.get());
                collect_expr_variables(gg->x2.get());
                collect_expr_variables(gg->y2.get());
                break;
            }

            case NodeType::PUT_GFX_STMT: {
                const PutGfxStmt* pg = static_cast<const PutGfxStmt*>(stmt->stmt.get());
                collect_expr_variables(pg->x.get());
                collect_expr_variables(pg->y.get());
                break;
            }

            case NodeType::SHELL_STMT: {
                const ShellStmt* sh = static_cast<const ShellStmt*>(stmt->stmt.get());
                if (sh->command) collect_expr_variables(sh->command.get());
                break;
            }

            case NodeType::POKE_STMT: {
                const PokeStmt* pk = static_cast<const PokeStmt*>(stmt->stmt.get());
                collect_expr_variables(pk->address.get());
                collect_expr_variables(pk->value.get());
                break;
            }

            case NodeType::DEF_SEG_STMT: {
                const DefSegStmt* ds = static_cast<const DefSegStmt*>(stmt->stmt.get());
                if (ds->segment) collect_expr_variables(ds->segment.get());
                break;
            }

            default:
                break;
        }
    }

    // Also collect variables from SUB and FUNCTION bodies
    // (so they get declared as globals, since all vars are effectively global)
    for (const auto& sub : program->subs) {
        collect_stmt_body_variables(sub->body);
    }
    for (const auto& func : program->functions) {
        collect_stmt_body_variables(func->body);
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
                    case NodeType::SELECT_CASE_STMT: {
                        const SelectCaseStmt* sel = static_cast<const SelectCaseStmt*>(stmt->stmt.get());
                        for (const auto& clause : sel->cases) {
                            collect_from_statements(clause.body);
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
    // Emit compile mode indicator for RBIDE and external tools
    if (needs_sdl) {
        writeln("// RB_COMPILE_MODE: graphics");
    } else if (needs_sdl_mixer) {
        writeln("// RB_COMPILE_MODE: console_audio");
    } else {
        writeln("// RB_COMPILE_MODE: console");
    }
    writeln("#include <stdio.h>");
    writeln("#include <stdlib.h>");
    writeln("#include <string.h>");
    
    if (needs_math_h) {
        writeln("#include <math.h>");
    }
    
    // Always include time.h now that we have DATE$, TIME$, TIMER, RANDOMIZE
    writeln("#include <time.h>");
    
    // SDL2 for graphics or audio
    if (needs_sdl || needs_sdl_mixer) {
        writeln("#include <SDL2/SDL.h>");
    }
    if (needs_sdl_image) {
        writeln("#include <SDL2/SDL_image.h>");
    }
    if (needs_sdl_mixer) {
        writeln("#include <SDL2/SDL_mixer.h>");
    }
    // For audio-only console programs: undo SDL's #define main SDL_main
    // so our main() stays as main() (called by shim_winmain, not SDL2main)
    if (needs_sdl_mixer && !needs_sdl) {
        writeln("#undef main");
    }
    
    // Windows headers for sound, sleep, and console allocation
    if (needs_sound || (needs_sdl && needs_console)) {
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
            // Sanitize variable name to avoid C/Windows conflicts
            std::string c_var = sanitize_varname(var);
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
            // Sanitize to avoid C/Windows conflicts
            c_var = sanitize_varname(c_var);
            writeln("char " + c_var + "[RB_STR_SIZE] = \"\";");
        }
        writeln("");
    }
    
    // User-defined type struct declarations
    if (!user_types.empty()) {
        writeln("// User-defined types");
        for (const auto& [name, info] : user_types) {
            writeln("typedef struct {");
            indent_level++;
            for (const auto& field : info.fields) {
                if (field.type_name == "STRING") {
                    int len = field.string_length > 0 ? field.string_length : 4096;
                    writeln("char " + field.name + "[" + std::to_string(len) + "];");
                } else if (field.type_name == "INTEGER") {
                    writeln("int " + field.name + ";");
                } else if (field.type_name == "LONG") {
                    writeln("long " + field.name + ";");
                } else if (field.type_name == "SINGLE") {
                    writeln("float " + field.name + ";");
                } else {
                    // DOUBLE or default
                    writeln("double " + field.name + ";");
                }
            }
            indent_level--;
            writeln("} " + name + ";");
            writeln("");
        }
    }

    // Typed variable instances
    if (!typed_variables.empty()) {
        writeln("// Typed variable instances");
        for (const auto& [var_name, type_name] : typed_variables) {
            writeln(type_name + " " + var_name + " = {0};");
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
        writeln("int* pixel_buffer = NULL;  // Software pixel buffer for POINT/PAINT");
        if (has_display_stmt) {
            writeln("int _auto_display = 0;  // Manual display mode (_DISPLAY used)");
        } else {
            writeln("int _auto_display = 1;  // Auto-present after each draw");
        }
        writeln("");
        writeln("// QB64 image management");
        writeln("#define MAX_IMAGES 256");
        writeln("SDL_Texture* _images[MAX_IMAGES] = {0};");
        writeln("int _image_w[MAX_IMAGES] = {0};");
        writeln("int _image_h[MAX_IMAGES] = {0};");
        writeln("int _next_image = 1;");
        writeln("");
        writeln("// QB64 mouse/keyboard state");
        writeln("int _mouse_x = 0, _mouse_y = 0;");
        writeln("int _mouse_buttons = 0;");
        writeln("int _mouse_wheel = 0;");
        writeln("const Uint8* _keyboard_state = NULL;");
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

    // SDL_mixer sound variables
    if (needs_sdl_mixer) {
        writeln("// SDL_mixer sound management");
        writeln("#define MAX_SOUNDS 64");
        writeln("Mix_Chunk* _sounds[MAX_SOUNDS] = {0};");
        writeln("int _sound_channels[MAX_SOUNDS] = {0};  // Channel assigned to each sound");
        writeln("int _next_sound = 1;");
        writeln("int _mixer_initialized = 0;");
        writeln("");
    }
}

void CodeGenerator::generate_array_declarations() {
    if (!arrays.empty()) {
        writeln("// Numeric arrays");
        for (const auto& arr_pair : arrays) {
            std::string arr_name = sanitize_varname(arr_pair.first);
            const auto& info = arr_pair.second;
            
            if (info.is_dynamic) {
                // Dynamic array - declare as pointer
                std::string decl = "double* " + arr_name + " = NULL;";
                writeln(decl);
            } else {
                // Static array - use fixed dimensions
                std::string decl = "double " + arr_name;
                for (const ASTNode* dim_expr : info.dimension_exprs) {
                    // Must be a number node
                    const NumberNode* num = static_cast<const NumberNode*>(dim_expr);
                    int dim_size = static_cast<int>(num->value);
                    decl += "[" + std::to_string(dim_size + 1) + "]";
                }
                decl += " = {0};";
                writeln(decl);
            }
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
            
            // Sanitize array name
            arr_name = sanitize_varname(arr_name);
            
            const auto& info = arr_pair.second;
            
            if (info.is_dynamic) {
                // Dynamic string array - declare as pointer
                std::string decl = "char (*" + arr_name + ")[RB_STR_SIZE] = NULL;";
                writeln(decl);
            } else {
                // Static string array
                std::string decl = "char " + arr_name;
                for (const ASTNode* dim_expr : info.dimension_exprs) {
                    const NumberNode* num = static_cast<const NumberNode*>(dim_expr);
                    int dim_size = static_cast<int>(num->value);
                    decl += "[" + std::to_string(dim_size + 1) + "]";
                }
                decl += "[RB_STR_SIZE] = {0};";
                writeln(decl);
            }
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
    writeln("int _gosub_stack[1000];");
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
    
    // String functions (always emitted - needed by _PRINTSTRING etc.)
    {
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
    
    // LTRIM$, RTRIM$, TRIM$
    {
        writeln("void RB_LTRIM(char* dest, const char* src) {");
        writeln("    while (*src == ' ') src++;");
        writeln("    strncpy(dest, src, 255); dest[255] = '\\0';");
        writeln("}");
        writeln("");

        writeln("void RB_RTRIM(char* dest, const char* src) {");
        writeln("    strncpy(dest, src, 255); dest[255] = '\\0';");
        writeln("    int len = strlen(dest);");
        writeln("    while (len > 0 && dest[len-1] == ' ') dest[--len] = '\\0';");
        writeln("}");
        writeln("");

        writeln("void RB_TRIM(char* dest, const char* src) {");
        writeln("    while (*src == ' ') src++;");
        writeln("    strncpy(dest, src, 255); dest[255] = '\\0';");
        writeln("    int len = strlen(dest);");
        writeln("    while (len > 0 && dest[len-1] == ' ') dest[--len] = '\\0';");
        writeln("}");
        writeln("");

        writeln("void RB_SPACE(char* dest, int count) {");
        writeln("    if (count < 0) count = 0;");
        writeln("    if (count > 255) count = 255;");
        writeln("    memset(dest, ' ', count);");
        writeln("    dest[count] = '\\0';");
        writeln("}");
        writeln("");

        writeln("void RB_STRING_FUNC(char* dest, int count, int ch) {");
        writeln("    if (count < 0) count = 0;");
        writeln("    if (count > 255) count = 255;");
        writeln("    memset(dest, ch, count);");
        writeln("    dest[count] = '\\0';");
        writeln("}");
        writeln("");
    }

    // ENVIRON$ helper
    writeln("void RB_ENVIRON(char* dest, const char* name) {");
    writeln("    char* val = getenv(name);");
    writeln("    if (val) { strncpy(dest, val, 255); dest[255] = '\\0'; }");
    writeln("    else dest[0] = '\\0';");
    writeln("}");
    writeln("");

    // COMMAND$ buffer
    writeln("char _command_str[RB_STR_SIZE] = \"\";");
    writeln("");

    // Emulated memory for PEEK/POKE/DEF SEG
    writeln("unsigned char _mem_segment[65536] = {0};");
    writeln("int _def_seg = 0;");
    writeln("");

    // File handles
    writeln("FILE* _file_handles[256] = {NULL};");
    writeln("int _file_reclen[256] = {0};  // Record lengths for RANDOM mode");
    writeln("");

    // Error handling globals
    if (has_error_handling) {
        writeln("// Error handling");
        writeln("int _error_active = 0;");
        writeln("int _error_handler_line = 0;");
        writeln("int _err = 0;");
        writeln("int _erl = 0;");
        writeln("int _resume_next_id = 0;");
        writeln("");
    } else {
        // Always declare _err and _erl so ERR/ERL expressions work
        writeln("int _err = 0;");
        writeln("int _erl = 0;");
        writeln("");
    }
    
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
    if (needs_sdl) {
        writeln("    current_color = fg;  // Set graphics color for _PRINTSTRING");
    }
    writeln("    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);");
    writeln("    SetConsoleTextAttribute(hConsole, (WORD)((bg << 4) | fg));");
    writeln("    fflush(stdout);  // Ensure color is applied");
    writeln("}");
    writeln("#else");
    writeln("void RB_CLS() { printf(\"\\033[2J\\033[H\"); }");
    writeln("void RB_LOCATE(int row, int col) { printf(\"\\033[%d;%dH\", row, col); }");
    if (needs_sdl) {
        writeln("void RB_COLOR(int fg, int bg) { current_color = fg; printf(\"\\033[%d;%dm\", 30 + fg, 40 + bg); }");
    } else {
        writeln("void RB_COLOR(int fg, int bg) { printf(\"\\033[%d;%dm\", 30 + fg, 40 + bg); }");
    }
    writeln("#endif");
    writeln("");
    
    // PRINT USING helper
    writeln("// PRINT USING - format a number according to format string");
    writeln("void RB_PRINT_USING_NUM(FILE* out, const char* fmt, double val) {");
    writeln("    int i = 0;");
    writeln("    int digits_before = 0, digits_after = 0;");
    writeln("    int has_dot = 0, has_comma = 0, has_plus = 0, has_minus = 0;");
    writeln("    int has_dollar = 0;");
    writeln("    // Count format characters");
    writeln("    for (i = 0; fmt[i]; i++) {");
    writeln("        if (fmt[i] == '#') { if (has_dot) digits_after++; else digits_before++; }");
    writeln("        else if (fmt[i] == '.') has_dot = 1;");
    writeln("        else if (fmt[i] == ',') has_comma = 1;");
    writeln("        else if (fmt[i] == '+') has_plus = 1;");
    writeln("        else if (fmt[i] == '-') has_minus = 1;");
    writeln("        else if (fmt[i] == '$' && fmt[i+1] == '$') { has_dollar = 1; i++; }");
    writeln("    }");
    writeln("    if (digits_before == 0) digits_before = 1;");
    writeln("    char fmtbuf[64];");
    writeln("    int w = digits_before + (has_dot ? 1 + digits_after : 0);");
    writeln("    if (has_dot)");
    writeln("        sprintf(fmtbuf, \"%*.*f\", w, digits_after, val);");
    writeln("    else");
    writeln("        sprintf(fmtbuf, \"%*.0f\", w, val);");
    writeln("    if (has_dollar) fprintf(out, \"$\");");
    writeln("    if (has_plus && val >= 0) fprintf(out, \"+\");");
    writeln("    fprintf(out, \"%s\", fmtbuf);");
    writeln("}");
    writeln("");
    writeln("void RB_PRINT_USING_STR(FILE* out, const char* fmt, const char* val) {");
    writeln("    if (fmt[0] == '!' && fmt[1] == '\\0') {");
    writeln("        fprintf(out, \"%c\", val[0]);  // First character only");
    writeln("    } else if (fmt[0] == '&') {");
    writeln("        fprintf(out, \"%s\", val);  // Entire string");
    writeln("    } else if (fmt[0] == '\\\\') {");
    writeln("        // Fixed width: count chars between backslashes");
    writeln("        int width = 2;");
    writeln("        for (int i = 1; fmt[i] && fmt[i] != '\\\\'; i++) width++;");
    writeln("        fprintf(out, \"%-*.*s\", width, width, val);");
    writeln("    } else {");
    writeln("        fprintf(out, \"%s\", val);");
    writeln("    }");
    writeln("}");
    writeln("");

    // MML PLAY helper (Windows Beep-based)
    writeln("#ifdef _WIN32");
    writeln("void RB_PLAY(const char* mml) {");
    writeln("    int octave = 4, tempo = 120, default_len = 4;");
    writeln("    int i = 0;");
    writeln("    double base_freq[12] = {16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87};");
    writeln("    while (mml[i]) {");
    writeln("        while (mml[i] == ' ') i++;");
    writeln("        if (!mml[i]) break;");
    writeln("        char c = mml[i]; i++;");
    writeln("        if (c == 'O' || c == 'o') { if (mml[i] >= '0' && mml[i] <= '8') { octave = mml[i] - '0'; i++; } continue; }");
    writeln("        if (c == 'L' || c == 'l') { default_len = 0; while (mml[i] >= '0' && mml[i] <= '9') { default_len = default_len * 10 + (mml[i] - '0'); i++; } if (default_len == 0) default_len = 4; continue; }");
    writeln("        if (c == 'T' || c == 't') { tempo = 0; while (mml[i] >= '0' && mml[i] <= '9') { tempo = tempo * 10 + (mml[i] - '0'); i++; } if (tempo == 0) tempo = 120; continue; }");
    writeln("        if (c == '<') { if (octave > 0) octave--; continue; }");
    writeln("        if (c == '>') { if (octave < 8) octave++; continue; }");
    writeln("        if (c == 'P' || c == 'p' || c == 'R' || c == 'r') {");
    writeln("            int len = default_len;");
    writeln("            if (mml[i] >= '0' && mml[i] <= '9') { len = 0; while (mml[i] >= '0' && mml[i] <= '9') { len = len * 10 + (mml[i] - '0'); i++; } }");
    writeln("            int ms = (60000 / tempo) * 4 / (len > 0 ? len : 4);");
    writeln("            Sleep(ms);");
    writeln("            continue;");
    writeln("        }");
    writeln("        int note = -1;");
    writeln("        switch (c) {");
    writeln("            case 'C': case 'c': note = 0; break;");
    writeln("            case 'D': case 'd': note = 2; break;");
    writeln("            case 'E': case 'e': note = 4; break;");
    writeln("            case 'F': case 'f': note = 5; break;");
    writeln("            case 'G': case 'g': note = 7; break;");
    writeln("            case 'A': case 'a': note = 9; break;");
    writeln("            case 'B': case 'b': note = 11; break;");
    writeln("        }");
    writeln("        if (note < 0) continue;");
    writeln("        if (mml[i] == '#' || mml[i] == '+') { note++; i++; }");
    writeln("        else if (mml[i] == '-') { note--; i++; }");
    writeln("        int len = default_len;");
    writeln("        if (mml[i] >= '0' && mml[i] <= '9') { len = 0; while (mml[i] >= '0' && mml[i] <= '9') { len = len * 10 + (mml[i] - '0'); i++; } }");
    writeln("        int dot = 0;");
    writeln("        if (mml[i] == '.') { dot = 1; i++; }");
    writeln("        if (note < 0) note = 0;");
    writeln("        if (note > 11) note = 11;");
    writeln("        int freq = (int)(base_freq[note % 12] * (1 << octave));");
    writeln("        int ms = (60000 / tempo) * 4 / (len > 0 ? len : 4);");
    writeln("        if (dot) ms = ms * 3 / 2;");
    writeln("        if (freq > 0) Beep(freq, ms);");
    writeln("    }");
    writeln("}");
    writeln("#else");
    writeln("void RB_PLAY(const char* mml) { (void)mml; }  // Stub for non-Windows");
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
        writeln("case 14: screen_width = 800; screen_height = 600; break;  // SVGA");
        writeln("default: screen_width = 640; screen_height = 480; break;");
        indent_level--;
        writeln("}");
        writeln("");
        if (needs_sdl_mixer) {
            writeln("if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {");
        } else {
            writeln("if (SDL_Init(SDL_INIT_VIDEO) < 0) {");
        }
        indent_level++;
        writeln("printf(\"SDL init failed: %s\\n\", SDL_GetError());");
        writeln("exit(1);");
        indent_level--;
        writeln("}");
        if (needs_sdl_image) {
            writeln("IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);");
        }
        if (needs_console) {
            writeln("");
            writeln("// Allocate console for text I/O in graphics mode");
            writeln("#ifdef _WIN32");
            writeln("AllocConsole();");
            writeln("freopen(\"CONIN$\", \"r\", stdin);");
            writeln("freopen(\"CONOUT$\", \"w\", stdout);");
            writeln("freopen(\"CONOUT$\", \"w\", stderr);");
            writeln("#endif");
        }
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
        writeln("");
        writeln("// Allocate pixel buffer for POINT/PAINT");
        writeln("pixel_buffer = (int*)calloc(screen_width * screen_height, sizeof(int));");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Cleanup graphics
        writeln("void cleanup_graphics() {");
        indent_level++;
        writeln("// Free image handles");
        writeln("for (int i = 1; i < MAX_IMAGES; i++) {");
        indent_level++;
        writeln("if (_images[i]) { SDL_DestroyTexture(_images[i]); _images[i] = NULL; }");
        indent_level--;
        writeln("}");
        if (needs_sdl_mixer) {
            writeln("// Free sound handles");
            writeln("for (int i = 1; i < MAX_SOUNDS; i++) {");
            indent_level++;
            writeln("if (_sounds[i]) { Mix_FreeChunk(_sounds[i]); _sounds[i] = NULL; }");
            indent_level--;
            writeln("}");
            writeln("if (_mixer_initialized) Mix_CloseAudio();");
        }
        writeln("if (pixel_buffer) { free(pixel_buffer); pixel_buffer = NULL; }");
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
        
        // Forward declarations for graphics helpers
        writeln("void set_pixel(int x, int y, int color_index);");
        writeln("int get_pixel_color(int x, int y);");
        writeln("void flood_fill(int x, int y, int fill_color, int border_color);");
        writeln("");

        // Draw line with pixel buffer update (Bresenham)
        writeln("void draw_line_buffered(int x0, int y0, int x1, int y1, int color) {");
        indent_level++;
        writeln("int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;");
        writeln("int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;");
        writeln("int err = dx + dy, e2;");
        writeln("for (;;) {");
        indent_level++;
        writeln("set_pixel(x0, y0, color);");
        writeln("if (x0 == x1 && y0 == y1) break;");
        writeln("e2 = 2 * err;");
        writeln("if (e2 >= dy) { err += dy; x0 += sx; }");
        writeln("if (e2 <= dx) { err += dx; y0 += sy; }");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("");

        // Draw filled rect with pixel buffer update
        writeln("void fill_rect_buffered(int x1, int y1, int w, int h, int color) {");
        indent_level++;
        writeln("for (int fy = y1; fy < y1 + h; fy++)");
        indent_level++;
        writeln("for (int fx = x1; fx < x1 + w; fx++)");
        indent_level++;
        writeln("set_pixel(fx, fy, color);");
        indent_level -= 2;
        indent_level--;
        writeln("}");
        writeln("");

        // Draw circle using Bresenham's algorithm
        writeln("void draw_circle(int cx, int cy, int radius, int color) {");
        indent_level++;
        writeln("int x = 0;");
        writeln("int y = radius;");
        writeln("int d = 3 - 2 * radius;");
        writeln("");
        writeln("while (x <= y) {");
        indent_level++;
        writeln("// Draw 8 octants");
        writeln("set_pixel(cx + x, cy + y, color);");
        writeln("set_pixel(cx - x, cy + y, color);");
        writeln("set_pixel(cx + x, cy - y, color);");
        writeln("set_pixel(cx - x, cy - y, color);");
        writeln("set_pixel(cx + y, cy + x, color);");
        writeln("set_pixel(cx - y, cy + x, color);");
        writeln("set_pixel(cx + y, cy - x, color);");
        writeln("set_pixel(cx - y, cy - x, color);");
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
        writeln("if (_auto_display) SDL_RenderPresent(renderer);");
        indent_level--;
        writeln("}");
        writeln("");
        
        // Set pixel in both renderer and pixel buffer
        writeln("void set_pixel(int x, int y, int color_index) {");
        indent_level++;
        writeln("if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return;");
        writeln("if (pixel_buffer) pixel_buffer[y * screen_width + x] = color_index;");
        writeln("SDL_Color c = get_palette_color(color_index);");
        writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
        writeln("SDL_RenderDrawPoint(renderer, x, y);");
        indent_level--;
        writeln("}");
        writeln("");

        // Get pixel color from buffer
        writeln("int get_pixel_color(int x, int y) {");
        indent_level++;
        writeln("if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return 0;");
        writeln("if (pixel_buffer) return pixel_buffer[y * screen_width + x];");
        writeln("return 0;");
        indent_level--;
        writeln("}");
        writeln("");

        // Scanline flood fill
        writeln("void flood_fill(int x, int y, int fill_color, int border_color) {");
        indent_level++;
        writeln("if (!pixel_buffer) return;");
        writeln("if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return;");
        writeln("int target_color = get_pixel_color(x, y);");
        writeln("if (target_color == fill_color) return;");
        writeln("if (target_color == border_color) return;");
        writeln("");
        writeln("// Stack-based flood fill");
        writeln("int stack_size = screen_width * screen_height;");
        writeln("int* stack_x = (int*)malloc(stack_size * sizeof(int));");
        writeln("int* stack_y = (int*)malloc(stack_size * sizeof(int));");
        writeln("if (!stack_x || !stack_y) { free(stack_x); free(stack_y); return; }");
        writeln("int sp = 0;");
        writeln("stack_x[sp] = x; stack_y[sp] = y; sp++;");
        writeln("");
        writeln("SDL_Color c = get_palette_color(fill_color);");
        writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
        writeln("");
        writeln("while (sp > 0) {");
        indent_level++;
        writeln("sp--;");
        writeln("int cx = stack_x[sp], cy = stack_y[sp];");
        writeln("if (cx < 0 || cx >= screen_width || cy < 0 || cy >= screen_height) continue;");
        writeln("int pc = pixel_buffer[cy * screen_width + cx];");
        writeln("if (pc == fill_color || pc == border_color) continue;");
        writeln("");
        writeln("// Scan left");
        writeln("int lx = cx;");
        writeln("while (lx > 0) {");
        indent_level++;
        writeln("int plc = pixel_buffer[cy * screen_width + (lx - 1)];");
        writeln("if (plc == fill_color || plc == border_color) break;");
        writeln("lx--;");
        indent_level--;
        writeln("}");
        writeln("// Scan right");
        writeln("int rx = cx;");
        writeln("while (rx < screen_width - 1) {");
        indent_level++;
        writeln("int prc = pixel_buffer[cy * screen_width + (rx + 1)];");
        writeln("if (prc == fill_color || prc == border_color) break;");
        writeln("rx++;");
        indent_level--;
        writeln("}");
        writeln("");
        writeln("// Fill the scanline");
        writeln("for (int i = lx; i <= rx; i++) {");
        indent_level++;
        writeln("pixel_buffer[cy * screen_width + i] = fill_color;");
        writeln("SDL_RenderDrawPoint(renderer, i, cy);");
        indent_level--;
        writeln("}");
        writeln("");
        writeln("// Push adjacent rows");
        writeln("for (int i = lx; i <= rx; i++) {");
        indent_level++;
        writeln("if (cy > 0 && sp < stack_size) {");
        indent_level++;
        writeln("int pa = pixel_buffer[(cy-1) * screen_width + i];");
        writeln("if (pa != fill_color && pa != border_color) {");
        writeln("    stack_x[sp] = i; stack_y[sp] = cy - 1; sp++;");
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("if (cy < screen_height - 1 && sp < stack_size) {");
        indent_level++;
        writeln("int pb = pixel_buffer[(cy+1) * screen_width + i];");
        writeln("if (pb != fill_color && pb != border_color) {");
        writeln("    stack_x[sp] = i; stack_y[sp] = cy + 1; sp++;");
        writeln("}");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("free(stack_x); free(stack_y);");
        writeln("if (_auto_display) SDL_RenderPresent(renderer);");
        indent_level--;
        writeln("}");
        writeln("");

        // DRAW GML interpreter
        writeln("void RB_DRAW(const char* cmd) {");
        indent_level++;
        writeln("static int draw_x = -1, draw_y = -1;");
        writeln("if (draw_x < 0) { draw_x = screen_width / 2; draw_y = screen_height / 2; }");
        writeln("int i = 0;");
        writeln("int draw_color = current_color;");
        writeln("while (cmd[i]) {");
        indent_level++;
        writeln("while (cmd[i] == ' ') i++;");
        writeln("if (!cmd[i]) break;");
        writeln("int blind = 0, ret = 0;");
        writeln("// Check prefixes B and N");
        writeln("while (cmd[i] == 'B' || cmd[i] == 'b' || cmd[i] == 'N' || cmd[i] == 'n') {");
        indent_level++;
        writeln("if (cmd[i] == 'B' || cmd[i] == 'b') blind = 1;");
        writeln("if (cmd[i] == 'N' || cmd[i] == 'n') ret = 1;");
        writeln("i++;");
        indent_level--;
        writeln("}");
        writeln("char c = cmd[i]; i++;");
        writeln("if (!c) break;");
        writeln("// Parse number");
        writeln("int neg = 0;");
        writeln("if (cmd[i] == '-') { neg = 1; i++; }");
        writeln("else if (cmd[i] == '+') { i++; }");
        writeln("int n = 0;");
        writeln("while (cmd[i] >= '0' && cmd[i] <= '9') { n = n * 10 + (cmd[i] - '0'); i++; }");
        writeln("if (neg) n = -n;");
        writeln("int sx = draw_x, sy = draw_y;");
        writeln("int dx = 0, dy = 0;");
        writeln("switch (c) {");
        indent_level++;
        writeln("case 'U': case 'u': dy = -n; break;");
        writeln("case 'D': case 'd': dy = n; break;");
        writeln("case 'L': case 'l': dx = -n; break;");
        writeln("case 'R': case 'r': dx = n; break;");
        writeln("case 'E': case 'e': dx = n; dy = -n; break;");
        writeln("case 'F': case 'f': dx = n; dy = n; break;");
        writeln("case 'G': case 'g': dx = -n; dy = n; break;");
        writeln("case 'H': case 'h': dx = -n; dy = -n; break;");
        writeln("case 'M': case 'm': {");
        indent_level++;
        writeln("// M x,y - move to position (absolute or relative)");
        writeln("int rel = 0;");
        writeln("while (cmd[i] == ' ') i++;");
        writeln("if (cmd[i] == '+' || cmd[i] == '-') rel = 1;");
        writeln("int mneg = 0;");
        writeln("if (cmd[i] == '-') { mneg = 1; i++; }");
        writeln("else if (cmd[i] == '+') { i++; }");
        writeln("int mx = 0;");
        writeln("while (cmd[i] >= '0' && cmd[i] <= '9') { mx = mx * 10 + (cmd[i] - '0'); i++; }");
        writeln("if (mneg) mx = -mx;");
        writeln("while (cmd[i] == ' ' || cmd[i] == ',') i++;");
        writeln("int myneg = 0;");
        writeln("if (cmd[i] == '-') { myneg = 1; i++; }");
        writeln("else if (cmd[i] == '+') { i++; }");
        writeln("int my = 0;");
        writeln("while (cmd[i] >= '0' && cmd[i] <= '9') { my = my * 10 + (cmd[i] - '0'); i++; }");
        writeln("if (myneg) my = -my;");
        writeln("if (rel) { dx = mx; dy = my; }");
        writeln("else {");
        indent_level++;
        writeln("if (!blind) draw_line_buffered(draw_x, draw_y, mx, my, draw_color);");
        writeln("draw_x = mx; draw_y = my;");
        writeln("if (_auto_display) SDL_RenderPresent(renderer);");
        writeln("continue;");
        indent_level--;
        writeln("}");
        writeln("break;");
        indent_level--;
        writeln("}");
        writeln("case 'C': case 'c': {");
        indent_level++;
        writeln("// C n - change color");
        writeln("draw_color = n;");
        writeln("current_color = n;");
        writeln("continue;");
        indent_level--;
        writeln("}");
        writeln("default: continue;");
        indent_level--;
        writeln("}");
        writeln("int nx = draw_x + dx, ny = draw_y + dy;");
        writeln("if (!blind) draw_line_buffered(draw_x, draw_y, nx, ny, draw_color);");
        writeln("if (ret) { /* don't move */ } else { draw_x = nx; draw_y = ny; }");
        indent_level--;
        writeln("}");
        writeln("if (_auto_display) SDL_RenderPresent(renderer);");
        indent_level--;
        writeln("}");
        writeln("");
    }

    // QB64 process_events helper (updates mouse/keyboard state)
    if (needs_sdl) {
        writeln("void rb_process_events() {");
        indent_level++;
        writeln("SDL_Event _evt;");
        writeln("_mouse_wheel = 0;");
        writeln("while (SDL_PollEvent(&_evt)) {");
        indent_level++;
        writeln("if (_evt.type == SDL_QUIT) { cleanup_graphics(); exit(0); }");
        writeln("if (_evt.type == SDL_KEYDOWN && _evt.key.keysym.sym == SDLK_ESCAPE) { cleanup_graphics(); exit(0); }");
        writeln("if (_evt.type == SDL_MOUSEWHEEL) { _mouse_wheel = _evt.wheel.y; }");
        indent_level--;
        writeln("}");
        writeln("_mouse_buttons = SDL_GetMouseState(&_mouse_x, &_mouse_y);");
        writeln("_keyboard_state = SDL_GetKeyboardState(NULL);");
        indent_level--;
        writeln("}");
        writeln("");

        // _NEWIMAGE helper
        writeln("int rb_newimage(int w, int h) {");
        indent_level++;
        writeln("if (_next_image >= MAX_IMAGES) return -1;");
        writeln("int handle = _next_image++;");
        writeln("_images[handle] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);");
        writeln("if (!_images[handle]) return -1;");
        writeln("_image_w[handle] = w;");
        writeln("_image_h[handle] = h;");
        writeln("SDL_SetTextureBlendMode(_images[handle], SDL_BLENDMODE_BLEND);");
        writeln("return handle;");
        indent_level--;
        writeln("}");
        writeln("");

        // _LOADIMAGE helper
        if (needs_sdl_image) {
            writeln("int rb_loadimage(const char* filename) {");
            indent_level++;
            writeln("if (_next_image >= MAX_IMAGES) return -1;");
            writeln("SDL_Surface* surf = IMG_Load(filename);");
            writeln("if (!surf) return -1;");
            writeln("int handle = _next_image++;");
            writeln("_images[handle] = SDL_CreateTextureFromSurface(renderer, surf);");
            writeln("_image_w[handle] = surf->w;");
            writeln("_image_h[handle] = surf->h;");
            writeln("SDL_FreeSurface(surf);");
            writeln("if (!_images[handle]) { _next_image--; return -1; }");
            writeln("return handle;");
            indent_level--;
            writeln("}");
        } else {
            writeln("int rb_loadimage(const char* filename) {");
            indent_level++;
            writeln("if (_next_image >= MAX_IMAGES) return -1;");
            writeln("SDL_Surface* surf = SDL_LoadBMP(filename);");
            writeln("if (!surf) return -1;");
            writeln("int handle = _next_image++;");
            writeln("_images[handle] = SDL_CreateTextureFromSurface(renderer, surf);");
            writeln("_image_w[handle] = surf->w;");
            writeln("_image_h[handle] = surf->h;");
            writeln("SDL_FreeSurface(surf);");
            writeln("if (!_images[handle]) { _next_image--; return -1; }");
            writeln("return handle;");
            indent_level--;
            writeln("}");
        }
        writeln("");

        // _PRINTSTRING helper — 8x8 bitmap font (CP437/IBM PC font)
        writeln("// CP437 8x8 bitmap font data (chars 32-127)");
        writeln("static const unsigned char _font8x8[96][8] = {");
        indent_level++;
        // Space through ~  (ASCII 32-127, 96 characters)
        // Each character is 8 bytes, each byte is a row (MSB = leftmost pixel)
        writeln("{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 ' '");
        writeln("{0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // 33 '!'");
        writeln("{0x6C,0x6C,0x24,0x00,0x00,0x00,0x00,0x00}, // 34 '\"'");
        writeln("{0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // 35 '#'");
        writeln("{0x18,0x7E,0xC0,0x7C,0x06,0xFC,0x18,0x00}, // 36 '$'");
        writeln("{0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00}, // 37 '%'");
        writeln("{0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, // 38 '&'");
        writeln("{0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}, // 39 '''");
        writeln("{0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // 40 '('");
        writeln("{0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // 41 ')'");
        writeln("{0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // 42 '*'");
        writeln("{0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00}, // 43 '+'");
        writeln("{0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // 44 ','");
        writeln("{0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // 45 '-'");
        writeln("{0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // 46 '.'");
        writeln("{0x02,0x06,0x0C,0x18,0x30,0x60,0xC0,0x00}, // 47 '/'");
        writeln("{0x7C,0xC6,0xCE,0xDE,0xF6,0xE6,0x7C,0x00}, // 48 '0'");
        writeln("{0x18,0x38,0x78,0x18,0x18,0x18,0x7E,0x00}, // 49 '1'");
        writeln("{0x7C,0xC6,0x06,0x1C,0x30,0x66,0xFE,0x00}, // 50 '2'");
        writeln("{0x7C,0xC6,0x06,0x3C,0x06,0xC6,0x7C,0x00}, // 51 '3'");
        writeln("{0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x1E,0x00}, // 52 '4'");
        writeln("{0xFE,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00}, // 53 '5'");
        writeln("{0x38,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00}, // 54 '6'");
        writeln("{0xFE,0xC6,0x0C,0x18,0x30,0x30,0x30,0x00}, // 55 '7'");
        writeln("{0x7C,0xC6,0xC6,0x7C,0xC6,0xC6,0x7C,0x00}, // 56 '8'");
        writeln("{0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00}, // 57 '9'");
        writeln("{0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // 58 ':'");
        writeln("{0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // 59 ';'");
        writeln("{0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00}, // 60 '<'");
        writeln("{0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // 61 '='");
        writeln("{0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00}, // 62 '>'");
        writeln("{0x7C,0xC6,0x0C,0x18,0x18,0x00,0x18,0x00}, // 63 '?'");
        writeln("{0x7C,0xC6,0xDE,0xDE,0xDE,0xC0,0x78,0x00}, // 64 '@'");
        writeln("{0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x00}, // 65 'A'");
        writeln("{0xFC,0x66,0x66,0x7C,0x66,0x66,0xFC,0x00}, // 66 'B'");
        writeln("{0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00}, // 67 'C'");
        writeln("{0xF8,0x6C,0x66,0x66,0x66,0x6C,0xF8,0x00}, // 68 'D'");
        writeln("{0xFE,0x62,0x68,0x78,0x68,0x62,0xFE,0x00}, // 69 'E'");
        writeln("{0xFE,0x62,0x68,0x78,0x68,0x60,0xF0,0x00}, // 70 'F'");
        writeln("{0x3C,0x66,0xC0,0xC0,0xCE,0x66,0x3E,0x00}, // 71 'G'");
        writeln("{0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00}, // 72 'H'");
        writeln("{0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 73 'I'");
        writeln("{0x1E,0x0C,0x0C,0x0C,0xCC,0xCC,0x78,0x00}, // 74 'J'");
        writeln("{0xE6,0x66,0x6C,0x78,0x6C,0x66,0xE6,0x00}, // 75 'K'");
        writeln("{0xF0,0x60,0x60,0x60,0x62,0x66,0xFE,0x00}, // 76 'L'");
        writeln("{0xC6,0xEE,0xFE,0xD6,0xC6,0xC6,0xC6,0x00}, // 77 'M'");
        writeln("{0xC6,0xE6,0xF6,0xDE,0xCE,0xC6,0xC6,0x00}, // 78 'N'");
        writeln("{0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // 79 'O'");
        writeln("{0xFC,0x66,0x66,0x7C,0x60,0x60,0xF0,0x00}, // 80 'P'");
        writeln("{0x7C,0xC6,0xC6,0xC6,0xD6,0xDE,0x7C,0x06}, // 81 'Q'");
        writeln("{0xFC,0x66,0x66,0x7C,0x6C,0x66,0xE6,0x00}, // 82 'R'");
        writeln("{0x7C,0xC6,0xC0,0x7C,0x06,0xC6,0x7C,0x00}, // 83 'S'");
        writeln("{0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00}, // 84 'T'");
        writeln("{0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00}, // 85 'U'");
        writeln("{0xC6,0xC6,0xC6,0xC6,0x6C,0x38,0x10,0x00}, // 86 'V'");
        writeln("{0xC6,0xC6,0xC6,0xD6,0xFE,0xEE,0xC6,0x00}, // 87 'W'");
        writeln("{0xC6,0xC6,0x6C,0x38,0x6C,0xC6,0xC6,0x00}, // 88 'X'");
        writeln("{0x66,0x66,0x66,0x3C,0x18,0x18,0x3C,0x00}, // 89 'Y'");
        writeln("{0xFE,0xC6,0x8C,0x18,0x32,0x66,0xFE,0x00}, // 90 'Z'");
        writeln("{0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00}, // 91 '['");
        writeln("{0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, // 92 '\\\\'");
        writeln("{0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, // 93 ']'");
        writeln("{0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00}, // 94 '^'");
        writeln("{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // 95 '_'");
        writeln("{0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, // 96 '`'");
        writeln("{0x00,0x00,0x78,0x0C,0x7C,0xCC,0x76,0x00}, // 97 'a'");
        writeln("{0xE0,0x60,0x7C,0x66,0x66,0x66,0xDC,0x00}, // 98 'b'");
        writeln("{0x00,0x00,0x7C,0xC6,0xC0,0xC6,0x7C,0x00}, // 99 'c'");
        writeln("{0x1C,0x0C,0x7C,0xCC,0xCC,0xCC,0x76,0x00}, // 100 'd'");
        writeln("{0x00,0x00,0x7C,0xC6,0xFE,0xC0,0x7C,0x00}, // 101 'e'");
        writeln("{0x1C,0x36,0x30,0x78,0x30,0x30,0x78,0x00}, // 102 'f'");
        writeln("{0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x78}, // 103 'g'");
        writeln("{0xE0,0x60,0x6C,0x76,0x66,0x66,0xE6,0x00}, // 104 'h'");
        writeln("{0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, // 105 'i'");
        writeln("{0x06,0x00,0x0E,0x06,0x06,0x66,0x66,0x3C}, // 106 'j'");
        writeln("{0xE0,0x60,0x66,0x6C,0x78,0x6C,0xE6,0x00}, // 107 'k'");
        writeln("{0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 108 'l'");
        writeln("{0x00,0x00,0xEC,0xFE,0xD6,0xC6,0xC6,0x00}, // 109 'm'");
        writeln("{0x00,0x00,0xDC,0x66,0x66,0x66,0x66,0x00}, // 110 'n'");
        writeln("{0x00,0x00,0x7C,0xC6,0xC6,0xC6,0x7C,0x00}, // 111 'o'");
        writeln("{0x00,0x00,0xDC,0x66,0x66,0x7C,0x60,0xF0}, // 112 'p'");
        writeln("{0x00,0x00,0x76,0xCC,0xCC,0x7C,0x0C,0x1E}, // 113 'q'");
        writeln("{0x00,0x00,0xDC,0x76,0x60,0x60,0xF0,0x00}, // 114 'r'");
        writeln("{0x00,0x00,0x7C,0xC0,0x7C,0x06,0xFC,0x00}, // 115 's'");
        writeln("{0x10,0x30,0x7C,0x30,0x30,0x34,0x18,0x00}, // 116 't'");
        writeln("{0x00,0x00,0xCC,0xCC,0xCC,0xCC,0x76,0x00}, // 117 'u'");
        writeln("{0x00,0x00,0xC6,0xC6,0xC6,0x6C,0x38,0x00}, // 118 'v'");
        writeln("{0x00,0x00,0xC6,0xC6,0xD6,0xFE,0x6C,0x00}, // 119 'w'");
        writeln("{0x00,0x00,0xC6,0x6C,0x38,0x6C,0xC6,0x00}, // 120 'x'");
        writeln("{0x00,0x00,0xC6,0xC6,0xCE,0x76,0x06,0x7C}, // 121 'y'");
        writeln("{0x00,0x00,0xFE,0x0C,0x38,0x60,0xFE,0x00}, // 122 'z'");
        writeln("{0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00}, // 123 '{'");
        writeln("{0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, // 124 '|'");
        writeln("{0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00}, // 125 '}'");
        writeln("{0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00}, // 126 '~'");
        indent_level--;
        writeln("};");
        writeln("");
        writeln("void rb_printstring(int px, int py, const char* text) {");
        indent_level++;
        writeln("SDL_Color c = get_palette_color(current_color);");
        writeln("SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);");
        writeln("int cx = px;");
        writeln("for (int i = 0; text[i]; i++) {");
        indent_level++;
        writeln("unsigned char ch = (unsigned char)text[i];");
        writeln("if (ch < 32 || ch > 126) { cx += 8; continue; }");
        writeln("const unsigned char* glyph = _font8x8[ch - 32];");
        writeln("for (int row = 0; row < 8; row++) {");
        indent_level++;
        writeln("unsigned char bits = glyph[row];");
        writeln("for (int col = 0; col < 8; col++) {");
        indent_level++;
        writeln("if (bits & (0x80 >> col)) {");
        indent_level++;
        writeln("SDL_RenderDrawPoint(renderer, cx + col, py + row);");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("cx += 8;");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("");
    }

    // SDL_mixer helpers
    if (needs_sdl_mixer) {
        writeln("void rb_init_mixer() {");
        indent_level++;
        writeln("if (_mixer_initialized) return;");
        writeln("// Ensure SDL audio subsystem is initialized");
        writeln("if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {");
        indent_level++;
        writeln("if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {");
        indent_level++;
        writeln("printf(\"SDL audio init failed: %s\\n\", SDL_GetError());");
        writeln("return;");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("}");
        writeln("if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {");
        indent_level++;
        writeln("printf(\"SDL_mixer init failed: %s\\n\", Mix_GetError());");
        writeln("return;");
        indent_level--;
        writeln("}");
        writeln("_mixer_initialized = 1;");
        indent_level--;
        writeln("}");
        writeln("");

        writeln("int rb_sndopen(const char* filename) {");
        indent_level++;
        writeln("rb_init_mixer();");
        writeln("if (_next_sound >= MAX_SOUNDS) return -1;");
        writeln("int handle = _next_sound++;");
        writeln("_sounds[handle] = Mix_LoadWAV(filename);");
        writeln("if (!_sounds[handle]) { _next_sound--; return -1; }");
        writeln("return handle;");
        indent_level--;
        writeln("}");
        writeln("");
    }
}

void CodeGenerator::generate_user_subs_and_functions(const Program* program) {
    // Forward declarations for all SUBs and FUNCTIONs
    for (const auto& sub : program->subs) {
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
        write(");\n");
    }
    for (const auto& func : program->functions) {
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
        write(");\n");
    }
    if (!program->subs.empty() || !program->functions.empty()) {
        writeln("");
    }

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

        // Declare local temp string buffers (needed for string operations)
        writeln("char _temp_str[RB_STR_SIZE];");
        writeln("char _temp_str2[RB_STR_SIZE];");
        writeln("char _temp_str3[RB_STR_SIZE];");

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
        // Declare local temp string buffers (needed for string operations)
        writeln("char _temp_str[RB_STR_SIZE];");
        writeln("char _temp_str2[RB_STR_SIZE];");
        writeln("char _temp_str3[RB_STR_SIZE];");

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
            writeln("static char _result[RB_STR_SIZE];");
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
        needs_math_h = true;  // Ensure math.h is included
        write(fname == "TAN" ? "tan" : fname == "SIN" ? "sin" : fname == "COS" ? "cos" : 
              fname == "LOG" ? "log" : "exp");
        write("(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "ATN") {
        needs_math_h = true;  // Ensure math.h is included
        write("atan(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "SQR") {
        needs_math_h = true;  // Ensure math.h is included
        write("sqrt(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "ABS") {
        needs_math_h = true;  // Ensure math.h is included
        write("fabs(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")");
    }
    else if (fname == "INT") {
        needs_math_h = true;  // Ensure math.h is included
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
        write("feof(_file_handles[(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")])");
    }
    else if (fname == "LOF") {
        // LOF(n) - length of file: seek to end, get position, seek back
        write("({ FILE* _f = _file_handles[(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")]; long _cur = ftell(_f); fseek(_f, 0, SEEK_END); long _len = ftell(_f); fseek(_f, _cur, SEEK_SET); (double)_len; })");
    }
    else if (fname == "LOC") {
        // LOC(n) - current file position (1-based record for RANDOM, byte pos for others)
        write("((double)ftell(_file_handles[(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")]))");
    }
    else if (fname == "SEEK") {
        // SEEK(n) as a function - returns current file position
        write("((double)(ftell(_file_handles[(int)(");
        if (!func->arguments.empty()) {
            generate_expression(func->arguments[0].get());
        }
        write(")]) + 1))");
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
    
    // Phase 4: LTRIM$, RTRIM$, TRIM$
    else if (fname == "LTRIM$") {
        write("(RB_LTRIM(_temp_str, ");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get(), true);
        write("), _temp_str)");
    }
    else if (fname == "RTRIM$") {
        write("(RB_RTRIM(_temp_str, ");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get(), true);
        write("), _temp_str)");
    }
    else if (fname == "TRIM$") {
        write("(RB_TRIM(_temp_str, ");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get(), true);
        write("), _temp_str)");
    }

    // SPACE$(n)
    else if (fname == "SPACE$") {
        write("(RB_SPACE(_temp_str, (int)(");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get());
        write(")), _temp_str)");
    }

    // STRING$(n, char_or_code)
    else if (fname == "STRING$") {
        write("(RB_STRING_FUNC(_temp_str, (int)(");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get());
        write("), ");
        if (func->arguments.size() >= 2) {
            if (is_string_expr(func->arguments[1].get())) {
                generate_expression(func->arguments[1].get(), true);
                write("[0]");
            } else {
                write("(int)(");
                generate_expression(func->arguments[1].get());
                write(")");
            }
        } else {
            write("32");  // space as default
        }
        write("), _temp_str)");
    }

    // ENVIRON$(name)
    else if (fname == "ENVIRON$") {
        write("(RB_ENVIRON(_temp_str, ");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get(), true);
        write("), _temp_str)");
    }

    // COMMAND$
    else if (fname == "COMMAND$") {
        write("_command_str");
    }

    // PEEK(address)
    else if (fname == "PEEK") {
        write("(double)_mem_segment[(int)(");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get());
        write(") & 0xFFFF]");
    }

    // Phase 6: QB64 Extension Functions
    else if (fname == "_RGB") {
        write("((int)(");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get());
        write(") << 16 | (int)(");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get());
        write(") << 8 | (int)(");
        if (func->arguments.size() >= 3) generate_expression(func->arguments[2].get());
        write("))");
    }
    else if (fname == "_RGB32") {
        if (func->arguments.size() >= 4) {
            write("((int)(");
            generate_expression(func->arguments[3].get());
            write(") << 24 | (int)(");
            generate_expression(func->arguments[0].get());
            write(") << 16 | (int)(");
            generate_expression(func->arguments[1].get());
            write(") << 8 | (int)(");
            generate_expression(func->arguments[2].get());
            write("))");
        } else if (func->arguments.size() >= 3) {
            write("(255 << 24 | (int)(");
            generate_expression(func->arguments[0].get());
            write(") << 16 | (int)(");
            generate_expression(func->arguments[1].get());
            write(") << 8 | (int)(");
            generate_expression(func->arguments[2].get());
            write("))");
        } else {
            write("0");
        }
    }
    else if (fname == "_MOUSEX") {
        write("(double)_mouse_x");
    }
    else if (fname == "_MOUSEY") {
        write("(double)_mouse_y");
    }
    else if (fname == "_MOUSEBUTTON") {
        write("(double)((");
        if (!func->arguments.empty()) {
            write("(int)(");
            generate_expression(func->arguments[0].get());
            write(") == 1 ? (_mouse_buttons & SDL_BUTTON_LMASK) : ");
            write("(int)(");
            generate_expression(func->arguments[0].get());
            write(") == 2 ? (_mouse_buttons & SDL_BUTTON_RMASK) : ");
            write("(int)(");
            generate_expression(func->arguments[0].get());
            write(") == 3 ? (_mouse_buttons & SDL_BUTTON_MMASK) : 0");
        } else {
            write("0");
        }
        write(") ? -1 : 0)");
    }
    else if (fname == "_KEYDOWN") {
        write("(double)(_keyboard_state ? _keyboard_state[SDL_GetScancodeFromKey((int)(");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get());
        write("))] : 0)");
    }
    else if (fname == "_MOUSEWHEEL") {
        write("(double)_mouse_wheel");
    }
    else if (fname == "_NEWIMAGE") {
        write("(double)rb_newimage((int)(");
        if (func->arguments.size() >= 1) generate_expression(func->arguments[0].get());
        write("), (int)(");
        if (func->arguments.size() >= 2) generate_expression(func->arguments[1].get());
        write("))");
    }
    else if (fname == "_LOADIMAGE") {
        write("(double)rb_loadimage(");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get(), true);
        write(")");
    }
    else if (fname == "_SNDOPEN") {
        write("(double)rb_sndopen(");
        if (!func->arguments.empty()) generate_expression(func->arguments[0].get(), true);
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
            // Check struct member access
            size_t dot_pos = var->name.find('.');
            if (dot_pos != std::string::npos) {
                std::string struct_var = var->name.substr(0, dot_pos);
                std::string field_name = var->name.substr(dot_pos + 1);
                if (!field_name.empty() && field_name.back() == '$') return true;
                // Look up field type
                for (auto& c : struct_var) c = std::toupper(c);
                std::string type_name_lookup;
                if (typed_variables.count(sanitize_varname(struct_var))) {
                    type_name_lookup = typed_variables[sanitize_varname(struct_var)];
                }
                if (user_types.count(type_name_lookup)) {
                    for (const auto& f : user_types[type_name_lookup].fields) {
                        std::string uf = f.name;
                        for (auto& c : uf) c = std::toupper(c);
                        std::string cf = field_name;
                        for (auto& c : cf) c = std::toupper(c);
                        if (uf == cf && f.type_name == "STRING") return true;
                    }
                }
                return false;
            }
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
            write("\"" + escape_string(str->value) + "\"");
            break;
        }
        
        case NodeType::VARIABLE: {
            const VariableNode* var = static_cast<const VariableNode*>(expr);
            std::string var_name = var->name;

            // Handle struct member access
            {
                size_t dot_pos = var_name.find('.');
                if (dot_pos != std::string::npos) {
                    std::string struct_var = var_name.substr(0, dot_pos);
                    std::string field_name = var_name.substr(dot_pos + 1);
                    // Remove trailing $ if present
                    std::string clean_field = field_name;
                    if (!clean_field.empty() && clean_field.back() == '$') clean_field.pop_back();
                    // Upper-case the struct var to match sanitized name
                    for (auto& c : struct_var) c = std::toupper(c);
                    std::string member_expr = sanitize_varname(struct_var) + "." + clean_field;
                    // Check if this is a non-string, non-double field that needs casting
                    bool needs_cast = false;
                    std::string type_name_lu;
                    if (typed_variables.count(sanitize_varname(struct_var))) {
                        type_name_lu = typed_variables[sanitize_varname(struct_var)];
                    }
                    if (user_types.count(type_name_lu)) {
                        for (const auto& f : user_types[type_name_lu].fields) {
                            std::string uf = f.name;
                            for (auto& c : uf) c = std::toupper(c);
                            std::string cf = clean_field;
                            for (auto& c : cf) c = std::toupper(c);
                            if (uf == cf && f.type_name != "STRING" && f.type_name != "DOUBLE") {
                                needs_cast = true;
                            }
                        }
                    }
                    if (needs_cast) {
                        write("(double)" + member_expr);
                    } else {
                        write(member_expr);
                    }
                    break;
                }
            }

            // Handle ERR and ERL built-in variables
            if (var_name == "ERR") {
                write("((double)_err)");
                break;
            }
            if (var_name == "ERL") {
                write("((double)_erl)");
                break;
            }

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
            
            // Sanitize variable name to avoid C/Windows conflicts
            var_name = sanitize_varname(var_name);
            
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
            
            // Sanitize array name to avoid C/Windows conflicts
            arr_name = sanitize_varname(arr_name);
            
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
                needs_math_h = true;  // fmod() requires math.h
                write("fmod(");
                generate_expression(binop->left.get());
                write(", ");
                generate_expression(binop->right.get());
                write(")");
            }
            else if (binop->op == "\\") {
                // Integer division - cast to int, divide, result is int
                write("(int)(");
                generate_expression(binop->left.get());
                write(") / (int)(");
                generate_expression(binop->right.get());
                write(")");
            }
            else if (binop->op == "^") {
                needs_math_h = true;  // pow() requires math.h
                write("pow(");
                generate_expression(binop->left.get());
                write(", ");
                generate_expression(binop->right.get());
                write(")");
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
    // Handle PRINT USING
    if (stmt->using_format) {
        generate_print_using(stmt);
        return;
    }

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
            
            if (i < stmt->expressions.size() - 1) {
                bool next_is_semicolon = (i + 1 < stmt->use_semicolon.size() && stmt->use_semicolon[i + 1]);
                if (!next_is_semicolon) {
                    writeln("fprintf(_file_handles[" + std::to_string(stmt->file_number) + "], \" \");");
                }
            }
        }
        
        // Only print newline if PRINT doesn't end with ; or ,
        if (!stmt->has_trailing_separator) {
            writeln("fprintf(_file_handles[" + std::to_string(stmt->file_number) + "], \"\\n\");");
        }
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
            if (is_string_expr(expr)) {
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
        
        if (i < stmt->expressions.size() - 1) {
            // Check separator before next item to decide spacing
            bool next_is_semicolon = (i + 1 < stmt->use_semicolon.size() && stmt->use_semicolon[i + 1]);
            if (!next_is_semicolon) {
                writeln("printf(\" \");");
            }
        }
    }

    // Only print newline if PRINT doesn't end with ; or ,
    if (!stmt->has_trailing_separator) {
        writeln("printf(\"\\n\");");
    }
    writeln("fflush(stdout);  // Ensure output appears immediately");
}

void CodeGenerator::generate_let_stmt(const LetStmt* stmt) {
    std::string var_name = stmt->variable;
    bool is_string = (!var_name.empty() && var_name.back() == '$');

    // Handle struct member assignment
    {
        size_t dot_pos = stmt->variable.find('.');
        if (dot_pos != std::string::npos) {
            std::string struct_var = stmt->variable.substr(0, dot_pos);
            std::string field_name = stmt->variable.substr(dot_pos + 1);
            bool field_is_string = false;

            // Remove $ suffix from field name
            std::string clean_field = field_name;
            if (!clean_field.empty() && clean_field.back() == '$') {
                clean_field.pop_back();
                field_is_string = true;
            }

            // Look up if field is string type from type definition
            for (auto& c : struct_var) c = std::toupper(c);
            std::string type_name;
            if (typed_variables.count(sanitize_varname(struct_var))) {
                type_name = typed_variables[sanitize_varname(struct_var)];
            }
            if (user_types.count(type_name)) {
                for (const auto& f : user_types[type_name].fields) {
                    std::string uf = f.name;
                    for (auto& c : uf) c = std::toupper(c);
                    std::string cf = clean_field;
                    for (auto& c : cf) c = std::toupper(c);
                    if (uf == cf && f.type_name == "STRING") {
                        field_is_string = true;
                    }
                }
            }

            std::string c_target = sanitize_varname(struct_var) + "." + clean_field;

            if (field_is_string) {
                write(get_indent() + "strncpy(" + c_target + ", ");
                generate_expression(stmt->expression.get(), true);
                // Get field string length
                int flen = 256;
                if (user_types.count(type_name)) {
                    for (const auto& f : user_types[type_name].fields) {
                        std::string uf = f.name;
                        for (auto& c : uf) c = std::toupper(c);
                        std::string cf = clean_field;
                        for (auto& c : cf) c = std::toupper(c);
                        if (uf == cf) {
                            flen = f.string_length > 0 ? f.string_length : 256;
                            break;
                        }
                    }
                }
                write(", " + std::to_string(flen - 1) + ");\n");
                writeln(c_target + "[" + std::to_string(flen - 1) + "] = '\\0';");
            } else {
                write(get_indent() + c_target + " = ");
                generate_expression(stmt->expression.get());
                write(";\n");
            }
            return;
        }
    }

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
        
        // Sanitize array name
        arr_name = sanitize_varname(arr_name);
        
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
        // Sanitize variable name
        var_name = sanitize_varname(var_name);
        
        if (is_string) {
            write(get_indent() + "strcpy(" + var_name + ", ");
            generate_expression(stmt->expression.get(), true);
            write(");\n");
        } else {
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
    
    // Sanitize variable name to avoid C/Windows conflicts
    std::string c_var = sanitize_varname(var);
    
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
    
    // Decrement indent to get back outside the FOR body
    indent_level--;
    
    // Generate label for NEXT if it has a line number
    // BUT only if it's different from the FOR's line number (to avoid duplicates)
    if (stmt->next_line_num > 0 && stmt->next_line_num != stmt->line_number) {
        auto it = line_labels.find(stmt->next_line_num);
        if (it != line_labels.end()) {
            writeln("L" + std::to_string(it->second) + ":;");
        }
    }
    
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
            
            // Sanitize variable name to avoid C/Windows conflicts
            var_name = sanitize_varname(var_name);
            
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
        writeln("printf(\"%s\", \"" + escape_string(stmt->prompt) + "\");");
        writeln("fflush(stdout);  // Ensure prompt displays immediately");
    }
    
    for (const auto& var : stmt->variables) {
        std::string var_name = var;
        bool is_string = (!var_name.empty() && var_name.back() == '$');
        
        if (is_string) {
            var_name.pop_back();
            var_name += "_str";
        }
        
        // Sanitize variable name to avoid C/Windows conflicts
        var_name = sanitize_varname(var_name);
        
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
        writeln("printf(\"%s\", \"" + escape_string(stmt->prompt) + "\");");
        writeln("fflush(stdout);  // Ensure prompt displays immediately");
    }
    
    std::string var_name = stmt->variable;
    if (!var_name.empty() && var_name.back() == '$') {
        var_name.pop_back();
        var_name += "_str";
    }
    
    // Sanitize variable name to avoid C/Windows conflicts
    var_name = sanitize_varname(var_name);
    
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
        
        // Sanitize variable name to avoid C/Windows conflicts
        var_name = sanitize_varname(var_name);
        
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

void CodeGenerator::generate_dim_stmt(const DimStmt* stmt) {
    // Generate runtime allocation for dynamic arrays
    for (const auto& array_decl : stmt->arrays) {
        // Handle typed variable declarations (DIM var AS TypeName)
        if (!array_decl.type_name.empty()) {
            typed_variables[sanitize_varname(array_decl.name)] = array_decl.type_name;
            continue;  // Skip array allocation
        }

        std::string arr_name = array_decl.name;

        // Check if this array is dynamic
        auto it = arrays.find(arr_name);
        bool is_string_array = false;
        bool is_dynamic = false;
        
        if (it != arrays.end()) {
            is_dynamic = it->second.is_dynamic;
        } else {
            // Check string arrays
            auto sit = string_arrays.find(arr_name);
            if (sit != string_arrays.end()) {
                is_dynamic = sit->second.is_dynamic;
                is_string_array = true;
            }
        }
        
        if (is_dynamic) {
            // Generate dynamic allocation code
            writeln(get_indent() + "{");
            indent_level++;
            
            // Calculate total size
            write(get_indent() + "int _size = 1;");
            writeln("");
            
            for (const auto& dim_expr : array_decl.dimension_exprs) {
                write(get_indent() + "_size *= ((int)");
                generate_expression(dim_expr.get());
                write(" + 1);");
                writeln("");
            }
            
            if (is_string_array) {
                // Allocate string array
                std::string c_name = arr_name;
                if (!c_name.empty() && c_name.back() == '$') {
                    c_name.pop_back();
                    c_name += "_str";
                }
                // Sanitize array name
                c_name = sanitize_varname(c_name);
                writeln(get_indent() + c_name + " = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);");
                writeln(get_indent() + "if (!" + c_name + ") {");
                writeln(get_indent() + "    fprintf(stderr, \"Out of memory allocating array " + arr_name + "\\n\");");
                writeln(get_indent() + "    exit(1);");
                writeln(get_indent() + "}");
            } else {
                // Allocate numeric array
                // Sanitize array name
                std::string c_name = sanitize_varname(arr_name);
                writeln(get_indent() + c_name + " = (double*)calloc(_size, sizeof(double));");
                writeln(get_indent() + "if (!" + c_name + ") {");
                writeln(get_indent() + "    fprintf(stderr, \"Out of memory allocating array " + arr_name + "\\n\");");
                writeln(get_indent() + "    exit(1);");
                writeln(get_indent() + "}");
            }
            
            indent_level--;
            writeln(get_indent() + "}");
        }
        // Static arrays don't need runtime initialization
    }
}

void CodeGenerator::generate_open_stmt(const OpenStmt* stmt) {
    std::string mode = "r";
    if (stmt->mode == "OUTPUT") {
        mode = "w";
    } else if (stmt->mode == "APPEND") {
        mode = "a";
    } else if (stmt->mode == "BINARY") {
        mode = "rb+";
    } else if (stmt->mode == "RANDOM") {
        mode = "rb+";
    }

    std::string fnum = std::to_string(stmt->file_number);

    // For BINARY and RANDOM modes, try to open existing, create if not found
    if (stmt->mode == "BINARY" || stmt->mode == "RANDOM") {
        write(get_indent() + "_file_handles[" + fnum + "] = fopen(");
        if (stmt->filename->type == NodeType::STRING) {
            const StringNode* str = static_cast<const StringNode*>(stmt->filename.get());
            write("\"" + escape_string(str->value) + "\"");
        } else if (stmt->filename->type == NodeType::VARIABLE) {
            const VariableNode* var = static_cast<const VariableNode*>(stmt->filename.get());
            std::string var_name = var->name;
            if (!var_name.empty() && var_name.back() == '$') {
                var_name.pop_back();
                var_name += "_str";
            }
            write(var_name);
        } else {
            generate_expression(stmt->filename.get(), true);
        }
        write(", \"" + mode + "\");\n");

        // If file doesn't exist, create it
        writeln("if (!_file_handles[" + fnum + "]) {");
        indent_level++;
        write(get_indent() + "_file_handles[" + fnum + "] = fopen(");
        if (stmt->filename->type == NodeType::STRING) {
            const StringNode* str = static_cast<const StringNode*>(stmt->filename.get());
            write("\"" + escape_string(str->value) + "\"");
        } else if (stmt->filename->type == NodeType::VARIABLE) {
            const VariableNode* var = static_cast<const VariableNode*>(stmt->filename.get());
            std::string var_name = var->name;
            if (!var_name.empty() && var_name.back() == '$') {
                var_name.pop_back();
                var_name += "_str";
            }
            write(var_name);
        } else {
            generate_expression(stmt->filename.get(), true);
        }
        write(", \"wb+\");\n");
        indent_level--;
        writeln("}");

        if (stmt->mode == "RANDOM") {
            writeln("_file_reclen[" + fnum + "] = " + std::to_string(stmt->record_length) + ";");
        }

        // Error check
        if (has_error_handling) {
            int rn_id = error_check_counter++;
            writeln("_resume_next_id = " + std::to_string(rn_id) + ";");
            writeln("if (!_file_handles[" + fnum + "]) {");
            indent_level++;
            writeln("_err = 53; _erl = " + std::to_string(current_basic_line) + ";");
            writeln("if (_error_active) goto _error_dispatch;");
            writeln("fprintf(stderr, \"File not found error %d at line %d\\n\", _err, _erl); exit(1);");
            indent_level--;
            writeln("}");
            writeln("_rn_" + std::to_string(rn_id) + ":;");
        }
    } else {
        // Standard text modes (INPUT/OUTPUT/APPEND)
        write(get_indent() + "_file_handles[" + fnum + "] = fopen(");

        if (stmt->filename->type == NodeType::STRING) {
            const StringNode* str = static_cast<const StringNode*>(stmt->filename.get());
            write("\"" + escape_string(str->value) + "\"");
        } else if (stmt->filename->type == NodeType::VARIABLE) {
            const VariableNode* var = static_cast<const VariableNode*>(stmt->filename.get());
            std::string var_name = var->name;
            if (!var_name.empty() && var_name.back() == '$') {
                var_name.pop_back();
                var_name += "_str";
            }
            write(var_name);
        } else {
            generate_expression(stmt->filename.get(), true);
        }

        write(", \"" + mode + "\");\n");

        // Error check for file open
        if (has_error_handling) {
            int rn_id = error_check_counter++;
            writeln("_resume_next_id = " + std::to_string(rn_id) + ";");
            writeln("if (!_file_handles[" + fnum + "]) {");
            indent_level++;
            writeln("_err = 53; _erl = " + std::to_string(current_basic_line) + ";");
            writeln("if (_error_active) goto _error_dispatch;");
            writeln("fprintf(stderr, \"File not found error %d at line %d\\n\", _err, _erl); exit(1);");
            indent_level--;
            writeln("}");
            writeln("_rn_" + std::to_string(rn_id) + ":;");
        }
    }
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
        writeln("if (_auto_display) SDL_RenderPresent(renderer);");
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
    // Check if this is SCREEN 0 (text mode)
    if (stmt->mode && stmt->mode->type == NodeType::NUMBER) {
        const NumberNode* num = static_cast<const NumberNode*>(stmt->mode.get());
        if (num->value == 0) {
            // SCREEN 0 is text mode - just a comment, no graphics initialization
            writeln("// SCREEN 0 - Text mode (no action needed)");
            return;
        }
    }
    
    // Graphics mode (SCREEN 1-13)
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
    
    writeln("set_pixel(_x, _y, current_color);");
    writeln("if (_auto_display) SDL_RenderPresent(renderer);");

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
    
    if (stmt->is_box) {
        if (stmt->is_filled) {
            // Draw filled box
            writeln("fill_rect_buffered(_x1, _y1, _x2 - _x1, _y2 - _y1, current_color);");
        } else {
            // Draw box outline
            writeln("draw_line_buffered(_x1, _y1, _x2, _y1, current_color);");
            writeln("draw_line_buffered(_x2, _y1, _x2, _y2, current_color);");
            writeln("draw_line_buffered(_x2, _y2, _x1, _y2, current_color);");
            writeln("draw_line_buffered(_x1, _y2, _x1, _y1, current_color);");
        }
    } else {
        // Draw simple line
        writeln("draw_line_buffered(_x1, _y1, _x2, _y2, current_color);");
    }

    writeln("if (_auto_display) SDL_RenderPresent(renderer);");
    
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
    
    writeln("flood_fill(_x, _y, _fill_color, _border_color);");
    
    indent_level--;
    writeln("}");
}

// ============== DRAW STATEMENT GENERATOR ==============

void CodeGenerator::generate_draw_stmt(const DrawStmt* stmt) {
    needs_sdl = true;

    writeln("{");
    indent_level++;

    // Generate the command string expression
    if (is_string_expr(stmt->command_string.get())) {
        writeln("char _draw_cmd[RB_STR_SIZE] = \"\";");
        write(get_indent() + "strcpy(_draw_cmd, ");
        generate_expression(stmt->command_string.get(), true);
        write(");\n");
    } else {
        write(get_indent() + "char _draw_cmd[RB_STR_SIZE]; sprintf(_draw_cmd, \"%g\", (double)(");
        generate_expression(stmt->command_string.get());
        write("));\n");
    }

    writeln("RB_DRAW(_draw_cmd);");

    indent_level--;
    writeln("}");
}

// ============== PALETTE STATEMENT GENERATOR ==============

void CodeGenerator::generate_palette_stmt(const PaletteStmt* stmt) {
    needs_sdl = true;

    if (!stmt->attribute) {
        // PALETTE with no args = reset to default
        writeln("{ // PALETTE reset");
        indent_level++;
        writeln("SDL_Color _def_pal[16] = {");
        writeln("    {0,0,0,255},{0,0,170,255},{0,170,0,255},{0,170,170,255},");
        writeln("    {170,0,0,255},{170,0,170,255},{170,85,0,255},{170,170,170,255},");
        writeln("    {85,85,85,255},{85,85,255,255},{85,255,85,255},{85,255,255,255},");
        writeln("    {255,85,85,255},{255,85,255,255},{255,255,85,255},{255,255,255,255}");
        writeln("};");
        writeln("for (int _pi = 0; _pi < 16; _pi++) palette[_pi] = _def_pal[_pi];");
        indent_level--;
        writeln("}");
    } else {
        writeln("{");
        indent_level++;

        write(get_indent() + "int _pattr = (int)(");
        generate_expression(stmt->attribute.get());
        write(");\n");

        write(get_indent() + "int _pcolor = (int)(");
        generate_expression(stmt->color.get());
        write(");\n");

        writeln("if (_pattr >= 0 && _pattr < 16) {");
        indent_level++;
        writeln("// Extract RGB from packed color value (QBasic format: &HRRGGBB or 6-bit EGA)");
        writeln("int _r, _g, _b;");
        writeln("if (_pcolor > 63) {");
        indent_level++;
        writeln("// Treat as packed RGB (&HRRGGBB)");
        writeln("_r = (_pcolor >> 16) & 0xFF;");
        writeln("_g = (_pcolor >> 8) & 0xFF;");
        writeln("_b = _pcolor & 0xFF;");
        indent_level--;
        writeln("} else {");
        indent_level++;
        writeln("// 6-bit EGA color: RGBRGB (2 bits each, high/low intensity)");
        writeln("_r = ((_pcolor & 4) ? 170 : 0) + ((_pcolor & 32) ? 85 : 0);");
        writeln("_g = ((_pcolor & 2) ? 170 : 0) + ((_pcolor & 16) ? 85 : 0);");
        writeln("_b = ((_pcolor & 1) ? 170 : 0) + ((_pcolor & 8) ? 85 : 0);");
        indent_level--;
        writeln("}");
        writeln("palette[_pattr].r = _r;");
        writeln("palette[_pattr].g = _g;");
        writeln("palette[_pattr].b = _b;");
        writeln("palette[_pattr].a = 255;");
        indent_level--;
        writeln("}");

        indent_level--;
        writeln("}");
    }
}

// ============== VIEW STATEMENT GENERATOR ==============

void CodeGenerator::generate_view_stmt(const ViewStmt* stmt) {
    needs_sdl = true;

    if (!stmt->x1) {
        // VIEW with no args = reset clip rect
        writeln("SDL_RenderSetClipRect(renderer, NULL);  // VIEW reset");
    } else {
        writeln("{");
        indent_level++;

        write(get_indent() + "int _vx1 = (int)(");
        generate_expression(stmt->x1.get());
        write(");\n");
        write(get_indent() + "int _vy1 = (int)(");
        generate_expression(stmt->y1.get());
        write(");\n");
        write(get_indent() + "int _vx2 = (int)(");
        generate_expression(stmt->x2.get());
        write(");\n");
        write(get_indent() + "int _vy2 = (int)(");
        generate_expression(stmt->y2.get());
        write(");\n");

        writeln("SDL_Rect _clip = {_vx1, _vy1, _vx2 - _vx1 + 1, _vy2 - _vy1 + 1};");
        writeln("SDL_RenderSetClipRect(renderer, &_clip);");

        indent_level--;
        writeln("}");
    }
}

// ============== WINDOW STATEMENT GENERATOR ==============

void CodeGenerator::generate_window_stmt(const WindowStmt* stmt) {
    needs_sdl = true;

    if (!stmt->x1) {
        writeln("// WINDOW reset (logical coordinates = physical coordinates)");
    } else {
        writeln("{");
        indent_level++;

        writeln("// WINDOW - logical coordinate mapping (stored for reference)");
        write(get_indent() + "// Logical x1 = ");
        generate_expression(stmt->x1.get());
        write(", y1 = ");
        generate_expression(stmt->y1.get());
        write("\n");
        write(get_indent() + "// Logical x2 = ");
        generate_expression(stmt->x2.get());
        write(", y2 = ");
        generate_expression(stmt->y2.get());
        write("\n");
        writeln("// TODO: Full WINDOW coordinate mapping not yet implemented");

        indent_level--;
        writeln("}");
    }
}

// ============== PCOPY STATEMENT GENERATOR ==============

void CodeGenerator::generate_pcopy_stmt(const PcopyStmt* stmt) {
    needs_sdl = true;

    writeln("{");
    indent_level++;

    write(get_indent() + "int _src_page = (int)(");
    generate_expression(stmt->src_page.get());
    write(");\n");
    write(get_indent() + "int _dest_page = (int)(");
    generate_expression(stmt->dest_page.get());
    write(");\n");
    writeln("// PCOPY %d, %d - page copy (single-page MVP, no-op)");
    writeln("(void)_src_page; (void)_dest_page;");

    indent_level--;
    writeln("}");
}

// ============== GET GRAPHICS STATEMENT GENERATOR ==============

void CodeGenerator::generate_get_gfx_stmt(const GetGfxStmt* stmt) {
    needs_sdl = true;

    std::string arr_name = sanitize_varname(stmt->array_name);

    writeln("{");
    indent_level++;

    write(get_indent() + "int _gx1 = (int)(");
    generate_expression(stmt->x1.get());
    write(");\n");
    write(get_indent() + "int _gy1 = (int)(");
    generate_expression(stmt->y1.get());
    write(");\n");
    write(get_indent() + "int _gx2 = (int)(");
    generate_expression(stmt->x2.get());
    write(");\n");
    write(get_indent() + "int _gy2 = (int)(");
    generate_expression(stmt->y2.get());
    write(");\n");

    writeln("int _gw = _gx2 - _gx1 + 1, _gh = _gy2 - _gy1 + 1;");
    writeln(arr_name + "[0] = (double)_gw;");
    writeln(arr_name + "[1] = (double)_gh;");
    writeln("for (int _gy = 0; _gy < _gh; _gy++)");
    indent_level++;
    writeln("for (int _gx = 0; _gx < _gw; _gx++)");
    indent_level++;
    writeln(arr_name + "[2 + _gy * _gw + _gx] = (double)get_pixel_color(_gx1 + _gx, _gy1 + _gy);");
    indent_level -= 2;

    indent_level--;
    writeln("}");
}

// ============== PUT GRAPHICS STATEMENT GENERATOR ==============

void CodeGenerator::generate_put_gfx_stmt(const PutGfxStmt* stmt) {
    needs_sdl = true;

    std::string arr_name = sanitize_varname(stmt->array_name);

    writeln("{");
    indent_level++;

    write(get_indent() + "int _px = (int)(");
    generate_expression(stmt->x.get());
    write(");\n");
    write(get_indent() + "int _py = (int)(");
    generate_expression(stmt->y.get());
    write(");\n");

    writeln("int _pw = (int)" + arr_name + "[0], _ph = (int)" + arr_name + "[1];");
    writeln("for (int _piy = 0; _piy < _ph; _piy++) {");
    indent_level++;
    writeln("for (int _pix = 0; _pix < _pw; _pix++) {");
    indent_level++;
    writeln("int _src = (int)" + arr_name + "[2 + _piy * _pw + _pix];");

    if (stmt->action == "PSET") {
        writeln("set_pixel(_px + _pix, _py + _piy, _src);");
    } else if (stmt->action == "PRESET") {
        writeln("set_pixel(_px + _pix, _py + _piy, _src ^ 15);");
    } else if (stmt->action == "AND") {
        writeln("int _dst = get_pixel_color(_px + _pix, _py + _piy);");
        writeln("set_pixel(_px + _pix, _py + _piy, _dst & _src);");
    } else if (stmt->action == "OR") {
        writeln("int _dst = get_pixel_color(_px + _pix, _py + _piy);");
        writeln("set_pixel(_px + _pix, _py + _piy, _dst | _src);");
    } else {
        // XOR (default)
        writeln("int _dst = get_pixel_color(_px + _pix, _py + _piy);");
        writeln("set_pixel(_px + _pix, _py + _piy, _dst ^ _src);");
    }

    indent_level--;
    writeln("}");
    indent_level--;
    writeln("}");
    writeln("if (_auto_display) SDL_RenderPresent(renderer);");

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
    write(get_indent() + "RB_PLAY(");
    generate_expression(stmt->music_string.get(), true);
    write(");\n");
    indent_level--;
    writeln("}");
}

// ============== SHELL STATEMENT ==============

void CodeGenerator::generate_shell_stmt(const ShellStmt* stmt) {
    if (stmt->command) {
        write(get_indent() + "system(");
        generate_expression(stmt->command.get(), true);
        write(");\n");
    } else {
        writeln("system(NULL);");
    }
}

// ============== POKE STATEMENT ==============

void CodeGenerator::generate_poke_stmt(const PokeStmt* stmt) {
    write(get_indent() + "_mem_segment[(int)(");
    generate_expression(stmt->address.get());
    write(") & 0xFFFF] = (unsigned char)(int)(");
    generate_expression(stmt->value.get());
    write(");\n");
}

// ============== DEF SEG STATEMENT ==============

void CodeGenerator::generate_def_seg_stmt(const DefSegStmt* stmt) {
    if (stmt->segment) {
        write(get_indent() + "_def_seg = (int)(");
        generate_expression(stmt->segment.get());
        write(");\n");
    } else {
        writeln("_def_seg = 0;");
    }
}

// ============== PHASE 6: QB64 EXTENSION CODE GENERATION ==============

void CodeGenerator::generate_display_stmt(const DisplayStmt* stmt) {
    (void)stmt;
    writeln("SDL_RenderPresent(renderer);");
}

void CodeGenerator::generate_limit_stmt(const LimitStmt* stmt) {
    writeln("{ // _LIMIT");
    indent_level++;
    writeln("rb_process_events();");
    write(get_indent() + "int _fps = (int)(");
    generate_expression(stmt->fps.get());
    write(");\n");
    writeln("if (_fps > 0) SDL_Delay(1000 / _fps);");
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_freeimage_stmt(const FreeImageStmt* stmt) {
    writeln("{ // _FREEIMAGE");
    indent_level++;
    write(get_indent() + "int _h = (int)(");
    generate_expression(stmt->handle.get());
    write(");\n");
    writeln("if (_h > 0 && _h < MAX_IMAGES && _images[_h]) {");
    indent_level++;
    writeln("SDL_DestroyTexture(_images[_h]);");
    writeln("_images[_h] = NULL;");
    indent_level--;
    writeln("}");
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_putimage_stmt(const PutImageStmt* stmt) {
    writeln("{ // _PUTIMAGE");
    indent_level++;
    write(get_indent() + "int _src_h = (int)(");
    generate_expression(stmt->source_handle.get());
    write(");\n");
    writeln("if (_src_h > 0 && _src_h < MAX_IMAGES && _images[_src_h]) {");
    indent_level++;
    if (stmt->dx && stmt->dy) {
        write(get_indent() + "SDL_Rect _dst = {(int)(");
        generate_expression(stmt->dx.get());
        write("), (int)(");
        generate_expression(stmt->dy.get());
        write("), _image_w[_src_h], _image_h[_src_h]};\n");
        writeln("SDL_RenderCopy(renderer, _images[_src_h], NULL, &_dst);");
    } else {
        writeln("SDL_RenderCopy(renderer, _images[_src_h], NULL, NULL);");
    }
    writeln("if (_auto_display) SDL_RenderPresent(renderer);");
    indent_level--;
    writeln("}");
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_printstring_stmt(const PrintStringStmt* stmt) {
    writeln("{ // _PRINTSTRING");
    indent_level++;
    write(get_indent() + "rb_printstring((int)(");
    generate_expression(stmt->x.get());
    write("), (int)(");
    generate_expression(stmt->y.get());
    write("), ");
    generate_expression(stmt->text.get(), true);
    write(");\n");
    writeln("if (_auto_display) SDL_RenderPresent(renderer);");
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_sndplay_stmt(const SndPlayStmt* stmt) {
    writeln("{ // _SNDPLAY");
    indent_level++;
    write(get_indent() + "int _sh = (int)(");
    generate_expression(stmt->handle.get());
    write(");\n");
    writeln("if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {");
    indent_level++;
    writeln("_sound_channels[_sh] = Mix_PlayChannel(-1, _sounds[_sh], 0);");
    indent_level--;
    writeln("}");
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_sndstop_stmt(const SndStopStmt* stmt) {
    writeln("{ // _SNDSTOP");
    indent_level++;
    write(get_indent() + "int _sh = (int)(");
    generate_expression(stmt->handle.get());
    write(");\n");
    writeln("if (_sh > 0 && _sh < MAX_SOUNDS && _sound_channels[_sh] >= 0) {");
    indent_level++;
    writeln("Mix_HaltChannel(_sound_channels[_sh]);");
    indent_level--;
    writeln("}");
    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_sndvol_stmt(const SndVolStmt* stmt) {
    writeln("{ // _SNDVOL");
    indent_level++;
    write(get_indent() + "int _sh = (int)(");
    generate_expression(stmt->handle.get());
    write(");\n");
    write(get_indent() + "int _vol = (int)(");
    generate_expression(stmt->volume.get());
    write(" * 128);\n");
    writeln("if (_sh > 0 && _sh < MAX_SOUNDS && _sounds[_sh]) {");
    indent_level++;
    writeln("Mix_VolumeChunk(_sounds[_sh], _vol < 0 ? 0 : _vol > 128 ? 128 : _vol);");
    indent_level--;
    writeln("}");
    indent_level--;
    writeln("}");
}


void CodeGenerator::generate_statement(const Statement* stmt) {
    // Generate line label if this statement has one
    if (stmt->line_num > 0) {
        auto it = line_labels.find(stmt->line_num);
        if (it != line_labels.end()) {
            // Temporarily outdent to write label at current level - 1
            int saved_indent = indent_level;
            if (indent_level > 0) {
                indent_level--;
            }
            writeln("L" + std::to_string(it->second) + ":;");
            indent_level = saved_indent;  // Restore original indent
        }
    }
    
    if (!stmt->stmt) return;
    
    // Store current statement line number for use by nested structures
    int current_stmt_line = stmt->line_num;
    current_basic_line = stmt->line_num;

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

        case NodeType::SELECT_CASE_STMT:
            generate_select_case_stmt(static_cast<const SelectCaseStmt*>(stmt->stmt.get()));
            break;

        case NodeType::EXIT_STMT:
            generate_exit_stmt(static_cast<const ExitStmt*>(stmt->stmt.get()));
            break;

        case NodeType::CONST_STMT:
            generate_const_stmt(static_cast<const ConstStmt*>(stmt->stmt.get()));
            break;

        case NodeType::SWAP_STMT:
            generate_swap_stmt(static_cast<const SwapStmt*>(stmt->stmt.get()));
            break;

        case NodeType::DECLARE_STMT:
            // DECLARE is a no-op - SUBs/FUNCTIONs are already forward-declared
            break;

        case NodeType::REDIM_STMT:
            generate_redim_stmt(static_cast<const RedimStmt*>(stmt->stmt.get()));
            break;

        case NodeType::ERASE_STMT:
            generate_erase_stmt(static_cast<const EraseStmt*>(stmt->stmt.get()));
            break;

        case NodeType::ON_ERROR_STMT:
            generate_on_error_stmt(static_cast<const OnErrorStmt*>(stmt->stmt.get()));
            break;

        case NodeType::RESUME_STMT:
            generate_resume_stmt(static_cast<const ResumeStmt*>(stmt->stmt.get()));
            break;

        case NodeType::WRITE_STMT:
            generate_write_stmt(static_cast<const WriteStmt*>(stmt->stmt.get()));
            break;

        case NodeType::SEEK_STMT:
            generate_seek_stmt(static_cast<const SeekStmt*>(stmt->stmt.get()));
            break;

        case NodeType::GET_FILE_STMT:
            generate_get_file_stmt(static_cast<const GetFileStmt*>(stmt->stmt.get()));
            break;

        case NodeType::PUT_FILE_STMT:
            generate_put_file_stmt(static_cast<const PutFileStmt*>(stmt->stmt.get()));
            break;

        case NodeType::MID_ASSIGN_STMT:
            generate_mid_assign_stmt(static_cast<const MidAssignStmt*>(stmt->stmt.get()));
            break;

        case NodeType::TYPE_DEF_STMT:
            generate_type_def_stmt(static_cast<const TypeDefStmt*>(stmt->stmt.get()));
            break;

        case NodeType::DRAW_STMT:
            generate_draw_stmt(static_cast<const DrawStmt*>(stmt->stmt.get()));
            break;

        case NodeType::PALETTE_STMT:
            generate_palette_stmt(static_cast<const PaletteStmt*>(stmt->stmt.get()));
            break;

        case NodeType::VIEW_STMT:
            generate_view_stmt(static_cast<const ViewStmt*>(stmt->stmt.get()));
            break;

        case NodeType::WINDOW_STMT:
            generate_window_stmt(static_cast<const WindowStmt*>(stmt->stmt.get()));
            break;

        case NodeType::PCOPY_STMT:
            generate_pcopy_stmt(static_cast<const PcopyStmt*>(stmt->stmt.get()));
            break;

        case NodeType::GET_GFX_STMT:
            generate_get_gfx_stmt(static_cast<const GetGfxStmt*>(stmt->stmt.get()));
            break;

        case NodeType::PUT_GFX_STMT:
            generate_put_gfx_stmt(static_cast<const PutGfxStmt*>(stmt->stmt.get()));
            break;

        case NodeType::SHELL_STMT:
            generate_shell_stmt(static_cast<const ShellStmt*>(stmt->stmt.get()));
            break;

        case NodeType::POKE_STMT:
            generate_poke_stmt(static_cast<const PokeStmt*>(stmt->stmt.get()));
            break;

        case NodeType::DEF_SEG_STMT:
            generate_def_seg_stmt(static_cast<const DefSegStmt*>(stmt->stmt.get()));
            break;

        case NodeType::DATA_STMT:
            // DATA statements are collected and processed separately
            break;

        case NodeType::DEF_FN_STMT:
            // DEF FN statements are generated as macros at the top
            break;

        // Phase 6: QB64 Extensions
        case NodeType::DISPLAY_STMT:
            generate_display_stmt(static_cast<const DisplayStmt*>(stmt->stmt.get()));
            break;
        case NodeType::LIMIT_STMT:
            generate_limit_stmt(static_cast<const LimitStmt*>(stmt->stmt.get()));
            break;
        case NodeType::FREEIMAGE_STMT:
            generate_freeimage_stmt(static_cast<const FreeImageStmt*>(stmt->stmt.get()));
            break;
        case NodeType::PUTIMAGE_STMT:
            generate_putimage_stmt(static_cast<const PutImageStmt*>(stmt->stmt.get()));
            break;
        case NodeType::PRINTSTRING_STMT:
            generate_printstring_stmt(static_cast<const PrintStringStmt*>(stmt->stmt.get()));
            break;
        case NodeType::SNDPLAY_STMT:
            generate_sndplay_stmt(static_cast<const SndPlayStmt*>(stmt->stmt.get()));
            break;
        case NodeType::SNDSTOP_STMT:
            generate_sndstop_stmt(static_cast<const SndStopStmt*>(stmt->stmt.get()));
            break;
        case NodeType::SNDVOL_STMT:
            generate_sndvol_stmt(static_cast<const SndVolStmt*>(stmt->stmt.get()));
            break;

        default:
            throw std::runtime_error("Unknown statement type in code generation: " +
                                   std::to_string(static_cast<int>(stmt->stmt->type)));
    }
}

// ============== SELECT CASE CODE GENERATION ==============

void CodeGenerator::generate_select_case_stmt(const SelectCaseStmt* stmt) {
    bool is_string = is_string_expr(stmt->test_expression.get());

    // Store test expression in a temp variable
    if (is_string) {
        writeln("{ // SELECT CASE");
        indent_level++;
        write(get_indent() + "char _select_val[RB_STR_SIZE]; strcpy(_select_val, ");
        generate_expression(stmt->test_expression.get(), true);
        write(");\n");
    } else {
        writeln("{ // SELECT CASE");
        indent_level++;
        write(get_indent() + "double _select_val = ");
        generate_expression(stmt->test_expression.get());
        write(";\n");
    }

    bool first_case = true;
    for (const auto& clause : stmt->cases) {
        if (clause.is_else) {
            if (first_case) {
                writeln("{ // CASE ELSE");
            } else {
                writeln("} else { // CASE ELSE");
            }
            indent_level++;
            for (const auto& body_stmt : clause.body) {
                generate_statement(body_stmt.get());
            }
            indent_level--;
        } else {
            // Build condition
            if (first_case) {
                write(get_indent() + "if (");
            } else {
                write(get_indent() + "} else if (");
            }

            for (size_t i = 0; i < clause.tests.size(); i++) {
                if (i > 0) write(" || ");
                const auto& test = clause.tests[i];

                switch (test.kind) {
                    case SelectCaseStmt::CaseClause::CaseTest::VALUE:
                        if (is_string) {
                            write("strcmp(_select_val, ");
                            generate_expression(test.value.get(), true);
                            write(") == 0");
                        } else {
                            write("_select_val == ");
                            generate_expression(test.value.get());
                        }
                        break;
                    case SelectCaseStmt::CaseClause::CaseTest::RANGE:
                        if (is_string) {
                            write("(strcmp(_select_val, ");
                            generate_expression(test.value.get(), true);
                            write(") >= 0 && strcmp(_select_val, ");
                            generate_expression(test.range_end.get(), true);
                            write(") <= 0)");
                        } else {
                            write("(_select_val >= ");
                            generate_expression(test.value.get());
                            write(" && _select_val <= ");
                            generate_expression(test.range_end.get());
                            write(")");
                        }
                        break;
                    case SelectCaseStmt::CaseClause::CaseTest::IS_COMPARE: {
                        std::string c_op = test.is_op;
                        if (c_op == "<>") c_op = "!=";
                        if (c_op == "=") c_op = "==";

                        if (is_string) {
                            write("strcmp(_select_val, ");
                            generate_expression(test.value.get(), true);
                            write(") " + c_op + " 0");
                        } else {
                            write("_select_val " + c_op + " ");
                            generate_expression(test.value.get());
                        }
                        break;
                    }
                }
            }

            write(") {\n");
            indent_level++;
            for (const auto& body_stmt : clause.body) {
                generate_statement(body_stmt.get());
            }
            indent_level--;
        }
        first_case = false;
    }

    if (!stmt->cases.empty()) {
        writeln("}");
    }

    indent_level--;
    writeln("} // END SELECT");
}

// ============== EXIT CODE GENERATION ==============

void CodeGenerator::generate_exit_stmt(const ExitStmt* stmt) {
    if (stmt->exit_type == "FOR" || stmt->exit_type == "DO" || stmt->exit_type == "WHILE") {
        writeln("break;");
    } else if (stmt->exit_type == "SUB") {
        writeln("return;");
    } else if (stmt->exit_type == "FUNCTION") {
        writeln("return _result;");
    }
}

// ============== CONST CODE GENERATION ==============

void CodeGenerator::generate_const_stmt(const ConstStmt* stmt) {
    // Emit as a runtime assignment (the variable is already declared globally)
    std::string var_name = stmt->name;
    bool is_string = (!var_name.empty() && var_name.back() == '$');

    if (is_string) {
        var_name.pop_back();
        var_name += "_str";
        var_name = sanitize_varname(var_name);
        write(get_indent() + "strcpy(" + var_name + ", ");
        generate_expression(stmt->value.get(), true);
        write(");\n");
    } else {
        var_name = sanitize_varname(var_name);
        write(get_indent() + var_name + " = ");
        generate_expression(stmt->value.get());
        write(";\n");
    }
}

// ============== SWAP CODE GENERATION ==============

void CodeGenerator::generate_swap_stmt(const SwapStmt* stmt) {
    bool is_string = (!stmt->var1.empty() && stmt->var1.back() == '$');

    auto get_c_name = [this](const std::string& name) -> std::string {
        std::string c = name;
        if (!c.empty() && c.back() == '$') {
            c.pop_back();
            c += "_str";
        }
        return sanitize_varname(c);
    };

    std::string c_var1 = get_c_name(stmt->var1);
    std::string c_var2 = get_c_name(stmt->var2);

    writeln("{ // SWAP");
    indent_level++;
    if (is_string) {
        writeln("char _swap_temp[RB_STR_SIZE];");
        writeln("strcpy(_swap_temp, " + c_var1 + ");");
        writeln("strcpy(" + c_var1 + ", " + c_var2 + ");");
        writeln("strcpy(" + c_var2 + ", _swap_temp);");
    } else {
        writeln("double _swap_temp = " + c_var1 + ";");
        writeln(c_var1 + " = " + c_var2 + ";");
        writeln(c_var2 + " = _swap_temp;");
    }
    indent_level--;
    writeln("}");
}

// ============== REDIM CODE GENERATION ==============

void CodeGenerator::generate_redim_stmt(const RedimStmt* stmt) {
    for (const auto& array_decl : stmt->arrays) {
        std::string arr_name = array_decl.name;
        bool is_string_array = (!arr_name.empty() && arr_name.back() == '$');
        std::string c_name = arr_name;
        if (is_string_array) {
            c_name.pop_back();
            c_name += "_str";
        }
        c_name = sanitize_varname(c_name);

        writeln("{ // REDIM" + std::string(stmt->preserve ? " PRESERVE" : ""));
        indent_level++;

        // Calculate total size
        writeln("int _size = 1;");
        for (const auto& dim_expr : array_decl.dimension_exprs) {
            write(get_indent() + "_size *= ((int)");
            generate_expression(dim_expr.get());
            write(" + 1);\n");
        }

        if (stmt->preserve) {
            // Use realloc to keep existing data
            if (is_string_array) {
                writeln(c_name + " = (char (*)[RB_STR_SIZE])realloc(" + c_name + ", _size * RB_STR_SIZE);");
            } else {
                writeln(c_name + " = (double*)realloc(" + c_name + ", _size * sizeof(double));");
            }
        } else {
            // Free and reallocate
            writeln("free(" + c_name + ");");
            if (is_string_array) {
                writeln(c_name + " = (char (*)[RB_STR_SIZE])calloc(_size, RB_STR_SIZE);");
            } else {
                writeln(c_name + " = (double*)calloc(_size, sizeof(double));");
            }
        }

        writeln("if (!" + c_name + ") {");
        writeln("    fprintf(stderr, \"Out of memory in REDIM for " + arr_name + "\\n\");");
        writeln("    exit(1);");
        writeln("}");

        indent_level--;
        writeln("}");
    }
}

// ============== ERASE CODE GENERATION ==============

void CodeGenerator::generate_erase_stmt(const EraseStmt* stmt) {
    for (const auto& name : stmt->array_names) {
        std::string c_name = name;
        bool is_string_array = (!c_name.empty() && c_name.back() == '$');
        if (is_string_array) {
            c_name.pop_back();
            c_name += "_str";
        }
        c_name = sanitize_varname(c_name);
        writeln("free(" + c_name + ");");
        writeln(c_name + " = NULL;");
    }
}

// ============== ON ERROR CODE GENERATION ==============

void CodeGenerator::generate_on_error_stmt(const OnErrorStmt* stmt) {
    if (stmt->target_line == 0) {
        writeln("_error_active = 0;  // ON ERROR GOTO 0 - disable error handling");
    } else {
        writeln("_error_active = 1;  // ON ERROR GOTO " + std::to_string(stmt->target_line));
        writeln("_error_handler_line = " + std::to_string(stmt->target_line) + ";");
    }
}

void CodeGenerator::generate_resume_stmt(const ResumeStmt* stmt) {
    if (stmt->resume_type == ResumeStmt::LINE) {
        auto it = line_labels.find(stmt->target_line);
        if (it != line_labels.end()) {
            writeln("goto L" + std::to_string(it->second) + ";  // RESUME " + std::to_string(stmt->target_line));
        } else {
            writeln("// Warning: RESUME target line " + std::to_string(stmt->target_line) + " not found");
        }
    } else if (stmt->resume_type == ResumeStmt::NEXT) {
        writeln("goto _resume_next_dispatch;  // RESUME NEXT");
    } else {
        // RESUME (retry) - same as RESUME NEXT for now
        writeln("goto _resume_next_dispatch;  // RESUME");
    }
}

// ============== WRITE # CODE GENERATION ==============

void CodeGenerator::generate_write_stmt(const WriteStmt* stmt) {
    std::string fhandle;
    if (stmt->file_number > 0) {
        fhandle = "_file_handles[" + std::to_string(stmt->file_number) + "]";
    } else {
        fhandle = "stdout";
    }

    for (size_t i = 0; i < stmt->expressions.size(); i++) {
        const ASTNode* expr = stmt->expressions[i].get();

        if (i > 0) {
            writeln("fprintf(" + fhandle + ", \",\");");
        }

        if (is_string_expr(expr)) {
            // Strings are output wrapped in quotes
            writeln("fprintf(" + fhandle + ", \"\\\"\");");
            write(get_indent() + "fprintf(" + fhandle + ", \"%s\", ");
            generate_expression(expr, true);
            write(");\n");
            writeln("fprintf(" + fhandle + ", \"\\\"\");");
        } else {
            write(get_indent() + "fprintf(" + fhandle + ", \"%g\", ");
            generate_expression(expr);
            write(");\n");
        }
    }

    writeln("fprintf(" + fhandle + ", \"\\n\");");
    if (stmt->file_number == 0) {
        writeln("fflush(stdout);");
    }
}

// ============== SEEK # CODE GENERATION ==============

void CodeGenerator::generate_seek_stmt(const SeekStmt* stmt) {
    write(get_indent() + "fseek(_file_handles[" + std::to_string(stmt->file_number) + "], (long)(");
    generate_expression(stmt->position.get());
    write(") - 1, SEEK_SET);\n");
}

// ============== GET # / PUT # CODE GENERATION ==============

void CodeGenerator::generate_get_file_stmt(const GetFileStmt* stmt) {
    std::string fnum = std::to_string(stmt->file_number);

    writeln("{ // GET #" + fnum);
    indent_level++;

    // If record number given, seek first
    if (stmt->record_number) {
        write(get_indent() + "if (_file_reclen[" + fnum + "] > 0) ");
        write("fseek(_file_handles[" + fnum + "], ((long)(");
        generate_expression(stmt->record_number.get());
        write(") - 1) * _file_reclen[" + fnum + "], SEEK_SET);\n");
    }

    if (!stmt->variable.empty()) {
        std::string var_name = stmt->variable;
        bool is_string = (!var_name.empty() && var_name.back() == '$');
        if (is_string) {
            var_name.pop_back();
            var_name += "_str";
        }
        var_name = sanitize_varname(var_name);

        if (is_string) {
            writeln("{ int _reclen = _file_reclen[" + fnum + "] > 0 ? _file_reclen[" + fnum + "] : 255;");
            writeln("  fread(" + var_name + ", 1, _reclen, _file_handles[" + fnum + "]);");
            writeln("  " + var_name + "[_reclen] = '\\0'; }");
        } else {
            writeln("fread(&" + var_name + ", sizeof(double), 1, _file_handles[" + fnum + "]);");
        }
    } else {
        // No variable - read record_length bytes and discard (advance position)
        writeln("if (_file_reclen[" + fnum + "] > 0) {");
        writeln("    char _buf[4096];");
        writeln("    fread(_buf, 1, _file_reclen[" + fnum + "], _file_handles[" + fnum + "]);");
        writeln("}");
    }

    indent_level--;
    writeln("}");
}

void CodeGenerator::generate_put_file_stmt(const PutFileStmt* stmt) {
    std::string fnum = std::to_string(stmt->file_number);

    writeln("{ // PUT #" + fnum);
    indent_level++;

    // If record number given, seek first
    if (stmt->record_number) {
        write(get_indent() + "if (_file_reclen[" + fnum + "] > 0) ");
        write("fseek(_file_handles[" + fnum + "], ((long)(");
        generate_expression(stmt->record_number.get());
        write(") - 1) * _file_reclen[" + fnum + "], SEEK_SET);\n");
    }

    if (!stmt->variable.empty()) {
        std::string var_name = stmt->variable;
        bool is_string = (!var_name.empty() && var_name.back() == '$');
        if (is_string) {
            var_name.pop_back();
            var_name += "_str";
        }
        var_name = sanitize_varname(var_name);

        if (is_string) {
            writeln("{ int _reclen = _file_reclen[" + fnum + "] > 0 ? _file_reclen[" + fnum + "] : (int)strlen(" + var_name + ");");
            writeln("  fwrite(" + var_name + ", 1, _reclen, _file_handles[" + fnum + "]);");
            writeln("  fflush(_file_handles[" + fnum + "]); }");
        } else {
            writeln("fwrite(&" + var_name + ", sizeof(double), 1, _file_handles[" + fnum + "]);");
            writeln("fflush(_file_handles[" + fnum + "]);");
        }
    }

    indent_level--;
    writeln("}");
}

// ============== MID$ ASSIGNMENT CODE GENERATION ==============

void CodeGenerator::generate_mid_assign_stmt(const MidAssignStmt* stmt) {
    std::string var_name = stmt->variable;
    if (!var_name.empty() && var_name.back() == '$') {
        var_name.pop_back();
        var_name += "_str";
    }
    var_name = sanitize_varname(var_name);

    writeln("{ // MID$ assignment");
    indent_level++;

    write(get_indent() + "int _start = (int)(");
    generate_expression(stmt->start.get());
    write(") - 1;\n");

    writeln("char _repl[RB_STR_SIZE];");
    write(get_indent() + "strcpy(_repl, ");
    generate_expression(stmt->replacement.get(), true);
    write(");\n");

    writeln("int _repl_len = strlen(_repl);");
    writeln("int _var_len = strlen(" + var_name + ");");

    if (stmt->length) {
        write(get_indent() + "int _len = (int)(");
        generate_expression(stmt->length.get());
        write(");\n");
        writeln("if (_len > _repl_len) _len = _repl_len;");
    } else {
        writeln("int _len = _repl_len;");
    }

    writeln("if (_start >= 0 && _start < _var_len) {");
    writeln("    if (_start + _len > _var_len) _len = _var_len - _start;");
    writeln("    strncpy(" + var_name + " + _start, _repl, _len);");
    writeln("}");

    indent_level--;
    writeln("}");
}

// ============== TYPE DEF CODE GENERATION ==============

void CodeGenerator::generate_type_def_stmt(const TypeDefStmt* stmt) {
    // Store type info for later use
    TypeInfo info;
    info.name = stmt->type_name;
    info.fields = stmt->fields;
    user_types[stmt->type_name] = info;
    // The actual struct definition is emitted in generate_variable_declarations
}

// ============== PRINT USING CODE GENERATION ==============

void CodeGenerator::generate_print_using(const PrintStmt* stmt) {
    std::string fhandle;
    if (stmt->file_number > 0) {
        fhandle = "_file_handles[" + std::to_string(stmt->file_number) + "]";
    } else {
        fhandle = "stdout";
    }

    writeln("{ // PRINT USING");
    indent_level++;

    writeln("char _using_fmt[RB_STR_SIZE];");
    write(get_indent() + "strcpy(_using_fmt, ");
    generate_expression(stmt->using_format.get(), true);
    write(");\n");

    for (const auto& expr : stmt->expressions) {
        if (is_string_expr(expr.get())) {
            write(get_indent() + "RB_PRINT_USING_STR(" + fhandle + ", _using_fmt, ");
            generate_expression(expr.get(), true);
            write(");\n");
        } else {
            write(get_indent() + "RB_PRINT_USING_NUM(" + fhandle + ", _using_fmt, ");
            generate_expression(expr.get());
            write(");\n");
        }
    }

    writeln("fprintf(" + fhandle + ", \"\\n\");");
    if (stmt->file_number == 0) {
        writeln("fflush(stdout);");
    }

    indent_level--;
    writeln("}");
}

void CodeGenerator::generate(const Program* program) {
    // First, detect if graphics or sound are used
    detect_graphics_sound(program);
    
    // Collect all necessary information
    collect_variables(program);
    collect_line_labels(program);
    collect_data_values(program);

    // Collect type definitions (pre-pass)
    for (const auto& stmt : program->statements) {
        if (stmt->stmt && stmt->stmt->type == NodeType::TYPE_DEF_STMT) {
            const TypeDefStmt* type_def = static_cast<const TypeDefStmt*>(stmt->stmt.get());
            TypeInfo info;
            info.name = type_def->type_name;
            info.fields = type_def->fields;
            user_types[type_def->type_name] = info;
        }
    }
    
    // Generate code
    generate_headers();

    // Define string buffer size constant before variable declarations
    writeln("#define RB_STR_SIZE 4096");
    writeln("");

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
    indent_level++;

    // Build COMMAND$ from argv
    writeln("// Build COMMAND$ from command line arguments");
    writeln("if (argc > 1) {");
    writeln("    _command_str[0] = '\\0';");
    writeln("    for (int _ci = 1; _ci < argc; _ci++) {");
    writeln("        if (_ci > 1) strncat(_command_str, \" \", RB_STR_SIZE - strlen(_command_str) - 1);");
    writeln("        strncat(_command_str, argv[_ci], RB_STR_SIZE - strlen(_command_str) - 1);");
    writeln("    }");
    writeln("}");
    writeln("");
    
    // Always declare temp strings - they're used by string operations,
    // _PRINTSTRING, PRINT with string expressions, etc.
    writeln("char _temp_str[RB_STR_SIZE];");
    writeln("char _temp_str2[RB_STR_SIZE];");  // For nested string operations
    writeln("char _temp_str3[RB_STR_SIZE];");  // For deeply nested operations
    writeln("");
    
    // Generate main program statements
    for (const auto& stmt : program->statements) {
        generate_statement(stmt.get());
    }
    
    writeln("");

    // Generate error dispatch code if error handling is used
    if (has_error_handling) {
        writeln("goto _skip_error_dispatch;");
        writeln("");
        writeln("_error_dispatch:;");
        indent_level++;
        writeln("switch(_error_handler_line) {");
        indent_level++;
        for (int target_line : on_error_targets) {
            auto it = line_labels.find(target_line);
            if (it != line_labels.end()) {
                writeln("case " + std::to_string(target_line) + ": goto L" + std::to_string(it->second) + ";");
            }
        }
        writeln("default: fprintf(stderr, \"Unhandled error %d at line %d\\n\", _err, _erl); exit(1);");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("");

        // Resume next dispatch
        writeln("_resume_next_dispatch:;");
        indent_level++;
        writeln("switch(_resume_next_id) {");
        indent_level++;
        for (int i = 0; i < error_check_counter; i++) {
            writeln("case " + std::to_string(i) + ": goto _rn_" + std::to_string(i) + ";");
        }
        writeln("default: break;");
        indent_level--;
        writeln("}");
        indent_level--;
        writeln("");
        writeln("_skip_error_dispatch:;");
    }

    // Audio cleanup for audio-only programs (no graphics)
    if (needs_sdl_mixer && !needs_sdl) {
        writeln("// Clean up SDL audio");
        writeln("for (int _si = 1; _si < MAX_SOUNDS; _si++) {");
        indent_level++;
        writeln("if (_sounds[_si]) { Mix_FreeChunk(_sounds[_si]); _sounds[_si] = NULL; }");
        indent_level--;
        writeln("}");
        writeln("if (_mixer_initialized) Mix_CloseAudio();");
        writeln("SDL_Quit();");
    }

    writeln("return 0;");

    indent_level--;
    writeln("}");

    std::cout << "Code generation complete." << std::endl;
}