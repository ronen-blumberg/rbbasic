#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

enum class NodeType {
    PROGRAM, STATEMENT,
    
    // Basic Statements
    PRINT_STMT, REM_STMT, LET_STMT, END_STMT,
    
    // Control Flow
    IF_STMT, GOTO_STMT, FOR_STMT, 
    WHILE_STMT, DO_LOOP_STMT, GOSUB_STMT, RETURN_STMT,
    
    // Input/Output
    INPUT_STMT, LINE_INPUT_STMT,
    
    // Data
    DATA_STMT, READ_STMT, RESTORE_STMT, DIM_STMT,
    
    // Subroutines
    SUB_DEF, FUNCTION_DEF, CALL_STMT,
    
    // File I/O
    OPEN_STMT, CLOSE_STMT, FILE_INPUT_STMT, FILE_PRINT_STMT,
    
    // Screen Control
    CLS_STMT, LOCATE_STMT, COLOR_STMT, KEY_STMT,
    
    // Graphics
    SCREEN_STMT, PSET_STMT, LINE_STMT, CIRCLE_STMT, PAINT_STMT,
    DRAW_STMT, PALETTE_STMT, VIEW_STMT, WINDOW_STMT, PCOPY_STMT,
    GET_GFX_STMT, PUT_GFX_STMT,
    
    // Sound
    SOUND_STMT, PLAY_STMT,
    
    // System
    RANDOMIZE_STMT, BEEP_STMT, SLEEP_STMT,
    
    // Advanced Control Flow
    ON_GOTO_STMT, ON_GOSUB_STMT,
    SELECT_CASE_STMT, EXIT_STMT,

    // Declarations
    CONST_STMT, SWAP_STMT, DECLARE_STMT,
    REDIM_STMT, ERASE_STMT,
    TYPE_DEF_STMT,

    // Phase 2: Error Handling & File I/O
    ON_ERROR_STMT, RESUME_STMT,
    WRITE_STMT, SEEK_STMT, GET_FILE_STMT, PUT_FILE_STMT,
    MID_ASSIGN_STMT,

    // Phase 4: System Functions
    SHELL_STMT, POKE_STMT, DEF_SEG_STMT,

    // Phase 6: QB64 Extensions
    DISPLAY_STMT, LIMIT_STMT, FREEIMAGE_STMT, PUTIMAGE_STMT,
    PRINTSTRING_STMT, SNDPLAY_STMT, SNDSTOP_STMT, SNDVOL_STMT,

    // User Functions
    DEF_FN_STMT,
    
    // Expressions
    BINARY_OP, UNARY_OP, FUNCTION_CALL,
    NUMBER, STRING, VARIABLE, ARRAY_ACCESS
};

class ASTNode {
public:
    NodeType type;
    int line_number;
    
    ASTNode(NodeType t) : type(t), line_number(0) {}
    virtual ~ASTNode() = default;
};

// ============== LITERALS ==============

class NumberNode : public ASTNode {
public:
    double value;
    NumberNode(double val) : ASTNode(NodeType::NUMBER), value(val) {}
};

class StringNode : public ASTNode {
public:
    std::string value;
    StringNode(const std::string& val) : ASTNode(NodeType::STRING), value(val) {}
};

class VariableNode : public ASTNode {
public:
    std::string name;
    bool is_string;  // true if ends with $
    VariableNode(const std::string& n) : ASTNode(NodeType::VARIABLE), name(n) {
        is_string = (!n.empty() && n.back() == '$');
    }
};

class ArrayAccessNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> indices;
    bool is_string;
    
    ArrayAccessNode(const std::string& n) : ASTNode(NodeType::ARRAY_ACCESS), name(n) {
        is_string = (!n.empty() && n.back() == '$');
    }
};

// ============== OPERATORS ==============

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(const std::string& o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : ASTNode(NodeType::BINARY_OP), op(o), left(std::move(l)), right(std::move(r)) {}
};

class UnaryOpNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> operand;
    
    UnaryOpNode(const std::string& o, std::unique_ptr<ASTNode> opnd)
        : ASTNode(NodeType::UNARY_OP), op(o), operand(std::move(opnd)) {}
};

