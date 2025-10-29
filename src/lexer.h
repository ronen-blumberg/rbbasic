#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <vector>
#include <string>
#include <unordered_map>

class Lexer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    char current_char;
    std::unordered_map<std::string, TokenType> keywords;
    
    void advance();
    void skip_whitespace();
    char peek(int offset = 1) const;
    Token make_number();
    Token make_string();
    Token make_identifier();
    Token make_token(TokenType type, const std::string& value);
    void error(const std::string& message);
    
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
};

#endif
