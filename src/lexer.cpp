#include "lexer.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

Lexer::Lexer(const std::string& src) 
    : source(src), pos(0), line(1), column(1) {
    
    current_char = source.empty() ? '\0' : source[0];
    
    // Basic keywords
    keywords["PRINT"] = TokenType::PRINT;
    keywords["REM"] = TokenType::REM;
    keywords["LET"] = TokenType::LET;
    keywords["END"] = TokenType::END;
    
    // Control flow
    keywords["IF"] = TokenType::IF;
    keywords["THEN"] = TokenType::THEN;
    keywords["ELSE"] = TokenType::ELSE;
    keywords["ELSEIF"] = TokenType::ELSEIF;
    keywords["GOTO"] = TokenType::GOTO;
    keywords["FOR"] = TokenType::FOR;
    keywords["TO"] = TokenType::TO;
    keywords["STEP"] = TokenType::STEP;
    keywords["NEXT"] = TokenType::NEXT;
    keywords["WHILE"] = TokenType::WHILE;
    keywords["WEND"] = TokenType::WEND;
    keywords["DO"] = TokenType::DO;
    keywords["LOOP"] = TokenType::LOOP;
    keywords["UNTIL"] = TokenType::UNTIL;
    keywords["GOSUB"] = TokenType::GOSUB;
    keywords["RETURN"] = TokenType::RETURN;
    
    // Input/Output
    keywords["INPUT"] = TokenType::INPUT;
    // Note: "LINE INPUT" is handled specially in make_identifier(), not here
    
    // Data
    keywords["DATA"] = TokenType::DATA;
    keywords["READ"] = TokenType::READ;
    keywords["RESTORE"] = TokenType::RESTORE;
    keywords["DIM"] = TokenType::DIM;
    
    // Subroutines
    keywords["SUB"] = TokenType::SUB;
    keywords["FUNCTION"] = TokenType::FUNCTION;
    keywords["CALL"] = TokenType::CALL;
    
    // File I/O
    keywords["OPEN"] = TokenType::OPEN;
    keywords["CLOSE"] = TokenType::CLOSE;
    keywords["AS"] = TokenType::AS;
    keywords["OUTPUT"] = TokenType::OUTPUT_KW;
    keywords["APPEND"] = TokenType::APPEND;
    
    // Screen Control
    keywords["CLS"] = TokenType::CLS;
    keywords["LOCATE"] = TokenType::LOCATE;
    keywords["COLOR"] = TokenType::COLOR;
    keywords["KEY"] = TokenType::KEY;
    
    // Graphics Commands
    keywords["SCREEN"] = TokenType::SCREEN;
    keywords["PSET"] = TokenType::PSET;
    keywords["LINE"] = TokenType::LINE;
    keywords["CIRCLE"] = TokenType::CIRCLE;
    keywords["PAINT"] = TokenType::PAINT;
    keywords["POINT"] = TokenType::POINT;
    
    // Sound Commands
    keywords["SOUND"] = TokenType::SOUND;
    keywords["PLAY"] = TokenType::PLAY_KW;
    
    // System Functions
    keywords["RANDOMIZE"] = TokenType::RANDOMIZE;
    keywords["BEEP"] = TokenType::BEEP;
    keywords["SLEEP"] = TokenType::SLEEP;
    
    // Advanced Control Flow
    keywords["ON"] = TokenType::ON;
    
    // User Functions
    keywords["DEF"] = TokenType::DEF;
    keywords["FN"] = TokenType::FN;
    
    // Logical operators
    keywords["AND"] = TokenType::AND;
    keywords["OR"] = TokenType::OR;
    keywords["NOT"] = TokenType::NOT;
    keywords["MOD"] = TokenType::MOD;
}