class FunctionCallNode : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    FunctionCallNode(const std::string& n) : ASTNode(NodeType::FUNCTION_CALL), name(n) {}
};

// ============== BASIC STATEMENTS ==============

class PrintStmt : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> expressions;
    std::vector<bool> use_semicolon;  // track if semicolon was used (no newline)
    int file_number;  // 0 for console, >0 for file
    std::unique_ptr<ASTNode> using_format;  // non-null for PRINT USING
    bool has_trailing_separator;  // true if PRINT ends with ; or ,

    PrintStmt() : ASTNode(NodeType::PRINT_STMT), file_number(0), has_trailing_separator(false) {}
};

class RemStmt : public ASTNode {
public:
    std::string comment;
    RemStmt(const std::string& c) : ASTNode(NodeType::REM_STMT), comment(c) {}
};

class LetStmt : public ASTNode {
public:
    std::string variable;
    std::unique_ptr<ASTNode> array_access;  // if assigning to array
    std::unique_ptr<ASTNode> expression;
    
    LetStmt(const std::string& v, std::unique_ptr<ASTNode> e)
        : ASTNode(NodeType::LET_STMT), variable(v), expression(std::move(e)) {}
};

class EndStmt : public ASTNode {
public:
    EndStmt() : ASTNode(NodeType::END_STMT) {}
};

// ============== CONTROL FLOW ==============

class IfStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<class Statement>> then_body;
    std::vector<std::unique_ptr<class Statement>> else_body;
    int target_line;  // for old-style IF THEN linenum
    bool is_block;    // true for IF...END IF, false for IF THEN linenum
    
    IfStmt(std::unique_ptr<ASTNode> c)
        : ASTNode(NodeType::IF_STMT), condition(std::move(c)), target_line(0), is_block(false) {}
};

class GotoStmt : public ASTNode {
public:
    int target_line;
    GotoStmt(int t) : ASTNode(NodeType::GOTO_STMT), target_line(t) {}
};

class ForStmt : public ASTNode {
public:
    std::string variable;
    std::unique_ptr<ASTNode> start;
    std::unique_ptr<ASTNode> end;
    std::unique_ptr<ASTNode> step;
    std::vector<std::unique_ptr<class Statement>> body;
    int next_line_num;  // Line number where NEXT appears
    
    ForStmt(const std::string& v, std::unique_ptr<ASTNode> s, std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> st)
        : ASTNode(NodeType::FOR_STMT), variable(v), start(std::move(s)), end(std::move(e)), step(std::move(st)), next_line_num(0) {}
};

class WhileStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<class Statement>> body;
    
    WhileStmt(std::unique_ptr<ASTNode> c) : ASTNode(NodeType::WHILE_STMT), condition(std::move(c)) {}
};

class DoLoopStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<class Statement>> body;
    bool check_at_start;  // DO WHILE vs DO...LOOP WHILE
    bool is_until;        // UNTIL vs WHILE
    
    DoLoopStmt() : ASTNode(NodeType::DO_LOOP_STMT), check_at_start(false), is_until(false) {}
};

class GosubStmt : public ASTNode {
public:
    int target_line;
    GosubStmt(int t) : ASTNode(NodeType::GOSUB_STMT), target_line(t) {}
};

class ReturnStmt : public ASTNode {
public:
    ReturnStmt() : ASTNode(NodeType::RETURN_STMT) {}
};

// ============== INPUT/OUTPUT ==============

class InputStmt : public ASTNode {
public:
    std::string prompt;
    std::vector<std::string> variables;
    bool is_line_input;
    int file_number;  // 0 for console, >0 for file
    
    InputStmt() : ASTNode(NodeType::INPUT_STMT), is_line_input(false), file_number(0) {}
};

class LineInputStmt : public ASTNode {
public:
    std::string prompt;
    std::string variable;
    int file_number;  // 0 for console, >0 for file
    
    LineInputStmt() : ASTNode(NodeType::LINE_INPUT_STMT), file_number(0) {}
};

// ============== DATA ==============

class DataStmt : public ASTNode {
public:
    std::vector<std::string> values;
    DataStmt() : ASTNode(NodeType::DATA_STMT) {}
};

