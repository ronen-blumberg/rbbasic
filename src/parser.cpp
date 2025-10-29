#include "parser.h"
#include <stdexcept>
#include <algorithm>

Parser::Parser(const std::vector<Token>& toks) 
    : tokens(toks), pos(0), last_line_num(0) {
    
    if (!tokens.empty()) {
        current_token = tokens[0];
    }
    
    // Initialize built-in function names
    builtin_functions = {
        // Math functions
        "SIN", "COS", "TAN", "ATN", "SQR", "ABS", "INT", "SGN", 
        "RND", "LOG", "EXP", "CINT", "FIX",
        // String functions
        "LEFT$", "RIGHT$", "MID$", "LEN", "INSTR", "CHR$", "ASC", 
        "STR$", "VAL", "UCASE$", "LCASE$", "LTRIM$", "RTRIM$", "TRIM$",
        "SPACE$", "STRING$", "HEX$", "OCT$", "TAB",
        // System functions
        "INPUT$", "INKEY$", "DATE$", "TIME$", "TIMER",
        // Array functions
        "LBOUND", "UBOUND",
        // File functions
        "EOF", "LOF",
        // Graphics functions
        "POINT"
    };
}

void Parser::advance() {
    if (pos < tokens.size() - 1) {
        pos++;
        current_token = tokens[pos];
    }
}

bool Parser::check(TokenType type) const {
    return current_token.type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type) {
    if (!check(type)) {
        error("Expected " + std::string(token_type_to_string(type)) + 
              " but got " + std::string(token_type_to_string(current_token.type)));
    }
    advance();
}

void Parser::error(const std::string& message) {
    std::string error_msg = "Parse error at line " + std::to_string(current_token.line) + 
                           ", column " + std::to_string(current_token.column) + ": " + message;
    throw std::runtime_error(error_msg);
}

void Parser::skip_newlines() {
    while (match(TokenType::NEWLINE)) {}
}

bool Parser::is_builtin_function(const std::string& name) const {
    return builtin_functions.find(name) != builtin_functions.end();
}

// ============== EXPRESSION PARSING ==============

