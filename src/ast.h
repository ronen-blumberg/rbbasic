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
    
    // Sound
    SOUND_STMT, PLAY_STMT,
    
    // System
    RANDOMIZE_STMT, BEEP_STMT, SLEEP_STMT,
    
    // Advanced Control Flow
    ON_GOTO_STMT, ON_GOSUB_STMT,
    
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
    
    PrintStmt() : ASTNode(NodeType::PRINT_STMT), file_number(0) {}
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
        std::vector<int> dimensions;
        bool is_string;
        
        ArrayDecl(const std::string& n) : name(n) {
            is_string = (!n.empty() && n.back() == '$');
        }
    };
    
    std::vector<ArrayDecl> arrays;  // Support multiple arrays: DIM A(10), B(5)
    
    // Legacy single-array interface (for compatibility)
    std::string name;
    std::vector<int> dimensions;
    bool is_string;
    
    DimStmt(const std::string& n) : ASTNode(NodeType::DIM_STMT), name(n) {
        is_string = (!n.empty() && n.back() == '$');
    }
    
    DimStmt() : ASTNode(NodeType::DIM_STMT), is_string(false) {}
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
    std::string filename;
    std::string mode;  // "INPUT", "OUTPUT", "APPEND"
    int file_number;
    
    OpenStmt() : ASTNode(NodeType::OPEN_STMT), file_number(0) {}
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

#endif