class ReadStmt : public ASTNode {
public:
    struct ReadTarget {
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> indices;  // Empty if not array
        bool is_array() const { return !indices.empty(); }
    };
    std::vector<ReadTarget> variables;
    ReadStmt() : ASTNode(NodeType::READ_STMT) {}
};

class RestoreStmt : public ASTNode {
public:
    int line_number;  // 0 means restore to beginning
    RestoreStmt(int ln = 0) : ASTNode(NodeType::RESTORE_STMT), line_number(ln) {}
};

class DimStmt : public ASTNode {
public:
    struct ArrayDecl {
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> dimension_exprs;  // Changed from int to expressions
        bool is_string;
        std::string type_name;  // For DIM var AS TypeName

        ArrayDecl(const std::string& n) : name(n) {
            is_string = (!n.empty() && n.back() == '$');
        }
    };
    
    std::vector<ArrayDecl> arrays;  // Support multiple arrays: DIM A(10), B(5)
    
    DimStmt() : ASTNode(NodeType::DIM_STMT) {}
};

// ============== SUBROUTINES ==============

class SubDef : public ASTNode {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<class Statement>> body;
    
    SubDef(const std::string& n) : ASTNode(NodeType::SUB_DEF), name(n) {}
};

class FunctionDef : public ASTNode {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<class Statement>> body;
    std::unique_ptr<ASTNode> return_value;
    
    FunctionDef(const std::string& n) : ASTNode(NodeType::FUNCTION_DEF), name(n) {}
};

class CallStmt : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    CallStmt(const std::string& n) : ASTNode(NodeType::CALL_STMT), name(n) {}
};

// ============== FILE I/O ==============

class OpenStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> filename;  // Changed from string to expression
    std::string mode;  // "INPUT", "OUTPUT", "APPEND", "BINARY", "RANDOM"
    int file_number;
    int record_length;  // for RANDOM mode (LEN = n)

    OpenStmt() : ASTNode(NodeType::OPEN_STMT), file_number(0), record_length(128) {}
};

class CloseStmt : public ASTNode {
public:
    std::vector<int> file_numbers;
    CloseStmt() : ASTNode(NodeType::CLOSE_STMT) {}
};

// ============== SCREEN CONTROL ==============

class ClsStmt : public ASTNode {
public:
    int mode;  // 0 = all, 1 = graphics, 2 = text (optional in GW-BASIC)
    ClsStmt(int m = 0) : ASTNode(NodeType::CLS_STMT), mode(m) {}
};

class LocateStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> row;
    std::unique_ptr<ASTNode> col;
    std::unique_ptr<ASTNode> cursor;  // optional cursor on/off
    
    LocateStmt() : ASTNode(NodeType::LOCATE_STMT) {}
};

class ColorStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> foreground;
    std::unique_ptr<ASTNode> background;
    std::unique_ptr<ASTNode> border;  // optional
    
    ColorStmt() : ASTNode(NodeType::COLOR_STMT) {}
};

class KeyStmt : public ASTNode {
public:
    std::string mode;  // "ON" or "OFF"
    KeyStmt() : ASTNode(NodeType::KEY_STMT) {}
};

// ============== SYSTEM ==============

class RandomizeStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> seed;  // optional
    RandomizeStmt() : ASTNode(NodeType::RANDOMIZE_STMT) {}
};

class BeepStmt : public ASTNode {
public:
    BeepStmt() : ASTNode(NodeType::BEEP_STMT) {}
};

class SleepStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> duration;  // Duration in seconds
    SleepStmt() : ASTNode(NodeType::SLEEP_STMT) {}
};

// ============== ADVANCED CONTROL FLOW ==============

class OnGotoStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    std::vector<int> target_lines;
    
    OnGotoStmt() : ASTNode(NodeType::ON_GOTO_STMT) {}
};

class OnGosubStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    std::vector<int> target_lines;
    
    OnGosubStmt() : ASTNode(NodeType::ON_GOSUB_STMT) {}
};

// ============== USER FUNCTIONS ==============

