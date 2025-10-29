#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>
#include <unordered_set>

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    Token current_token;
    int last_line_num;  // Track the last BASIC line number seen
    
    // Built-in function names (for recognition)
    std::unordered_set<std::string> builtin_functions;
    
    void advance();
    void expect(TokenType type);
    bool check(TokenType type) const;
    bool check_ahead_for_identifier() const;
    bool match(TokenType type);
    void error(const std::string& message);
    void skip_newlines();
    
    // Expression parsing (with precedence climbing)
    std::unique_ptr<ASTNode> parse_expression();
    std::unique_ptr<ASTNode> parse_logical_or();
    std::unique_ptr<ASTNode> parse_logical_and();
    std::unique_ptr<ASTNode> parse_logical_not();
    std::unique_ptr<ASTNode> parse_comparison();
    std::unique_ptr<ASTNode> parse_addition();
    std::unique_ptr<ASTNode> parse_term();
    std::unique_ptr<ASTNode> parse_power();
    std::unique_ptr<ASTNode> parse_unary();
    std::unique_ptr<ASTNode> parse_primary();
    std::unique_ptr<ASTNode> parse_function_call(const std::string& name);
    
    // Statement parsing
    std::unique_ptr<ASTNode> parse_print_stmt();
    std::unique_ptr<ASTNode> parse_rem_stmt();
    std::unique_ptr<ASTNode> parse_let_stmt();
    std::unique_ptr<ASTNode> parse_if_stmt();
    std::unique_ptr<ASTNode> parse_goto_stmt();
    std::unique_ptr<ASTNode> parse_end_stmt();
    std::unique_ptr<ASTNode> parse_for_stmt();
    std::unique_ptr<ASTNode> parse_while_stmt();
    std::unique_ptr<ASTNode> parse_do_loop_stmt();
    std::unique_ptr<ASTNode> parse_gosub_stmt();
    std::unique_ptr<ASTNode> parse_return_stmt();
    std::unique_ptr<ASTNode> parse_input_stmt();
    std::unique_ptr<ASTNode> parse_line_input_stmt();
    std::unique_ptr<ASTNode> parse_data_stmt();
    std::unique_ptr<ASTNode> parse_read_stmt();
    std::unique_ptr<ASTNode> parse_restore_stmt();
    std::unique_ptr<ASTNode> parse_dim_stmt();
    std::unique_ptr<ASTNode> parse_open_stmt();
    std::unique_ptr<ASTNode> parse_close_stmt();
    std::unique_ptr<SubDef> parse_sub_def();
    std::unique_ptr<FunctionDef> parse_function_def();
    std::unique_ptr<ASTNode> parse_call_stmt();
    std::unique_ptr<ASTNode> parse_cls_stmt();
    std::unique_ptr<ASTNode> parse_locate_stmt();
    std::unique_ptr<ASTNode> parse_color_stmt();
    std::unique_ptr<ASTNode> parse_key_stmt();
    std::unique_ptr<ASTNode> parse_randomize_stmt();
    std::unique_ptr<ASTNode> parse_beep_stmt();
    std::unique_ptr<ASTNode> parse_sleep_stmt();
    std::unique_ptr<ASTNode> parse_on_stmt();
    std::unique_ptr<DefFnStmt> parse_def_fn_stmt();
    std::unique_ptr<ASTNode> parse_screen_stmt();
    std::unique_ptr<ASTNode> parse_pset_stmt();
    std::unique_ptr<ASTNode> parse_line_graphics();  // LINE command (not parse_line!)
    std::unique_ptr<ASTNode> parse_circle_stmt();
    std::unique_ptr<ASTNode> parse_paint_stmt();
    std::unique_ptr<ASTNode> parse_sound_stmt();
    std::unique_ptr<ASTNode> parse_play_stmt();
    
    std::unique_ptr<Statement> parse_line();  // Parse a line of BASIC code
    std::unique_ptr<ASTNode> parse_statement();  // Parse just a statement (no line number wrapper)
    
    bool is_builtin_function(const std::string& name) const;
    
public:
    Parser(const std::vector<Token>& toks);
    std::unique_ptr<Program> parse();
};

#endif
