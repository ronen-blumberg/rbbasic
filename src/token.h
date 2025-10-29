#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Control Flow
    PRINT, REM, LET, IF, THEN, ELSE, ELSEIF, END_IF, GOTO, END,
    FOR, TO, STEP, NEXT,
    WHILE, WEND,
    DO, LOOP, UNTIL,
    GOSUB, RETURN,
    
    // Input/Output
    INPUT, LINE_INPUT,
    
    // Data
    DATA, READ, RESTORE,
    DIM,
    
    // Subroutines
    SUB, END_SUB, FUNCTION, END_FUNCTION, CALL,
    
    // File I/O
    OPEN, CLOSE, AS, OUTPUT_KW, APPEND, INPUT_KW,
    
    // Screen Control
    CLS, LOCATE, COLOR, KEY,
    
    // Graphics Commands
    SCREEN, PSET, LINE, CIRCLE, PAINT, POINT,
    
    // Sound Commands
    SOUND, PLAY_KW,
    
    // System Functions
    RANDOMIZE, BEEP, SLEEP,
    
    // Advanced Control Flow
    ON,
    
    // User Functions
    DEF, FN,
    
    // String Functions (will be recognized as identifiers but handled specially)
    // LEFT$, RIGHT$, MID$, LEN, INSTR, CHR$, ASC, STR$, VAL, UCASE$, LCASE$
    // INKEY$, DATE$, TIME$, HEX$, OCT$
    
    // Math Functions (will be recognized as identifiers but handled specially)  
    // SIN, COS, TAN, ATN, SQR, ABS, INT, SGN, RND, LOG, EXP, CINT, FIX
    // TIMER, LBOUND, UBOUND
    
    // Literals and Identifiers
    NUMBER, STRING, IDENTIFIER,
    
    // Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, MOD, POWER,
    EQUALS, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, NOT_EQUAL,
    AND, OR, NOT,
    
    // Punctuation
    LPAREN, RPAREN, COMMA, SEMICOLON, COLON,
    HASH, DOLLAR,
    
    // Special
    NEWLINE, END_OF_FILE, INVALID
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token() : type(TokenType::INVALID), line(0), column(0) {}
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

const char* token_type_to_string(TokenType type);

#endif