class DefFnStmt : public ASTNode {
public:
    std::string name;  // e.g., "FNarea" from "DEF FNarea(x)"
    std::vector<std::string> parameters;
    std::unique_ptr<ASTNode> expression;
    
    DefFnStmt(const std::string& n) : ASTNode(NodeType::DEF_FN_STMT), name(n) {}
};

// ============== SELECT CASE ==============

class SelectCaseStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> test_expression;
    struct CaseClause {
        struct CaseTest {
            enum Kind { VALUE, RANGE, IS_COMPARE };
            Kind kind;
            std::unique_ptr<ASTNode> value;        // VALUE: single value, IS_COMPARE: rhs
            std::unique_ptr<ASTNode> range_end;     // RANGE: end of TO range
            std::string is_op;                       // IS_COMPARE: operator string (=, <, >, etc.)
        };
        std::vector<CaseTest> tests;
        bool is_else;
        std::vector<std::unique_ptr<Statement>> body;
        CaseClause() : is_else(false) {}
    };
    std::vector<CaseClause> cases;
    SelectCaseStmt() : ASTNode(NodeType::SELECT_CASE_STMT) {}
};

// ============== EXIT ==============

class ExitStmt : public ASTNode {
public:
    std::string exit_type;  // "FOR", "DO", "WHILE", "SUB", "FUNCTION"
    ExitStmt(const std::string& t) : ASTNode(NodeType::EXIT_STMT), exit_type(t) {}
};

// ============== CONST ==============

class ConstStmt : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> value;
    ConstStmt(const std::string& n) : ASTNode(NodeType::CONST_STMT), name(n) {}
};

// ============== SWAP ==============

class SwapStmt : public ASTNode {
public:
    std::string var1;
    std::string var2;
    std::unique_ptr<ASTNode> array1;  // if var1 is array access
    std::unique_ptr<ASTNode> array2;  // if var2 is array access
    SwapStmt() : ASTNode(NodeType::SWAP_STMT) {}
};

// ============== DECLARE ==============

class DeclareStmt : public ASTNode {
public:
    std::string kind;  // "SUB" or "FUNCTION"
    std::string name;
    DeclareStmt() : ASTNode(NodeType::DECLARE_STMT) {}
};

// ============== REDIM ==============

class RedimStmt : public ASTNode {
public:
    struct ArrayDecl {
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> dimension_exprs;
        bool is_string;
        bool preserve;

        ArrayDecl(const std::string& n, bool p = false) : name(n), preserve(p) {
            is_string = (!n.empty() && n.back() == '$');
        }
    };

    std::vector<ArrayDecl> arrays;
    bool preserve;

    RedimStmt(bool p = false) : ASTNode(NodeType::REDIM_STMT), preserve(p) {}
};

// ============== ERASE ==============

class EraseStmt : public ASTNode {
public:
    std::vector<std::string> array_names;
    EraseStmt() : ASTNode(NodeType::ERASE_STMT) {}
};

// ============== ON ERROR ==============

class OnErrorStmt : public ASTNode {
public:
    int target_line;  // 0 means ON ERROR GOTO 0 (disable handler)
    OnErrorStmt(int t) : ASTNode(NodeType::ON_ERROR_STMT), target_line(t) {}
};

class ResumeStmt : public ASTNode {
public:
    enum ResumeType { RETRY, NEXT, LINE };
    ResumeType resume_type;
    int target_line;  // for RESUME line
    ResumeStmt() : ASTNode(NodeType::RESUME_STMT), resume_type(NEXT), target_line(0) {}
};

// ============== WRITE # ==============

class WriteStmt : public ASTNode {
public:
    int file_number;
    std::vector<std::unique_ptr<ASTNode>> expressions;
    WriteStmt() : ASTNode(NodeType::WRITE_STMT), file_number(0) {}
};

// ============== SEEK # ==============

class SeekStmt : public ASTNode {
public:
    int file_number;
    std::unique_ptr<ASTNode> position;
    SeekStmt() : ASTNode(NodeType::SEEK_STMT), file_number(0) {}
};

// ============== GET # / PUT # (File) ==============