void Lexer::advance() {
    if (current_char == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    
    pos++;
    current_char = (pos < source.length()) ? source[pos] : '\0';
}

char Lexer::peek(int offset) const {
    size_t peek_pos = pos + offset;
    return (peek_pos < source.length()) ? source[peek_pos] : '\0';
}

void Lexer::skip_whitespace() {
    while (current_char == ' ' || current_char == '\t' || current_char == '\r') {
        advance();
    }
}

Token Lexer::make_token(TokenType type, const std::string& value) {
    return Token(type, value, line, column);
}

void Lexer::error(const std::string& message) {
    std::string error_msg = "Lexer error at line " + std::to_string(line) + 
                           ", column " + std::to_string(column) + ": " + message;
    throw std::runtime_error(error_msg);
}

Token Lexer::make_number() {
    std::string num_str;
    int start_col = column;
    bool has_dot = false;
    
    while (std::isdigit(current_char) || current_char == '.') {
        if (current_char == '.') {
            if (has_dot) {
                error("Invalid number format: multiple decimal points");
            }
            has_dot = true;
        }
        num_str += current_char;
        advance();
    }
    
    return Token(TokenType::NUMBER, num_str, line, start_col);
}

Token Lexer::make_string() {
    std::string str_value;
    int start_col = column;
    char quote_char = current_char;
    
    advance();
    
    while (current_char != quote_char && current_char != '\0' && current_char != '\n') {
        if (current_char == '\\' && peek() == quote_char) {
            advance();
            str_value += current_char;
            advance();
        } else {
            str_value += current_char;
            advance();
        }
    }
    
    if (current_char != quote_char) {
        error("Unterminated string literal");
    }
    
    advance();
    
    return Token(TokenType::STRING, str_value, line, start_col);
}

Token Lexer::make_identifier() {
    std::string id_str;
    int start_col = column;
    
    while (std::isalnum(current_char) || current_char == '_' || current_char == '$') {
        id_str += std::toupper(current_char);
        advance();
    }
    
    // Handle "END IF", "END SUB", "END FUNCTION", "LINE INPUT"
    if (id_str == "END" && (current_char == ' ' || current_char == '\t')) {
        size_t saved_pos = pos;
        int saved_line = line;
        int saved_col = column;
        char saved_char = current_char;
        
        skip_whitespace();
        std::string next_word;
        while (std::isalpha(current_char)) {
            next_word += std::toupper(current_char);
            advance();
        }
        
        if (next_word == "IF") {
            return Token(TokenType::END_IF, "END IF", line, start_col);
        } else if (next_word == "SUB") {
            return Token(TokenType::END_SUB, "END SUB", line, start_col);
        } else if (next_word == "FUNCTION") {
            return Token(TokenType::END_FUNCTION, "END FUNCTION", line, start_col);
        } else {
            // Restore position
            pos = saved_pos;
            line = saved_line;
            column = saved_col;
            current_char = saved_char;
        }
    }
    
    if (id_str == "LINE" && (current_char == ' ' || current_char == '\t')) {
        size_t saved_pos = pos;
        int saved_line = line;
        int saved_col = column;
        char saved_char = current_char;
        
        skip_whitespace();
        std::string next_word;
        while (std::isalpha(current_char)) {
            next_word += std::toupper(current_char);
            advance();
        }
        
        if (next_word == "INPUT") {
            return Token(TokenType::LINE_INPUT, "LINE INPUT", line, start_col);
        } else {
            // Restore position
            pos = saved_pos;
            line = saved_line;
            column = saved_col;
            current_char = saved_char;
        }
    }
    
    auto it = keywords.find(id_str);
    if (it != keywords.end()) {
        // Special handling for REM - consume rest of line as comment
        if (it->second == TokenType::REM) {
            std::string comment;
            // Skip any whitespace after REM
            while (current_char == ' ' || current_char == '\t') {
                advance();
            }
            // Consume everything until newline
            while (current_char != '\n' && current_char != '\0') {
                comment += current_char;
                advance();
            }
            return Token(TokenType::REM, comment, line, start_col);
        }
        return Token(it->second, id_str, line, start_col);
    }
    
    return Token(TokenType::IDENTIFIER, id_str, line, start_col);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (current_char != '\0') {
        int start_col = column;
        
        if (current_char == ' ' || current_char == '\t' || current_char == '\r') {
            skip_whitespace();
            continue;
        }
        
        if (current_char == '\n') {
            tokens.push_back(Token(TokenType::NEWLINE, "\\n", line, column));
            advance();
            continue;
        }
        
        if (std::isdigit(current_char)) {
            tokens.push_back(make_number());
            continue;
        }
        
        if (current_char == '"' || current_char == '\'') {
            tokens.push_back(make_string());
            continue;
        }
        
        if (std::isalpha(current_char) || current_char == '_') {
            tokens.push_back(make_identifier());
            continue;
        }
        
        switch (current_char) {
            case '+':
                tokens.push_back(Token(TokenType::PLUS, "+", line, start_col));
                advance();
                break;
            case '-':
                tokens.push_back(Token(TokenType::MINUS, "-", line, start_col));
                advance();
                break;
            case '*':
                tokens.push_back(Token(TokenType::MULTIPLY, "*", line, start_col));
                advance();
                break;
            case '/':
                tokens.push_back(Token(TokenType::DIVIDE, "/", line, start_col));
                advance();
                break;
            case '^':
                tokens.push_back(Token(TokenType::POWER, "^", line, start_col));
                advance();
                break;
            case '=':
                tokens.push_back(Token(TokenType::EQUALS, "=", line, start_col));
                advance();
                break;
            case '<':
                if (peek() == '=') {
                    tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", line, start_col));
                    advance();
                    advance();
                } else if (peek() == '>') {
                    tokens.push_back(Token(TokenType::NOT_EQUAL, "<>", line, start_col));
                    advance();
                    advance();
                } else {
                    tokens.push_back(Token(TokenType::LESS, "<", line, start_col));
                    advance();
                }
                break;
            case '>':
                if (peek() == '=') {
                    tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", line, start_col));
                    advance();
                    advance();
                } else {
                    tokens.push_back(Token(TokenType::GREATER, ">", line, start_col));
                    advance();
                }
                break;
            case '(':
                tokens.push_back(Token(TokenType::LPAREN, "(", line, start_col));
                advance();
                break;
            case ')':
                tokens.push_back(Token(TokenType::RPAREN, ")", line, start_col));
                advance();
                break;
            case ',':
                tokens.push_back(Token(TokenType::COMMA, ",", line, start_col));
                advance();
                break;
            case ';':
                tokens.push_back(Token(TokenType::SEMICOLON, ";", line, start_col));
                advance();
                break;
            case ':':
                tokens.push_back(Token(TokenType::COLON, ":", line, start_col));
                advance();
                break;
            case '#':
                tokens.push_back(Token(TokenType::HASH, "#", line, start_col));
                advance();
                break;
            case '$':
                tokens.push_back(Token(TokenType::DOLLAR, "$", line, start_col));
                advance();
                break;
            case '!':
                // Single-precision type suffix - just skip it for now
                advance();
                break;
            default:
                error(std::string("Unexpected character: '") + current_char + "'");
        }
    }
    
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}