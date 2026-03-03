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
    
    // Array metadata - stores pointers to dimension expressions for runtime evaluation
    struct ArrayInfo {
        std::vector<const ASTNode*> dimension_exprs;
        bool is_dynamic;  // true if any dimension is not a constant
    };
    std::unordered_map<std::string, ArrayInfo> arrays;  // name -> array info
    std::unordered_map<std::string, ArrayInfo> string_arrays;
    
    std::unordered_map<int, int> line_labels;
    std::vector<std::string> data_values;
    std::unordered_map<std::string, std::string> constants;  // name -> C expression string

    // User-defined types
    struct TypeInfo {
        std::string name;
        std::vector<TypeField> fields;
    };
    std::unordered_map<std::string, TypeInfo> user_types;  // type_name -> TypeInfo
    std::unordered_map<std::string, std::string> typed_variables;  // var_name -> type_name
    std::unordered_set<int> on_error_targets;  // BASIC line numbers used as ON ERROR GOTO targets
    int error_check_counter;  // counter for resume-next labels
    bool has_error_handling;  // true if ON ERROR GOTO is used
    int current_basic_line;  // current BASIC line being generated
    int next_label;
    int indent_level;
    bool needs_math_h;
    bool needs_string_h;
    bool needs_time_h;
    bool needs_sdl;
    bool needs_sound;
    bool has_display_stmt;   // true if _DISPLAY is used (disables auto-present)
    bool needs_sdl_image;    // true if _LOADIMAGE is used
    bool needs_sdl_mixer;    // true if _SNDOPEN etc. are used
    bool needs_console;      // true if INPUT/LINE INPUT/PRINT used (for console allocation in graphics mode)
    
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
    void generate_select_case_stmt(const SelectCaseStmt* stmt);
    void generate_exit_stmt(const ExitStmt* stmt);
    void generate_const_stmt(const ConstStmt* stmt);
    void generate_swap_stmt(const SwapStmt* stmt);
    void generate_redim_stmt(const RedimStmt* stmt);
    void generate_erase_stmt(const EraseStmt* stmt);
    void generate_on_error_stmt(const OnErrorStmt* stmt);
    void generate_resume_stmt(const ResumeStmt* stmt);
    void generate_write_stmt(const WriteStmt* stmt);
    void generate_seek_stmt(const SeekStmt* stmt);
    void generate_get_file_stmt(const GetFileStmt* stmt);
    void generate_put_file_stmt(const PutFileStmt* stmt);
    void generate_mid_assign_stmt(const MidAssignStmt* stmt);
    void generate_print_using(const PrintStmt* stmt);
    void generate_type_def_stmt(const TypeDefStmt* stmt);
    void generate_draw_stmt(const DrawStmt* stmt);
    void generate_palette_stmt(const PaletteStmt* stmt);
    void generate_view_stmt(const ViewStmt* stmt);
    void generate_window_stmt(const WindowStmt* stmt);
    void generate_pcopy_stmt(const PcopyStmt* stmt);
    void generate_get_gfx_stmt(const GetGfxStmt* stmt);
    void generate_put_gfx_stmt(const PutGfxStmt* stmt);
    void generate_shell_stmt(const ShellStmt* stmt);
    void generate_poke_stmt(const PokeStmt* stmt);
    void generate_def_seg_stmt(const DefSegStmt* stmt);

    // Phase 6: QB64 Extensions
    void generate_display_stmt(const DisplayStmt* stmt);
    void generate_limit_stmt(const LimitStmt* stmt);
    void generate_freeimage_stmt(const FreeImageStmt* stmt);
    void generate_putimage_stmt(const PutImageStmt* stmt);
    void generate_printstring_stmt(const PrintStringStmt* stmt);
    void generate_sndplay_stmt(const SndPlayStmt* stmt);
    void generate_sndstop_stmt(const SndStopStmt* stmt);
    void generate_sndvol_stmt(const SndVolStmt* stmt);

    void generate_expression(const ASTNode* expr, bool is_string_context = false);
    std::string expression_to_string(const ASTNode* expr);  // Helper to get expression as string
    void generate_function_call(const FunctionCallNode* func);
    bool is_string_expr(const ASTNode* expr);
    std::string escape_string(const std::string& str);  // Helper to escape strings for C
    std::string sanitize_varname(const std::string& name);  // Sanitize variable names to avoid conflicts
    
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
    bool uses_sdl_image() const { return needs_sdl_image; }
    bool uses_sdl_mixer() const { return needs_sdl_mixer; }
};

#endif