class GetFileStmt : public ASTNode {
public:
    int file_number;
    std::unique_ptr<ASTNode> record_number;  // optional
    std::string variable;  // optional target variable
    GetFileStmt() : ASTNode(NodeType::GET_FILE_STMT), file_number(0) {}
};

class PutFileStmt : public ASTNode {
public:
    int file_number;
    std::unique_ptr<ASTNode> record_number;  // optional
    std::string variable;  // optional source variable
    PutFileStmt() : ASTNode(NodeType::PUT_FILE_STMT), file_number(0) {}
};

// ============== MID$ ASSIGNMENT ==============

class MidAssignStmt : public ASTNode {
public:
    std::string variable;  // target string variable
    std::unique_ptr<ASTNode> start;
    std::unique_ptr<ASTNode> length;  // optional
    std::unique_ptr<ASTNode> replacement;
    MidAssignStmt() : ASTNode(NodeType::MID_ASSIGN_STMT) {}
};

// ============== GRAPHICS ==============

class ScreenStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> mode;  // Screen mode (0-13 in GW-BASIC)
    
    ScreenStmt() : ASTNode(NodeType::SCREEN_STMT) {}
};

class PsetStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x;
    std::unique_ptr<ASTNode> y;
    std::unique_ptr<ASTNode> color;  // optional
    
    PsetStmt() : ASTNode(NodeType::PSET_STMT) {}
};

class LineStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x1;
    std::unique_ptr<ASTNode> y1;
    std::unique_ptr<ASTNode> x2;
    std::unique_ptr<ASTNode> y2;
    std::unique_ptr<ASTNode> color;
    bool is_box;       // true if B flag
    bool is_filled;    // true if BF flag
    
    LineStmt() : ASTNode(NodeType::LINE_STMT), is_box(false), is_filled(false) {}
};

class CircleStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x;
    std::unique_ptr<ASTNode> y;
    std::unique_ptr<ASTNode> radius;
    std::unique_ptr<ASTNode> color;      // optional
    std::unique_ptr<ASTNode> start_angle; // optional (radians)
    std::unique_ptr<ASTNode> end_angle;   // optional (radians)
    std::unique_ptr<ASTNode> aspect;      // optional (aspect ratio)
    
    CircleStmt() : ASTNode(NodeType::CIRCLE_STMT) {}
};

class PaintStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x;
    std::unique_ptr<ASTNode> y;
    std::unique_ptr<ASTNode> paint_color;
    std::unique_ptr<ASTNode> border_color;  // optional
    
    PaintStmt() : ASTNode(NodeType::PAINT_STMT) {}
};

// ============== DRAW (GML turtle graphics) ==============

class DrawStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> command_string;
    DrawStmt() : ASTNode(NodeType::DRAW_STMT) {}
};

// ============== PALETTE ==============

class PaletteStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> attribute;  // nullable for reset
    std::unique_ptr<ASTNode> color;      // nullable for reset
    PaletteStmt() : ASTNode(NodeType::PALETTE_STMT) {}
};

// ============== VIEW (clipping viewport) ==============

class ViewStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x1;  // nullable for reset
    std::unique_ptr<ASTNode> y1;
    std::unique_ptr<ASTNode> x2;
    std::unique_ptr<ASTNode> y2;
    ViewStmt() : ASTNode(NodeType::VIEW_STMT) {}
};

// ============== WINDOW (logical coordinates) ==============

class WindowStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x1;  // nullable for reset
    std::unique_ptr<ASTNode> y1;
    std::unique_ptr<ASTNode> x2;
    std::unique_ptr<ASTNode> y2;
    WindowStmt() : ASTNode(NodeType::WINDOW_STMT) {}
};

// ============== PCOPY ==============

class PcopyStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> src_page;
    std::unique_ptr<ASTNode> dest_page;
    PcopyStmt() : ASTNode(NodeType::PCOPY_STMT) {}
};

// ============== GET/PUT Graphics (Sprites) ==============

class GetGfxStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x1;
    std::unique_ptr<ASTNode> y1;
    std::unique_ptr<ASTNode> x2;
    std::unique_ptr<ASTNode> y2;
    std::string array_name;
    GetGfxStmt() : ASTNode(NodeType::GET_GFX_STMT) {}
};

class PutGfxStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x;
    std::unique_ptr<ASTNode> y;
    std::string array_name;
    std::string action;  // "PSET", "PRESET", "AND", "OR", "XOR"
    PutGfxStmt() : ASTNode(NodeType::PUT_GFX_STMT), action("XOR") {}
};

// ============== SOUND ==============

class SoundStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> frequency;  // Hz
    std::unique_ptr<ASTNode> duration;   // clock ticks (18.2 per second)
    
    SoundStmt() : ASTNode(NodeType::SOUND_STMT) {}
};

class PlayStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> music_string;  // MML format string
    
    PlayStmt() : ASTNode(NodeType::PLAY_STMT) {}
};

// ============== SHELL ==============

class ShellStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> command;  // nullable - SHELL alone opens shell
    ShellStmt() : ASTNode(NodeType::SHELL_STMT) {}
};

// ============== POKE ==============

class PokeStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> address;
    std::unique_ptr<ASTNode> value;
    PokeStmt() : ASTNode(NodeType::POKE_STMT) {}
};

// ============== DEF SEG ==============

class DefSegStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> segment;  // nullable - DEF SEG alone resets to 0
    DefSegStmt() : ASTNode(NodeType::DEF_SEG_STMT) {}
};

// ============== QB64 EXTENSIONS ==============

class DisplayStmt : public ASTNode {
public:
    DisplayStmt() : ASTNode(NodeType::DISPLAY_STMT) {}
};

class LimitStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> fps;
    LimitStmt() : ASTNode(NodeType::LIMIT_STMT) {}
};

class FreeImageStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> handle;
    FreeImageStmt() : ASTNode(NodeType::FREEIMAGE_STMT) {}
};

class PutImageStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> dx;
    std::unique_ptr<ASTNode> dy;
    std::unique_ptr<ASTNode> source_handle;
    std::unique_ptr<ASTNode> dest_handle;  // optional, default = screen
    PutImageStmt() : ASTNode(NodeType::PUTIMAGE_STMT) {}
};

class PrintStringStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> x;
    std::unique_ptr<ASTNode> y;
    std::unique_ptr<ASTNode> text;
    PrintStringStmt() : ASTNode(NodeType::PRINTSTRING_STMT) {}
};

class SndPlayStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> handle;
    SndPlayStmt() : ASTNode(NodeType::SNDPLAY_STMT) {}
};

class SndStopStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> handle;
    SndStopStmt() : ASTNode(NodeType::SNDSTOP_STMT) {}
};

class SndVolStmt : public ASTNode {
public:
    std::unique_ptr<ASTNode> handle;
    std::unique_ptr<ASTNode> volume;
    SndVolStmt() : ASTNode(NodeType::SNDVOL_STMT) {}
};

// ============== PROGRAM STRUCTURE ==============

class Statement : public ASTNode {
public:
    int line_num;
    std::unique_ptr<ASTNode> stmt;
    
    Statement(int ln, std::unique_ptr<ASTNode> s)
        : ASTNode(NodeType::STATEMENT), line_num(ln), stmt(std::move(s)) {}
};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    std::vector<std::unique_ptr<SubDef>> subs;
    std::vector<std::unique_ptr<FunctionDef>> functions;
    std::vector<std::unique_ptr<DefFnStmt>> def_fns;
    
    Program() : ASTNode(NodeType::PROGRAM) {}
};

// TYPE...END TYPE - User-Defined Type
class TypeField {
public:
    std::string name;
    std::string type_name;  // "INTEGER", "LONG", "SINGLE", "DOUBLE", "STRING"
    int string_length;      // For STRING * n, default 0 means variable-length (use 256)

    TypeField(const std::string& n, const std::string& t, int sl = 0)
        : name(n), type_name(t), string_length(sl) {}
};

class TypeDefStmt : public ASTNode {
public:
    std::string type_name;
    std::vector<TypeField> fields;

    TypeDefStmt() : ASTNode(NodeType::TYPE_DEF_STMT) {}
};

#endif