std::unique_ptr<ASTNode> Parser::parse_primary() {
    // Number literal
    if (check(TokenType::NUMBER)) {
        double value = std::stod(current_token.value);
        advance();
        return std::make_unique<NumberNode>(value);
    }
    
    // String literal
    if (check(TokenType::STRING)) {
        std::string value = current_token.value;
        advance();
        return std::make_unique<StringNode>(value);
    }
    
    // Variable or function call
    if (check(TokenType::IDENTIFIER)) {
        std::string name = current_token.value;
        advance();
        
        // Check for function call or array access
        if (check(TokenType::LPAREN)) {
            // Check if it's a built-in function or user-defined FN function
            if (is_builtin_function(name) || name.substr(0, 2) == "FN") {
                return parse_function_call(name);
            }
            
            // Otherwise it's array access
            advance();  // consume (
            
            auto array_access = std::make_unique<ArrayAccessNode>(name);
            
            if (!check(TokenType::RPAREN)) {
                array_access->indices.push_back(parse_expression());
                
                while (match(TokenType::COMMA)) {
                    array_access->indices.push_back(parse_expression());
                }
            }
            
            expect(TokenType::RPAREN);
            return array_access;
        }
        
        // Check if this is a zero-argument function that can be called without ()
        // In BASIC: RND, TIMER, INKEY$, DATE$, TIME$
        if (name == "RND" || name == "TIMER" || name == "INKEY$" || 
            name == "DATE$" || name == "TIME$") {
            // Treat as function call with no arguments
            auto func_call = std::make_unique<FunctionCallNode>(name);
            return func_call;
        }
        
        return std::make_unique<VariableNode>(name);
    }
    
    // Parenthesized expression
    if (match(TokenType::LPAREN)) {
        auto expr = parse_expression();
        expect(TokenType::RPAREN);
        return expr;
    }
    
    error("Expected expression");
    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parse_function_call(const std::string& name) {
    expect(TokenType::LPAREN);
    
    auto func_call = std::make_unique<FunctionCallNode>(name);
    
    if (!check(TokenType::RPAREN)) {
        func_call->arguments.push_back(parse_expression());
        
        while (match(TokenType::COMMA)) {
            func_call->arguments.push_back(parse_expression());
        }
    }
    
    expect(TokenType::RPAREN);
    return func_call;
}

std::unique_ptr<ASTNode> Parser::parse_unary() {
    if (match(TokenType::MINUS)) {
        auto operand = parse_unary();
        return std::make_unique<UnaryOpNode>("-", std::move(operand));
    }
    
    if (match(TokenType::PLUS)) {
        return parse_unary();
    }
    
    return parse_primary();
}

std::unique_ptr<ASTNode> Parser::parse_power() {
    auto left = parse_unary();
    
    while (match(TokenType::POWER)) {
        auto right = parse_unary();
        left = std::make_unique<BinaryOpNode>("^", std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_term() {
    auto left = parse_power();
    
    while (check(TokenType::MULTIPLY) || check(TokenType::DIVIDE) || check(TokenType::MOD)) {
        std::string op = current_token.value;
        TokenType op_type = current_token.type;
        advance();
        auto right = parse_power();
        
        if (op_type == TokenType::MOD) {
            left = std::make_unique<BinaryOpNode>("MOD", std::move(left), std::move(right));
        } else {
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_addition() {
    auto left = parse_term();
    
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        std::string op = current_token.value;
        advance();
        auto right = parse_term();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_comparison() {
    auto left = parse_addition();
    
    while (check(TokenType::EQUALS) || check(TokenType::LESS) || 
           check(TokenType::GREATER) || check(TokenType::LESS_EQUAL) ||
           check(TokenType::GREATER_EQUAL) || check(TokenType::NOT_EQUAL)) {
        std::string op = current_token.value;
        advance();
        auto right = parse_addition();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_logical_not() {
    if (match(TokenType::NOT)) {
        auto operand = parse_logical_not();
        return std::make_unique<UnaryOpNode>("NOT", std::move(operand));
    }
    
    return parse_comparison();
}

std::unique_ptr<ASTNode> Parser::parse_logical_and() {
    auto left = parse_logical_not();
    
    while (match(TokenType::AND)) {
        auto right = parse_logical_not();
        left = std::make_unique<BinaryOpNode>("AND", std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_logical_or() {
    auto left = parse_logical_and();
    
    while (match(TokenType::OR)) {
        auto right = parse_logical_and();
        left = std::make_unique<BinaryOpNode>("OR", std::move(left), std::move(right));
    }
    
    return left;
}

std::unique_ptr<ASTNode> Parser::parse_expression() {
    return parse_logical_or();
}

// ============== STATEMENT PARSING ==============

std::unique_ptr<ASTNode> Parser::parse_print_stmt() {
    expect(TokenType::PRINT);
    
    auto print_stmt = std::make_unique<PrintStmt>();
    
    // Check for file number (PRINT #1, ...)
    if (match(TokenType::HASH)) {
        if (!check(TokenType::NUMBER)) {
            error("Expected file number after #");
        }
        print_stmt->file_number = static_cast<int>(std::stod(current_token.value));
        advance();
        
        if (match(TokenType::COMMA)) {
            // Continue to parse expressions
        }
    }
    
    if (!check(TokenType::NEWLINE) && !check(TokenType::COLON) && !check(TokenType::END_OF_FILE)) {
        print_stmt->expressions.push_back(parse_expression());
        print_stmt->use_semicolon.push_back(false);
        
        while (match(TokenType::COMMA) || match(TokenType::SEMICOLON)) {
            bool was_semicolon = (tokens[pos - 1].type == TokenType::SEMICOLON);
            
            if (check(TokenType::NEWLINE) || check(TokenType::END_OF_FILE) || check(TokenType::COLON)) {
                break;
            }
            
            print_stmt->expressions.push_back(parse_expression());
            print_stmt->use_semicolon.push_back(was_semicolon);
        }
    }
    
    return print_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_rem_stmt() {
    expect(TokenType::REM);
    
    std::string comment;
    while (!check(TokenType::NEWLINE) && !check(TokenType::END_OF_FILE)) {
        comment += current_token.value + " ";
        advance();
    }
    
    return std::make_unique<RemStmt>(comment);
}

std::unique_ptr<ASTNode> Parser::parse_let_stmt() {
    bool has_let = match(TokenType::LET);
    
    if (!check(TokenType::IDENTIFIER)) {
        if (has_let) {
            error("Expected variable name after LET");
        } else {
            error("Expected LET statement or valid statement");
        }
    }
    
    std::string var_name = current_token.value;
    advance();
    
    // Check for array access
    std::unique_ptr<ASTNode> array_access = nullptr;
    if (match(TokenType::LPAREN)) {
        auto arr_access = std::make_unique<ArrayAccessNode>(var_name);
        
        arr_access->indices.push_back(parse_expression());
        while (match(TokenType::COMMA)) {
            arr_access->indices.push_back(parse_expression());
        }
        
        expect(TokenType::RPAREN);
        array_access = std::move(arr_access);
    }
    
    expect(TokenType::EQUALS);
    
    auto expr = parse_expression();
    
    auto let_stmt = std::make_unique<LetStmt>(var_name, std::move(expr));
    let_stmt->array_access = std::move(array_access);
    
    return let_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_if_stmt() {
    expect(TokenType::IF);
    
    auto condition = parse_expression();
    
    expect(TokenType::THEN);
    
    auto if_stmt = std::make_unique<IfStmt>(std::move(condition));
    
    // Check if it's old-style IF THEN linenum or block IF
    if (check(TokenType::NUMBER)) {
        // Old style: IF condition THEN linenum
        if_stmt->target_line = static_cast<int>(std::stod(current_token.value));
        advance();
        if_stmt->is_block = false;
    } else {
        // Block style IF...END IF or single-line IF THEN ... ELSE ...
        if_stmt->is_block = true;
        
        if (!check(TokenType::NEWLINE)) {
            // Single-line IF THEN statement (may have ELSE and/or multiple statements with colons)
            // Parse all statements until ELSE or newline
            while (!check(TokenType::NEWLINE) && !check(TokenType::ELSE) && !check(TokenType::END_OF_FILE)) {
                auto stmt = parse_statement();  // Parse statement without line wrapper
                if (stmt) {
                    // Wrap in Statement with line_num 0
                    if_stmt->then_body.push_back(std::make_unique<Statement>(0, std::move(stmt)));
                }
                // Check for colon (more statements in THEN part)
                if (check(TokenType::COLON)) {
                    advance();  // consume colon
                    continue;  // parse next statement
                }
                // If no colon, we're done with THEN part
                break;
            }
            
            // Check for ELSE in single-line IF
            if (check(TokenType::ELSE)) {
                advance();  // consume ELSE
                // Parse all statements in ELSE part (may also have colons)
                while (!check(TokenType::NEWLINE) && !check(TokenType::END_OF_FILE)) {
                    auto else_stmt = parse_statement();  // Parse statement without line wrapper
                    if (else_stmt) {
                        // Wrap in Statement with line_num 0
                        if_stmt->else_body.push_back(std::make_unique<Statement>(0, std::move(else_stmt)));
                    }
                    // Check for colon (more statements in ELSE part)
                    if (check(TokenType::COLON)) {
                        advance();  // consume colon
                        continue;  // parse next statement
                    }
                    // If no colon, we're done with ELSE part
                    break;
                }
            }
            
            return if_stmt;
        }
        
        skip_newlines();
        
        // Parse THEN block
        while (!check(TokenType::ELSE) && !check(TokenType::ELSEIF) && 
               !check(TokenType::END_IF) && !check(TokenType::END_OF_FILE)) {
            auto stmt = parse_line();
            if (stmt) {
                if_stmt->then_body.push_back(std::move(stmt));
            }
            
            if (!check(TokenType::END_OF_FILE)) {
                if (!match(TokenType::NEWLINE) && !check(TokenType::ELSE) && 
                    !check(TokenType::ELSEIF) && !check(TokenType::END_IF)) {
                    match(TokenType::COLON);
                }
            }
            skip_newlines();
        }
        
        // Handle ELSE
        if (match(TokenType::ELSE)) {
            skip_newlines();
            
            while (!check(TokenType::END_IF) && !check(TokenType::END_OF_FILE)) {
                auto stmt = parse_line();
                if (stmt) {
                    if_stmt->else_body.push_back(std::move(stmt));
                }
                
                if (!check(TokenType::END_OF_FILE) && !check(TokenType::END_IF)) {
                    if (!match(TokenType::NEWLINE)) {
                        match(TokenType::COLON);
                    }
                }
                skip_newlines();
            }
        }
        
        expect(TokenType::END_IF);
    }
    
    return if_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_goto_stmt() {
    expect(TokenType::GOTO);
    
    if (!check(TokenType::NUMBER)) {
        error("Expected line number after GOTO");
    }
    
    int target = static_cast<int>(std::stod(current_token.value));
    advance();
    
    return std::make_unique<GotoStmt>(target);
}

std::unique_ptr<ASTNode> Parser::parse_end_stmt() {
    expect(TokenType::END);
    return std::make_unique<EndStmt>();
}

std::unique_ptr<ASTNode> Parser::parse_for_stmt() {
    expect(TokenType::FOR);
    
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable name after FOR");
    }
    std::string var_name = current_token.value;
    advance();
    
    expect(TokenType::EQUALS);
    
    auto start_expr = parse_expression();
    
    expect(TokenType::TO);
    
    auto end_expr = parse_expression();
    
    std::unique_ptr<ASTNode> step_expr = nullptr;
    if (match(TokenType::STEP)) {
        step_expr = parse_expression();
    }
    
    auto for_stmt = std::make_unique<ForStmt>(var_name, std::move(start_expr), 
                                               std::move(end_expr), std::move(step_expr));
    
    if (!check(TokenType::NEWLINE) && !check(TokenType::COLON)) {
        error("Expected newline after FOR statement");
    }
    if (match(TokenType::NEWLINE)) {
        skip_newlines();
    } else if (match(TokenType::COLON)) {
        // Allow colon separator
    }
    
    while (!check(TokenType::NEXT) && !check(TokenType::END_OF_FILE)) {
        auto stmt = parse_line();
        if (stmt != nullptr) {
            for_stmt->body.push_back(std::move(stmt));
        }
        
        // Check for colon or newline
        if (match(TokenType::COLON)) {
            // After colon, might be NEXT (single-line FOR loop)
            if (check(TokenType::NEXT)) {
                break;  // Exit loop, will parse NEXT below
            }
            continue;  // Parse next statement
        }
        
        // If not NEXT or EOF, expect newline
        if (!check(TokenType::END_OF_FILE) && !check(TokenType::NEXT)) {
            if (!match(TokenType::NEWLINE)) {
                error("Expected newline or colon after statement");
            }
        }
        
        skip_newlines();
    }
    
    if (!check(TokenType::NEXT)) {
        error("Expected NEXT to close FOR loop");
    }
    
    // Capture the line number of the NEXT statement
    // last_line_num was set by parse_line() when it saw "300 NEXT I"
    for_stmt->next_line_num = last_line_num;
    
    expect(TokenType::NEXT);
    
    if (check(TokenType::IDENTIFIER)) {
        if (current_token.value != var_name) {
            error("NEXT variable mismatch: expected " + var_name + " but got " + current_token.value);
        }
        advance();
    }
    
    return for_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_while_stmt() {
    expect(TokenType::WHILE);
    
    auto condition = parse_expression();
    
    auto while_stmt = std::make_unique<WhileStmt>(std::move(condition));
    
    skip_newlines();
    
    while (!check(TokenType::WEND) && !check(TokenType::END_OF_FILE)) {
        auto stmt = parse_line();
        if (stmt) {
            while_stmt->body.push_back(std::move(stmt));
        }
        
        if (match(TokenType::COLON)) {
            continue;
        }
        
        if (!check(TokenType::END_OF_FILE) && !check(TokenType::WEND)) {
            if (!match(TokenType::NEWLINE)) {
                error("Expected newline after statement");
            }
        }
        
        skip_newlines();
    }
    
    expect(TokenType::WEND);
    
    return while_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_do_loop_stmt() {
    expect(TokenType::DO);
    
    auto do_stmt = std::make_unique<DoLoopStmt>();
    
    // Check for DO WHILE or DO UNTIL
    if (check(TokenType::WHILE)) {
        advance();
        do_stmt->condition = parse_expression();
        do_stmt->check_at_start = true;
        do_stmt->is_until = false;
    } else if (check(TokenType::UNTIL)) {
        advance();
        do_stmt->condition = parse_expression();
        do_stmt->check_at_start = true;
        do_stmt->is_until = true;
    } else {
        // DO without condition - check at end
        do_stmt->check_at_start = false;
    }
    
    skip_newlines();
    
    // Parse body
    while (!check(TokenType::LOOP) && !check(TokenType::END_OF_FILE)) {
        auto stmt = parse_line();
        if (stmt) {
            do_stmt->body.push_back(std::move(stmt));
        }
        
        if (match(TokenType::COLON)) {
            continue;
        }
        
        if (!check(TokenType::END_OF_FILE) && !check(TokenType::LOOP)) {
            if (!match(TokenType::NEWLINE)) {
                error("Expected newline after statement");
            }
        }
        
        skip_newlines();
    }
    
    expect(TokenType::LOOP);
    
    // Check for LOOP WHILE or LOOP UNTIL
    if (!do_stmt->check_at_start) {
        if (check(TokenType::WHILE)) {
            advance();
            do_stmt->condition = parse_expression();
            do_stmt->is_until = false;
        } else if (check(TokenType::UNTIL)) {
            advance();
            do_stmt->condition = parse_expression();
            do_stmt->is_until = true;
        }
    }
    
    return do_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_gosub_stmt() {
    expect(TokenType::GOSUB);
    
    if (!check(TokenType::NUMBER)) {
        error("Expected line number after GOSUB");
    }
    
    int target = static_cast<int>(std::stod(current_token.value));
    advance();
    
    return std::make_unique<GosubStmt>(target);
}

std::unique_ptr<ASTNode> Parser::parse_return_stmt() {
    expect(TokenType::RETURN);
    return std::make_unique<ReturnStmt>();
}

std::unique_ptr<ASTNode> Parser::parse_input_stmt() {
    expect(TokenType::INPUT);
    
    auto input_stmt = std::make_unique<InputStmt>();
    
    // Check for file number (INPUT #1, ...)
    if (match(TokenType::HASH)) {
        if (!check(TokenType::NUMBER)) {
            error("Expected file number after #");
        }
        input_stmt->file_number = static_cast<int>(std::stod(current_token.value));
        advance();
        expect(TokenType::COMMA);
    }
    
    // Check for prompt string
    if (check(TokenType::STRING)) {
        input_stmt->prompt = current_token.value;
        advance();
        
        if (match(TokenType::SEMICOLON) || match(TokenType::COMMA)) {
            // Continue to variables
        } else {
            error("Expected ; or , after INPUT prompt");
        }
    }
    
    // Parse variable list
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable name in INPUT statement");
    }
    
    input_stmt->variables.push_back(current_token.value);
    advance();
    
    while (match(TokenType::COMMA)) {
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected variable name after comma");
        }
        input_stmt->variables.push_back(current_token.value);
        advance();
    }
    
    return input_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_line_input_stmt() {
    expect(TokenType::LINE_INPUT);
    
    auto input_stmt = std::make_unique<LineInputStmt>();
    
    // Check for file number (LINE INPUT #1, ...)
    if (match(TokenType::HASH)) {
        if (!check(TokenType::NUMBER)) {
            error("Expected file number after #");
        }
        input_stmt->file_number = static_cast<int>(std::stod(current_token.value));
        advance();
        expect(TokenType::COMMA);
    }
    
    // Check for prompt string
    if (check(TokenType::STRING)) {
        input_stmt->prompt = current_token.value;
        advance();
        
        if (match(TokenType::SEMICOLON) || match(TokenType::COMMA)) {
            // Continue
        }
    }
    
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable name in LINE INPUT statement");
    }
    
    input_stmt->variable = current_token.value;
    advance();
    
    return input_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_data_stmt() {
    expect(TokenType::DATA);
    
    auto data_stmt = std::make_unique<DataStmt>();
    
    // Parse comma-separated values
    while (!check(TokenType::NEWLINE) && !check(TokenType::COLON) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::STRING)) {
            data_stmt->values.push_back(current_token.value);
            advance();
        } else if (check(TokenType::NUMBER)) {
            data_stmt->values.push_back(current_token.value);
            advance();
        } else if (check(TokenType::IDENTIFIER)) {
            data_stmt->values.push_back(current_token.value);
            advance();
        } else {
            error("Expected value in DATA statement");
        }
        
        if (!match(TokenType::COMMA)) {
            break;
        }
    }
    
    return data_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_read_stmt() {
    expect(TokenType::READ);
    
    auto read_stmt = std::make_unique<ReadStmt>();
    
    // Parse first variable (could be simple variable or array)
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable name in READ statement");
    }
    
    ReadStmt::ReadTarget target;
    target.name = current_token.value;
    advance();
    
    // Check for array indices
    if (match(TokenType::LPAREN)) {
        // Array access: READ A(I)
        do {
            target.indices.push_back(parse_expression());
        } while (match(TokenType::COMMA));
        expect(TokenType::RPAREN);
    }
    
    read_stmt->variables.push_back(std::move(target));
    
    // Parse additional variables
    while (match(TokenType::COMMA)) {
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected variable name after comma");
        }
        
        ReadStmt::ReadTarget next_target;
        next_target.name = current_token.value;
        advance();
        
        // Check for array indices
        if (match(TokenType::LPAREN)) {
            do {
                next_target.indices.push_back(parse_expression());
            } while (match(TokenType::COMMA));
            expect(TokenType::RPAREN);
        }
        
        read_stmt->variables.push_back(std::move(next_target));
    }
    
    return read_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_restore_stmt() {
    expect(TokenType::RESTORE);
    
    int line_num = 0;
    if (check(TokenType::NUMBER)) {
        line_num = static_cast<int>(std::stod(current_token.value));
        advance();
    }
    
    return std::make_unique<RestoreStmt>(line_num);
}

std::unique_ptr<ASTNode> Parser::parse_dim_stmt() {
    expect(TokenType::DIM);
    
    auto dim_stmt = std::make_unique<DimStmt>();
    
    // DIM can have multiple arrays: DIM A(10), B(5), C(3,3)
    do {
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected array name after DIM");
        }
        
        std::string array_name = current_token.value;
        advance();
        
        DimStmt::ArrayDecl array_decl(array_name);
        
        expect(TokenType::LPAREN);
        
        // Parse dimensions
        if (!check(TokenType::NUMBER)) {
            error("Expected array dimension");
        }
        
        array_decl.dimensions.push_back(static_cast<int>(std::stod(current_token.value)));
        advance();
        
        // Parse additional dimensions (for multi-dimensional arrays)
        while (check(TokenType::COMMA) && !check_ahead_for_identifier()) {
            match(TokenType::COMMA);
            if (!check(TokenType::NUMBER)) {
                error("Expected array dimension");
            }
            array_decl.dimensions.push_back(static_cast<int>(std::stod(current_token.value)));
            advance();
        }
        
        expect(TokenType::RPAREN);
        
        dim_stmt->arrays.push_back(array_decl);
        
        // Also populate legacy fields for first array (backward compatibility)
        if (dim_stmt->arrays.size() == 1) {
            dim_stmt->name = array_decl.name;
            dim_stmt->dimensions = array_decl.dimensions;
            dim_stmt->is_string = array_decl.is_string;
        }
        
    } while (match(TokenType::COMMA));  // Continue if there's a comma for next array
    
    return dim_stmt;
}

bool Parser::check_ahead_for_identifier() const {
    // Check if next token after comma is an identifier (another array name)
    if (pos + 1 < tokens.size()) {
        return tokens[pos + 1].type == TokenType::IDENTIFIER;
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::parse_open_stmt() {
    expect(TokenType::OPEN);
    
    auto open_stmt = std::make_unique<OpenStmt>();
    
    // OPEN filename FOR mode AS #filenumber
    if (!check(TokenType::STRING)) {
        error("Expected filename in OPEN statement");
    }
    
    open_stmt->filename = current_token.value;
    advance();
    
    // FOR keyword (optional in some dialects, but we'll require it)
    if (check(TokenType::FOR)) {
        advance();
    }
    
    // Mode: INPUT, OUTPUT, or APPEND
    // INPUT can be tokenized as INPUT (statement) or INPUT_KW (file mode)
    if (check(TokenType::INPUT) || check(TokenType::INPUT_KW) || 
        check(TokenType::OUTPUT_KW) || check(TokenType::APPEND)) {
        if (check(TokenType::INPUT)) {
            open_stmt->mode = "INPUT";
        } else {
            open_stmt->mode = current_token.value;
        }
        advance();
    } else {
        error("Expected INPUT, OUTPUT, or APPEND after FOR");
    }
    
    expect(TokenType::AS);
    expect(TokenType::HASH);
    
    if (!check(TokenType::NUMBER)) {
        error("Expected file number");
    }
    
    open_stmt->file_number = static_cast<int>(std::stod(current_token.value));
    advance();
    
    return open_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_close_stmt() {
    expect(TokenType::CLOSE);
    
    auto close_stmt = std::make_unique<CloseStmt>();
    
    // CLOSE or CLOSE #1 or CLOSE #1, #2
    if (match(TokenType::HASH)) {
        if (!check(TokenType::NUMBER)) {
            error("Expected file number after #");
        }
        close_stmt->file_numbers.push_back(static_cast<int>(std::stod(current_token.value)));
        advance();
        
        while (match(TokenType::COMMA)) {
            expect(TokenType::HASH);
            if (!check(TokenType::NUMBER)) {
                error("Expected file number after #");
            }
            close_stmt->file_numbers.push_back(static_cast<int>(std::stod(current_token.value)));
            advance();
        }
    }
    // If no file numbers, close all files
    
    return close_stmt;
}

std::unique_ptr<SubDef> Parser::parse_sub_def() {
    expect(TokenType::SUB);
    
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected subroutine name after SUB");
    }
    
    std::string sub_name = current_token.value;
    advance();
    
    auto sub_def = std::make_unique<SubDef>(sub_name);
    
    // Parse parameters
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) {
            if (!check(TokenType::IDENTIFIER)) {
                error("Expected parameter name");
            }
            sub_def->parameters.push_back(current_token.value);
            advance();
            
            while (match(TokenType::COMMA)) {
                if (!check(TokenType::IDENTIFIER)) {
                    error("Expected parameter name");
                }
                sub_def->parameters.push_back(current_token.value);
                advance();
            }
        }
        expect(TokenType::RPAREN);
    }
    
    skip_newlines();
    
    // Parse body
    while (!check(TokenType::END_SUB) && !check(TokenType::END_OF_FILE)) {
        auto stmt = parse_line();
        if (stmt) {
            sub_def->body.push_back(std::move(stmt));
        }
        
        if (match(TokenType::COLON)) {
            continue;
        }
        
        if (!check(TokenType::END_OF_FILE) && !check(TokenType::END_SUB)) {
            if (!match(TokenType::NEWLINE)) {
                error("Expected newline after statement");
            }
        }
        
        skip_newlines();
    }
    
    expect(TokenType::END_SUB);
    
    return sub_def;
}

std::unique_ptr<FunctionDef> Parser::parse_function_def() {
    expect(TokenType::FUNCTION);
    
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected function name after FUNCTION");
    }
    
    std::string func_name = current_token.value;
    advance();
    
    auto func_def = std::make_unique<FunctionDef>(func_name);
    
    // Parse parameters
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) {
            if (!check(TokenType::IDENTIFIER)) {
                error("Expected parameter name");
            }
            func_def->parameters.push_back(current_token.value);
            advance();
            
            while (match(TokenType::COMMA)) {
                if (!check(TokenType::IDENTIFIER)) {
                    error("Expected parameter name");
                }
                func_def->parameters.push_back(current_token.value);
                advance();
            }
        }
        expect(TokenType::RPAREN);
    }
    
    skip_newlines();
    
    // Parse body
    while (!check(TokenType::END_FUNCTION) && !check(TokenType::END_OF_FILE)) {
        auto stmt = parse_line();
        if (stmt) {
            func_def->body.push_back(std::move(stmt));
        }
        
        if (match(TokenType::COLON)) {
            continue;
        }
        
        if (!check(TokenType::END_OF_FILE) && !check(TokenType::END_FUNCTION)) {
            if (!match(TokenType::NEWLINE)) {
                error("Expected newline after statement");
            }
        }
        
        skip_newlines();
    }
    
    expect(TokenType::END_FUNCTION);
    
    return func_def;
}

std::unique_ptr<ASTNode> Parser::parse_call_stmt() {
    expect(TokenType::CALL);
    
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected subroutine name after CALL");
    }
    
    std::string sub_name = current_token.value;
    advance();
    
    auto call_stmt = std::make_unique<CallStmt>(sub_name);
    
    // Parse arguments
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) {
            call_stmt->arguments.push_back(parse_expression());
            
            while (match(TokenType::COMMA)) {
                call_stmt->arguments.push_back(parse_expression());
            }
        }
        expect(TokenType::RPAREN);
    }
    
    return call_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_cls_stmt() {
    expect(TokenType::CLS);
    
    int mode = 0;
    if (check(TokenType::NUMBER)) {
        mode = static_cast<int>(std::stod(current_token.value));
        advance();
    }
    
    return std::make_unique<ClsStmt>(mode);
}

std::unique_ptr<ASTNode> Parser::parse_locate_stmt() {
    expect(TokenType::LOCATE);
    
    auto locate_stmt = std::make_unique<LocateStmt>();
    
    // LOCATE row, col [, cursor]
    if (!check(TokenType::NEWLINE) && !check(TokenType::COLON)) {
        locate_stmt->row = parse_expression();
        
        if (match(TokenType::COMMA)) {
            locate_stmt->col = parse_expression();
            
            if (match(TokenType::COMMA)) {
                locate_stmt->cursor = parse_expression();
            }
        }
    }
    
    return locate_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_color_stmt() {
    expect(TokenType::COLOR);
    
    auto color_stmt = std::make_unique<ColorStmt>();
    
    // COLOR foreground [, background [, border]]
    if (!check(TokenType::NEWLINE) && !check(TokenType::COLON)) {
        color_stmt->foreground = parse_expression();
        
        if (match(TokenType::COMMA)) {
            if (!check(TokenType::COMMA) && !check(TokenType::NEWLINE) && !check(TokenType::COLON)) {
                color_stmt->background = parse_expression();
            }
            
            if (match(TokenType::COMMA)) {
                color_stmt->border = parse_expression();
            }
        }
    }
    
    return color_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_key_stmt() {
    expect(TokenType::KEY);
    
    auto key_stmt = std::make_unique<KeyStmt>();
    
    // KEY OFF or KEY ON (we'll just ignore both)
    if (check(TokenType::IDENTIFIER)) {
        key_stmt->mode = current_token.value;  // "OFF" or "ON"
        advance();
    }
    
    return key_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_randomize_stmt() {
    expect(TokenType::RANDOMIZE);
    
    auto randomize_stmt = std::make_unique<RandomizeStmt>();
    
    // RANDOMIZE [seed]
    if (!check(TokenType::NEWLINE) && !check(TokenType::COLON) && !check(TokenType::END_OF_FILE)) {
        randomize_stmt->seed = parse_expression();
    }
    
    return randomize_stmt;
}

std::unique_ptr<ASTNode> Parser::parse_beep_stmt() {
    expect(TokenType::BEEP);
    return std::make_unique<BeepStmt>();
}

std::unique_ptr<ASTNode> Parser::parse_sleep_stmt() {
    expect(TokenType::SLEEP);
    auto stmt = std::make_unique<SleepStmt>();
    stmt->duration = parse_expression();  // Duration in seconds
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_on_stmt() {
    expect(TokenType::ON);
    
    auto expr = parse_expression();
    
    if (check(TokenType::GOTO)) {
        advance();
        
        auto on_goto = std::make_unique<OnGotoStmt>();
        on_goto->expression = std::move(expr);
        
        // Parse list of line numbers
        if (!check(TokenType::NUMBER)) {
            error("Expected line number after ON...GOTO");
        }
        on_goto->target_lines.push_back(static_cast<int>(std::stod(current_token.value)));
        advance();
        
        while (match(TokenType::COMMA)) {
            if (!check(TokenType::NUMBER)) {
                error("Expected line number in ON...GOTO list");
            }
            on_goto->target_lines.push_back(static_cast<int>(std::stod(current_token.value)));
            advance();
        }
        
        return on_goto;
        
    } else if (check(TokenType::GOSUB)) {
        advance();
        
        auto on_gosub = std::make_unique<OnGosubStmt>();
        on_gosub->expression = std::move(expr);
        
        // Parse list of line numbers
        if (!check(TokenType::NUMBER)) {
            error("Expected line number after ON...GOSUB");
        }
        on_gosub->target_lines.push_back(static_cast<int>(std::stod(current_token.value)));
        advance();
        
        while (match(TokenType::COMMA)) {
            if (!check(TokenType::NUMBER)) {
                error("Expected line number in ON...GOSUB list");
            }
            on_gosub->target_lines.push_back(static_cast<int>(std::stod(current_token.value)));
            advance();
        }
        
        return on_gosub;
        
    } else {
        error("Expected GOTO or GOSUB after ON expression");
        return nullptr;
    }
}

std::unique_ptr<DefFnStmt> Parser::parse_def_fn_stmt() {
    expect(TokenType::DEF);
    
    std::string fn_name;
    
    // Handle both "DEF FN name" and "DEF FNname"
    if (check(TokenType::FN)) {
        advance();  // Skip FN keyword
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected function name after DEF FN");
        }
        fn_name = "FN" + current_token.value;
        advance();
    } else if (check(TokenType::IDENTIFIER)) {
        // FN is part of the identifier (e.g., FNdouble)
        fn_name = current_token.value;
        if (fn_name.substr(0, 2) != "FN") {
            error("Function name must start with FN");
        }
        advance();
    } else {
        error("Expected function name after DEF");
    }
    
    auto def_fn = std::make_unique<DefFnStmt>(fn_name);
    
    // Parse parameters
    if (match(TokenType::LPAREN)) {
        if (!check(TokenType::RPAREN)) {
            if (!check(TokenType::IDENTIFIER)) {
                error("Expected parameter name");
            }
            def_fn->parameters.push_back(current_token.value);
            advance();
            
            while (match(TokenType::COMMA)) {
                if (!check(TokenType::IDENTIFIER)) {
                    error("Expected parameter name");
                }
                def_fn->parameters.push_back(current_token.value);
                advance();
            }
        }
        expect(TokenType::RPAREN);
    }
    
    expect(TokenType::EQUALS);
    
    // Parse the function expression
    def_fn->expression = parse_expression();
    
    return def_fn;
}


std::unique_ptr<Statement> Parser::parse_line() {
    int line_num = 0;
    if (check(TokenType::NUMBER)) {
        line_num = static_cast<int>(std::stod(current_token.value));
        last_line_num = line_num;  // Track this line number
        advance();
    }
    
    std::unique_ptr<ASTNode> stmt = nullptr;
    
    // Skip empty lines
    if (check(TokenType::NEWLINE) || check(TokenType::END_OF_FILE) || 
        check(TokenType::NEXT) || check(TokenType::WEND) || 
        check(TokenType::LOOP) || check(TokenType::END_IF) ||
        check(TokenType::END_SUB) || check(TokenType::END_FUNCTION) ||\
        check(TokenType::ELSE) || check(TokenType::ELSEIF)) {
        return nullptr;
    }
    
    // Parse statement based on keyword
    if (check(TokenType::PRINT)) {
        stmt = parse_print_stmt();
    } else if (check(TokenType::REM)) {
        stmt = parse_rem_stmt();
    } else if (check(TokenType::FOR)) {
        stmt = parse_for_stmt();
    } else if (check(TokenType::WHILE)) {
        stmt = parse_while_stmt();
    } else if (check(TokenType::DO)) {
        stmt = parse_do_loop_stmt();
    } else if (check(TokenType::IF)) {
        stmt = parse_if_stmt();
    } else if (check(TokenType::GOTO)) {
        stmt = parse_goto_stmt();
    } else if (check(TokenType::GOSUB)) {
        stmt = parse_gosub_stmt();
    } else if (check(TokenType::RETURN)) {
        stmt = parse_return_stmt();
    } else if (check(TokenType::END)) {
        stmt = parse_end_stmt();
    } else if (check(TokenType::INPUT)) {
        stmt = parse_input_stmt();
    } else if (check(TokenType::LINE_INPUT)) {
        stmt = parse_line_input_stmt();
    } else if (check(TokenType::DATA)) {
        stmt = parse_data_stmt();
    } else if (check(TokenType::READ)) {
        stmt = parse_read_stmt();
    } else if (check(TokenType::RESTORE)) {
        stmt = parse_restore_stmt();
    } else if (check(TokenType::DIM)) {
        stmt = parse_dim_stmt();
    } else if (check(TokenType::OPEN)) {
        stmt = parse_open_stmt();
    } else if (check(TokenType::CLOSE)) {
        stmt = parse_close_stmt();
    } else if (check(TokenType::CALL)) {
        stmt = parse_call_stmt();
    } else if (check(TokenType::CLS)) {
        stmt = parse_cls_stmt();
    } else if (check(TokenType::LOCATE)) {
        stmt = parse_locate_stmt();
    } else if (check(TokenType::COLOR)) {
        stmt = parse_color_stmt();
    } else if (check(TokenType::KEY)) {
        stmt = parse_key_stmt();
    } else if (check(TokenType::RANDOMIZE)) {
        stmt = parse_randomize_stmt();
    } else if (check(TokenType::BEEP)) {
        stmt = parse_beep_stmt();
    } else if (check(TokenType::SLEEP)) {
        stmt = parse_sleep_stmt();
    } else if (check(TokenType::ON)) {
        stmt = parse_on_stmt();
    } else if (check(TokenType::SCREEN)) {
        stmt = parse_screen_stmt();
    } else if (check(TokenType::PSET)) {
        stmt = parse_pset_stmt();
    } else if (check(TokenType::LINE)) {
        stmt = parse_line_graphics();
    } else if (check(TokenType::CIRCLE)) {
        stmt = parse_circle_stmt();
    } else if (check(TokenType::PAINT)) {
        stmt = parse_paint_stmt();
    } else if (check(TokenType::SOUND)) {
        stmt = parse_sound_stmt();
    } else if (check(TokenType::PLAY_KW)) {
        stmt = parse_play_stmt();
    } else if (check(TokenType::LET) || check(TokenType::IDENTIFIER)) {
        stmt = parse_let_stmt();
    } else {
        error("Unexpected token: " + std::string(token_type_to_string(current_token.type)));
    }
    
    return std::make_unique<Statement>(line_num, std::move(stmt));
}

// Helper function to parse just a statement without line number wrapper
// Used for parsing statements inside single-line IF THEN/ELSE with colons
std::unique_ptr<ASTNode> Parser::parse_statement() {
    std::unique_ptr<ASTNode> stmt = nullptr;
    
    // Skip empty/terminator tokens
    if (check(TokenType::NEWLINE) || check(TokenType::END_OF_FILE) || 
        check(TokenType::NEXT) || check(TokenType::WEND) || 
        check(TokenType::LOOP) || check(TokenType::END_IF) ||
        check(TokenType::END_SUB) || check(TokenType::END_FUNCTION) ||
        check(TokenType::ELSE) || check(TokenType::ELSEIF) ||
        check(TokenType::COLON)) {
        return nullptr;
    }
    
    // Parse statement based on keyword (same as parse_line but without line number handling)
    if (check(TokenType::PRINT)) {
        stmt = parse_print_stmt();
    } else if (check(TokenType::REM)) {
        stmt = parse_rem_stmt();
    } else if (check(TokenType::FOR)) {
        stmt = parse_for_stmt();
    } else if (check(TokenType::WHILE)) {
        stmt = parse_while_stmt();
    } else if (check(TokenType::DO)) {
        stmt = parse_do_loop_stmt();
    } else if (check(TokenType::IF)) {
        stmt = parse_if_stmt();
    } else if (check(TokenType::GOTO)) {
        stmt = parse_goto_stmt();
    } else if (check(TokenType::GOSUB)) {
        stmt = parse_gosub_stmt();
    } else if (check(TokenType::RETURN)) {
        stmt = parse_return_stmt();
    } else if (check(TokenType::END)) {
        stmt = parse_end_stmt();
    } else if (check(TokenType::INPUT)) {
        stmt = parse_input_stmt();
    } else if (check(TokenType::LINE_INPUT)) {
        stmt = parse_line_input_stmt();
    } else if (check(TokenType::DATA)) {
        stmt = parse_data_stmt();
    } else if (check(TokenType::READ)) {
        stmt = parse_read_stmt();
    } else if (check(TokenType::RESTORE)) {
        stmt = parse_restore_stmt();
    } else if (check(TokenType::DIM)) {
        stmt = parse_dim_stmt();
    } else if (check(TokenType::OPEN)) {
        stmt = parse_open_stmt();
    } else if (check(TokenType::CLOSE)) {
        stmt = parse_close_stmt();
    } else if (check(TokenType::CALL)) {
        stmt = parse_call_stmt();
    } else if (check(TokenType::CLS)) {
        stmt = parse_cls_stmt();
    } else if (check(TokenType::LOCATE)) {
        stmt = parse_locate_stmt();
    } else if (check(TokenType::COLOR)) {
        stmt = parse_color_stmt();
    } else if (check(TokenType::KEY)) {
        stmt = parse_key_stmt();
    } else if (check(TokenType::RANDOMIZE)) {
        stmt = parse_randomize_stmt();
    } else if (check(TokenType::BEEP)) {
        stmt = parse_beep_stmt();
    } else if (check(TokenType::SLEEP)) {
        stmt = parse_sleep_stmt();
    } else if (check(TokenType::ON)) {
        stmt = parse_on_stmt();
    } else if (check(TokenType::SCREEN)) {
        stmt = parse_screen_stmt();
    } else if (check(TokenType::PSET)) {
        stmt = parse_pset_stmt();
    } else if (check(TokenType::LINE)) {
        stmt = parse_line_graphics();
    } else if (check(TokenType::CIRCLE)) {
        stmt = parse_circle_stmt();
    } else if (check(TokenType::PAINT)) {
        stmt = parse_paint_stmt();
    } else if (check(TokenType::SOUND)) {
        stmt = parse_sound_stmt();
    } else if (check(TokenType::PLAY_KW)) {
        stmt = parse_play_stmt();
    } else if (check(TokenType::LET) || check(TokenType::IDENTIFIER)) {
        stmt = parse_let_stmt();
    } else {
        error("Unexpected token: " + std::string(token_type_to_string(current_token.type)));
    }
    
    return stmt;
}

// ============== GRAPHICS PARSING ==============

std::unique_ptr<ASTNode> Parser::parse_screen_stmt() {
    // SCREEN mode
    expect(TokenType::SCREEN);
    
    auto stmt = std::make_unique<ScreenStmt>();
    stmt->mode = parse_expression();
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_pset_stmt() {
    // PSET (x, y) [, color]
    expect(TokenType::PSET);
    
    auto stmt = std::make_unique<PsetStmt>();
    
    expect(TokenType::LPAREN);
    stmt->x = parse_expression();
    expect(TokenType::COMMA);
    stmt->y = parse_expression();
    expect(TokenType::RPAREN);
    
    // Optional color
    if (match(TokenType::COMMA)) {
        stmt->color = parse_expression();
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_line_graphics() {
    // LINE (x1, y1)-(x2, y2) [, color] [, B/BF]
    expect(TokenType::LINE);
    
    auto stmt = std::make_unique<LineStmt>();
    
    expect(TokenType::LPAREN);
    stmt->x1 = parse_expression();
    expect(TokenType::COMMA);
    stmt->y1 = parse_expression();
    expect(TokenType::RPAREN);
    
    expect(TokenType::MINUS);  // The dash between coordinates
    
    expect(TokenType::LPAREN);
    stmt->x2 = parse_expression();
    expect(TokenType::COMMA);
    stmt->y2 = parse_expression();
    expect(TokenType::RPAREN);
    
    // Optional color
    if (match(TokenType::COMMA)) {
        stmt->color = parse_expression();
        
        // Optional B or BF flag
        if (match(TokenType::COMMA)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string flag = current_token.value;
                if (flag == "B") {
                    stmt->is_box = true;
                    advance();
                } else if (flag == "BF") {
                    stmt->is_box = true;
                    stmt->is_filled = true;
                    advance();
                }
            }
        }
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_circle_stmt() {
    // CIRCLE (x, y), radius [, color] [, start, end] [, aspect]
    expect(TokenType::CIRCLE);
    
    auto stmt = std::make_unique<CircleStmt>();
    
    expect(TokenType::LPAREN);
    stmt->x = parse_expression();
    expect(TokenType::COMMA);
    stmt->y = parse_expression();
    expect(TokenType::RPAREN);
    
    expect(TokenType::COMMA);
    stmt->radius = parse_expression();
    
    // Optional color
    if (match(TokenType::COMMA)) {
        stmt->color = parse_expression();
        
        // Optional start and end angles
        if (match(TokenType::COMMA)) {
            stmt->start_angle = parse_expression();
            if (match(TokenType::COMMA)) {
                stmt->end_angle = parse_expression();
                
                // Optional aspect ratio
                if (match(TokenType::COMMA)) {
                    stmt->aspect = parse_expression();
                }
            }
        }
    }
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_paint_stmt() {
    // PAINT (x, y), paint_color [, border_color]
    expect(TokenType::PAINT);
    
    auto stmt = std::make_unique<PaintStmt>();
    
    expect(TokenType::LPAREN);
    stmt->x = parse_expression();
    expect(TokenType::COMMA);
    stmt->y = parse_expression();
    expect(TokenType::RPAREN);
    
    expect(TokenType::COMMA);
    stmt->paint_color = parse_expression();
    
    // Optional border color
    if (match(TokenType::COMMA)) {
        stmt->border_color = parse_expression();
    }
    
    return stmt;
}

// ============== SOUND PARSING ==============

std::unique_ptr<ASTNode> Parser::parse_sound_stmt() {
    // SOUND frequency, duration
    expect(TokenType::SOUND);
    
    auto stmt = std::make_unique<SoundStmt>();
    stmt->frequency = parse_expression();
    expect(TokenType::COMMA);
    stmt->duration = parse_expression();
    
    return stmt;
}

std::unique_ptr<ASTNode> Parser::parse_play_stmt() {
    // PLAY music_string
    expect(TokenType::PLAY_KW);
    
    auto stmt = std::make_unique<PlayStmt>();
    stmt->music_string = parse_expression();
    
    return stmt;
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    
    skip_newlines();
    
    while (!check(TokenType::END_OF_FILE)) {
        // Check for SUB or FUNCTION definitions (these don't have line numbers)
        if (check(TokenType::SUB)) {
            auto sub = parse_sub_def();
            program->subs.push_back(std::move(sub));
            
            if (match(TokenType::COLON)) {
                continue;
            }
            
            if (!check(TokenType::END_OF_FILE)) {
                if (!match(TokenType::NEWLINE)) {
                    error("Expected newline or colon after statement");
                }
            }
            
            skip_newlines();
            continue;
        } else if (check(TokenType::FUNCTION)) {
            auto func = parse_function_def();
            program->functions.push_back(std::move(func));
            
            if (match(TokenType::COLON)) {
                continue;
            }
            
            if (!check(TokenType::END_OF_FILE)) {
                if (!match(TokenType::NEWLINE)) {
                    error("Expected newline or colon after statement");
                }
            }
            
            skip_newlines();
            continue;
        }
        
        // Check if this line starts with a line number
        int line_num = 0;
        size_t saved_pos = pos;
        if (check(TokenType::NUMBER)) {
            line_num = static_cast<int>(std::stod(current_token.value));
            advance();
        }
        
        // After line number, check for DEF
        if (check(TokenType::DEF)) {
            auto def_fn = parse_def_fn_stmt();
            program->def_fns.push_back(std::move(def_fn));
            
            if (match(TokenType::COLON)) {
                continue;
            }
            
            if (!check(TokenType::END_OF_FILE)) {
                if (!match(TokenType::NEWLINE)) {
                    error("Expected newline or colon after statement");
                }
            }
            
            skip_newlines();
            continue;
        } else {
            // Put the line number back by seeking backward
            if (line_num > 0) {
                pos = saved_pos;
                current_token = tokens[pos];
            }
            
            auto stmt = parse_line();
            
            if (stmt != nullptr) {
                program->statements.push_back(std::move(stmt));
            }
        }
        
        if (match(TokenType::COLON)) {
            continue;
        }
        
        if (!check(TokenType::END_OF_FILE)) {
            if (!match(TokenType::NEWLINE)) {
                error("Expected newline or colon after statement");
            }
        }
        
        skip_newlines();
    }
    
    return program;
}
