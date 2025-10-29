#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <string>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>

class CodeGenerator {
private:
    std::ofstream output;
    std::unordered_set<std::string> numeric_variables;
    std::unordered_set<std::string> string_variables;
    std::unordered_map<std::string, std::vector<int>> arrays;  // name -> dimensions
    std::unordered_map<std::string, std::vector<int>> string_arrays;
    std::unordered_map<int, int> line_labels;
    std::vector<std::string> data_values;
    int next_label;
    int indent_level;
    bool needs_math_h;
    bool needs_string_h;
    bool needs_time_h;
    bool needs_sdl;
    bool needs_sound;
    
    void write(const std::string& code);
    void writeln(const std::string& code);
    std::string get_indent() const;
    
    void generate_headers();
    void generate_variable_declarations();
    void generate_array_declarations();
    void generate_data_initialization();
    void generate_builtin_functions();
    void generate_def_fns(const Program* program);
    void generate_user_subs_and_functions(const Program* program);
    
    void generate_statement(const Statement* stmt);
    void generate_print_stmt(const PrintStmt* stmt);
    void generate_let_stmt(const LetStmt* stmt);
    void generate_if_stmt(const IfStmt* stmt);
    void generate_goto_stmt(const GotoStmt* stmt);
    void generate_end_stmt(const EndStmt* stmt);
    void generate_for_stmt(const ForStmt* stmt);
    void generate_while_stmt(const WhileStmt* stmt);
    void generate_do_loop_stmt(const DoLoopStmt* stmt);
    void generate_gosub_stmt(const GosubStmt* stmt);
    void generate_return_stmt(const ReturnStmt* stmt);
    void generate_input_stmt(const InputStmt* stmt);
    void generate_line_input_stmt(const LineInputStmt* stmt);
    void generate_read_stmt(const ReadStmt* stmt);
    void generate_restore_stmt(const RestoreStmt* stmt);
    void generate_dim_stmt(const DimStmt* stmt);
    void generate_open_stmt(const OpenStmt* stmt);
    void generate_close_stmt(const CloseStmt* stmt);
    void generate_call_stmt(const CallStmt* stmt);
    void generate_cls_stmt(const ClsStmt* stmt);
    void generate_locate_stmt(const LocateStmt* stmt);
    void generate_color_stmt(const ColorStmt* stmt);
    void generate_key_stmt(const KeyStmt* stmt);
    void generate_randomize_stmt(const RandomizeStmt* stmt);
    void generate_beep_stmt(const BeepStmt* stmt);
    void generate_sleep_stmt(const SleepStmt* stmt);
    void generate_on_goto_stmt(const OnGotoStmt* stmt);
    void generate_on_gosub_stmt(const OnGosubStmt* stmt);
    void generate_screen_stmt(const ScreenStmt* stmt);
    void generate_pset_stmt(const PsetStmt* stmt);
    void generate_line_stmt(const LineStmt* stmt);
    void generate_circle_stmt(const CircleStmt* stmt);
    void generate_paint_stmt(const PaintStmt* stmt);
    void generate_sound_stmt(const SoundStmt* stmt);
    void generate_play_stmt(const PlayStmt* stmt);
    
    void generate_expression(const ASTNode* expr, bool is_string_context = false);
    std::string expression_to_string(const ASTNode* expr);  // Helper to get expression as string
    void generate_function_call(const FunctionCallNode* func);
    bool is_string_expr(const ASTNode* expr);
    
    void collect_variables(const Program* program);
    void collect_line_labels(const Program* program);
    void collect_data_values(const Program* program);
    void collect_expr_variables(const ASTNode* expr);
    void collect_stmt_body_variables(const std::vector<std::unique_ptr<Statement>>& body);
    void detect_graphics_sound(const Program* program);
    
public:
    CodeGenerator(const std::string& filename);
    ~CodeGenerator();
    void generate(const Program* program);
    bool uses_graphics() const { return needs_sdl; }
};

#endif
