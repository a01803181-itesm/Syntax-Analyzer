#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

// Parser class to process the second stage of syntax analysis:
// Asserting the correct order of tokens to declare sintactic valid statements
class Parser {
    private:
        std::vector<Token> tokens;
        int token_cursor = 0;
        std::vector<TokenType> statements;
        std::vector<std::pair<TokenType,std::string>> errors;

        // Returns the current token the cursor is pointing to
        Token getToken() { return tokens[token_cursor]; }
        // Moves the token cursor to a specific position
        void moveCursorTo(int pos) { token_cursor = pos; }
        // Return the token cursor position
        int getCursor() { return token_cursor; }
        // Notifies whether the cursor is pointing out the tokens vector
        bool isCursorOutOfBound() { return getCursor() >= tokens.size(); }
        // Moves the cursor to the next token (increases by one) 
        void moveToNextToken() { ++token_cursor; }
        // Atomic function to determine whether the current token the cursor is pointing to
        // matches to the TokenType the function call received
        bool matchCurrentTokenWith(TokenType type) { return getToken().type == type; }

        // Determines whether the current pointed token belongs to the token set specified,
        // if it does not, throws an error since this function is called during particular
        // statement case analysis
        void analyzeCurrentToken(std::unordered_set<TokenType> tokens_set, std::string error, TokenType statement_error) {
            std::unordered_set<TokenType>::iterator it = tokens_set.find(tokens[token_cursor].type);
            if (it != tokens_set.end()) {
                moveToNextToken();
            } else {
                throw std::make_pair(statement_error,std::runtime_error(error));
            }
        }

        // Determines whether the current pointed token belongs to the TokenType specified,
        // if it does not, throws an error since this function is called during particular
        // statement case analysis
        void analyzeCurrentToken(TokenType type, std::string error, TokenType statement_error) {
            if (matchCurrentTokenWith(type)) {
                moveToNextToken();
            } else {
                throw std::make_pair(statement_error, std::runtime_error(error));
            }
        }

        // Asserts the correct definition of some function parameters (when it is the case)
        // According to the CFG:
        // S -> ( P )
        // P -> TokenType::IDENTIFIER M | \varepsilon
        // M -> , TokenType::IDENTIFIER M | \varepsilon
        void analyzeFunctionParameters() {
            if (getToken().type != TokenType::RIGHT_PAREN) {
                analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::DEF_FUNCTION_STATEMENT);
                if (getToken().type == TokenType::COMMA) {
                    while (getToken().type == TokenType::COMMA) {
                        analyzeCurrentToken(TokenType::COMMA, "Expected ',' delimiter", TokenType::DEF_FUNCTION_STATEMENT);
                        analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::DEF_FUNCTION_STATEMENT);
                    }
                }
            }
        }

        // Asserts the correct definition of some function arguments (when the function is being called)
        // According to the CFG:
        // S -> ( A )
        // A -> TokenType::ANY_EXPRESSION M | \varepsilon
        // M -> , TokenType::ANY_EXPRESSION M | \varepsilon
        void analyzeFunctionArguments() {
            analyzeAnyExpression();
            if (getToken().type == TokenType::COMMA) {
                while (getToken().type == TokenType::COMMA) {
                    analyzeCurrentToken(TokenType::COMMA, "Expected ',' delimiter", TokenType::FUNCTION_CALL_EXP);
                    analyzeAnyExpression();
                }
            }
        }

        // Asserts whether the next code segment is a function call or a simply identifier
        // (This is required since a function call is interpreted by the parser as an
        // identifier followed by a pair of parenthesis that may contain none or many arguments)
        void analyzePossibleFunctionCall() {
            if (getToken().type == TokenType::LEFT_PAREN) {
                analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::ANY_EXPRESSION);
                if (getToken().type != TokenType::RIGHT_PAREN)
                    analyzeFunctionArguments();
                analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::ANY_EXPRESSION);
            }
        }

        // Asserts wether the next code segment is any valid expression
        // Notice that an expression can be interpreted in many representations
        // Just as our CFG delimits:
        // A -> E | ( A )
        // E -> TokenType::VAL_NUM | TokenType::IDENTIFIER | TokenType::FUNC_CALL_EXPR | TokenType::ARITHMETIC_EXPR | TokenType::LOGICAL_EXP
        void analyzeAnyExpression() {
            if (getToken().type == TokenType::LEFT_PAREN) {
                analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::ANY_EXPRESSION);
                analyzeAnyExpression();
                analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::ANY_EXPRESSION);
            } else {
                if (getToken().type == TokenType::VAL_NUM) {
                    analyzeCurrentToken(TokenType::VAL_NUM, "Expected a numerical value", TokenType::ANY_EXPRESSION);
                } else if (getToken().type == TokenType::IDENTIFIER) {
                    analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::ANY_EXPRESSION);
                    analyzePossibleFunctionCall();
                }
            }
            if (TokenDefinitions().arithmetical_operators.find(getToken().type) != TokenDefinitions().arithmetical_operators.end()) {
                analyzeCurrentToken(TokenDefinitions().arithmetical_operators, "Expected an operator", TokenType::ANY_EXPRESSION);
                if (getToken().type == TokenType::LEFT_PAREN) {
                    analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::ANY_EXPRESSION);
                    analyzeAnyExpression();
                    analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::ANY_EXPRESSION);
                    analyzeAnyExpression();
                } else {
                    if (getToken().type == TokenType::VAL_NUM) {
                        analyzeCurrentToken(TokenType::VAL_NUM, "Expected a numerical value", TokenType::ANY_EXPRESSION);
                    } else if (getToken().type == TokenType::IDENTIFIER) {
                        analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::ANY_EXPRESSION);
                        analyzePossibleFunctionCall();
                    } else {
                        throw std::make_pair(TokenType::ANY_EXPRESSION, std::runtime_error("Expected an expression"));
                    }
                }
            } else if (TokenDefinitions().logical_operators.find(getToken().type) != TokenDefinitions().logical_operators.end()) {
                analyzeCurrentToken(TokenDefinitions().logical_operators, "Expected an operator", TokenType::ANY_EXPRESSION);
                if (getToken().type == TokenType::LEFT_PAREN) {
                    analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::ANY_EXPRESSION);
                    analyzeAnyExpression();
                    analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::ANY_EXPRESSION);
                } else {
                    if (getToken().type == TokenType::VAL_NUM) {
                        analyzeCurrentToken(TokenType::VAL_NUM, "Expected a numerical value", TokenType::ANY_EXPRESSION);
                    } else if (getToken().type == TokenType::IDENTIFIER) {
                        analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::ANY_EXPRESSION);
                        analyzePossibleFunctionCall();
                    } else {
                        throw std::make_pair(TokenType::ANY_EXPRESSION, std::runtime_error("Expected an expression"));
                    }
                }
            }
        }

        // Analyzes and asserts the correct definitions of any inner statement a parent statement
        // may contain (e.g. of parent statements are conditionals, loops or function definitions)
        // Notice that our CFG definition each of this parent containers are acceptable either when they
        // contain zero or many statements
        void analyzeInnerStatements() {
            int sentinel = statements.size();
            while (getToken().type != TokenType::RIGHT_BRACE) {
                if (isCursorOutOfBound()) {
                    throw std::make_pair(TokenType::UNKNOWN, "Expected '}' delimiter");
                }
                predictStatement();
            }
            if (statements.size() > sentinel) {
                statements.erase(statements.begin() + sentinel, statements.end());
            }
        }

        // Asserts whether the next code segment is a correct variable declaration
        // according to our CFG:
        // S -> TokenType::KEYWORD_VAR N
        // N -> TokenType::IDENTIFIER O
        // O -> TokenType::OP_EQUALS A | ;
        // A -< TokenType::ANY_EXPRESSION ;
        void analyzeDefVarStatement() {
            analyzeCurrentToken(TokenType::KEYWORD_VAR, "expected 'var' keyword", TokenType::DEF_VAR_STATEMENT);
            analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::DEF_VAR_STATEMENT);
            if (getToken().type == TokenType::OP_EQUALS) {
                analyzeCurrentToken(TokenType::OP_EQUALS, "expected '=' operator", TokenType::DEF_VAR_STATEMENT);
                analyzeAnyExpression();
            }
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::DEF_VAR_STATEMENT);
            statements.push_back(TokenType::DEF_VAR_STATEMENT);
        }

        // Asserts whether the next code segment is a correct assign statement
        // based on our CFG:
        // S -> TokenType::IDENTIFIER O
        // O -> TokenType::OP_EQUALS A
        // A -> TokenType::ANY_EXPRESSION ;
        void analyzeAssignStatement() {
            analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::ASSIGN_STATEMENT);
            analyzeCurrentToken(TokenType::OP_EQUALS, "Expected '=' operator", TokenType::ASSIGN_STATEMENT);
            analyzeAnyExpression();
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::ASSIGN_STATEMENT);
            statements.push_back(TokenType::ASSIGN_STATEMENT);
        }

        // Asserts whether the next code segment is a correct condition statement block
        // based on our CFG definition:
        // S -> TokenType::KEYWORD_IF ( C ) { B } O
        // C -> TokenType::ANY_EXPRESSION
        // B -> TokenType::ANY_STATEMENT B | \varepsilon
        // O -> TokenType::KEYWORD_ELSE { B } | \varepsilon
        // Note: we all know a condition statement evaluates based on a logical operation
        // Nevertheless, our parser is unable to identify when a function call as an expression
        // returns a true, false or null value, therefore our CFG limits at the moment to determine
        // whether the contained expression on the condition statement evaluation is sintactically correct
        void analyzeConditionStatement() {
            analyzeCurrentToken(TokenType::KEYWORD_IF, "Expected 'if' keyword", TokenType::CONDITION_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::CONDITION_STATEMENT);
            analyzeAnyExpression();
            analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expeted ')' delimiter", TokenType::CONDITION_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_BRACE, "Expected '{' delimiter", TokenType::CONDITION_STATEMENT);
            analyzeInnerStatements();
            analyzeCurrentToken(TokenType::RIGHT_BRACE, "Expected '}' delimiter", TokenType::CONDITION_STATEMENT);
            if (!isCursorOutOfBound() && getToken().type == TokenType::KEYWORD_ELSE) {
                analyzeCurrentToken(TokenType::KEYWORD_ELSE, "Expected 'else' keyword", TokenType::CONDITION_STATEMENT);
                analyzeCurrentToken(TokenType::LEFT_BRACE, "Expected '{' delimiter", TokenType::CONDITION_STATEMENT);
                analyzeInnerStatements();
                analyzeCurrentToken(TokenType::RIGHT_BRACE, "Expected '}' delimiter", TokenType::CONDITION_STATEMENT);
            }
            statements.push_back(TokenType::CONDITION_STATEMENT);
        }

        // Asserts whether the next code segment is a correct while loop declaration
        // based on our CFG:
        // S -> TokenType::KEYWORD_WHILE ( C ) { B }
        // C -> TokenType::ANY_EXPRESSION
        // B -> TokenType::ANY_STATEMENT B | \varepsilon
        // Note: similarly to the condition statement case, the loopable while statement
        // behaves based on an iterative logical evaluation that is out of the scope of
        // our parser to distinguish if the expression is logical or not, thus here again
        // our parser only limits to assert the expression contained is valid.
        void analyzeWhileStatement() {
            analyzeCurrentToken(TokenType::KEYWORD_WHILE, "Expected 'while' keyword", TokenType::LOOP_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::LOOP_STATEMENT);
            analyzeAnyExpression();
            analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::LOOP_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_BRACE, "Expected '{' delimiter", TokenType::LOOP_STATEMENT);
            analyzeInnerStatements();
            analyzeCurrentToken(TokenType::RIGHT_BRACE, "Expected '}' delimiter", TokenType::LOOP_STATEMENT);
            statements.push_back(TokenType::LOOP_STATEMENT);
        }

        // Asserts whether the next code segment is a correct function call statement
        // according to our CFG:
        // F -> TokenType::IDENTIFIER ( A ) ;
        // A -> TokenType::ANY_EXPRESSION R | \varepsilon
        // R -> , TokenType::ANY_EXPRESSION R | \varepsilon
        // Note: notice that when calling a function, all possible expressions are acceptable as
        // function arguments.
        void analyzeFunctionCallStatement() {
            analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::FUNCTION_CALL_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::FUNCTION_CALL_STATEMENT);
            if (getToken().type != TokenType::RIGHT_PAREN) {
                analyzeAnyExpression();
                while (getToken().type == TokenType::COMMA) {
                    analyzeCurrentToken(TokenType::COMMA, "Expected ',' delimiter", TokenType::FUNCTION_CALL_STATEMENT);
                    analyzeAnyExpression();
                }
            }
            analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::FUNCTION_CALL_STATEMENT);
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::ASSIGN_STATEMENT);
            statements.push_back(TokenType::FUNCTION_CALL_STATEMENT);
        }

        // Asserts whether the next code segment is a correct function definition
        // according to our CFG:
        // S -> TokenType::KEYWORD_FUNCTION N
        // N -> TokenType::IDENTIFIER ( P ) { B }
        // P -> TokenType::IDENTIFIER M | \varepsilon
        // M -> , TokenType::IDENTIFIER M | \varepsilon
        // B -> TokenType::ANY_STATEMENT B | TokenType::KEYWORD_RETURN R | \varepsilon
        // R -> TokenType::ANY_EXPRESSION ;
        // Note: notice that when defining a function, only identifiers are acceptable as
        // function parameters, thus no expressions are allowed when declaring a new function
        void analyzeDefFunctionStatement() {
            analyzeCurrentToken(TokenType::KEYWORD_FUNCTION, "Expected 'function' keyword", TokenType::DEF_FUNCTION_STATEMENT);
            analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::DEF_FUNCTION_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::DEF_FUNCTION_STATEMENT);
            analyzeFunctionParameters();
            analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::DEF_FUNCTION_STATEMENT);
            analyzeCurrentToken(TokenType::LEFT_BRACE, "Expected '{' delimiter", TokenType::DEF_FUNCTION_STATEMENT);
            analyzeInnerStatements();
            analyzeCurrentToken(TokenType::RIGHT_BRACE, "Expected '}' delimiter", TokenType::DEF_FUNCTION_STATEMENT);
            statements.push_back(TokenType::DEF_FUNCTION_STATEMENT);
        }

        // Asserts whether the next code segment is a valid return statement
        // based on our CFG:
        // R -> TokenType::KEYWORD_RETURN TokenType::ANY_EXPRESSION ;
        void analyzeReturnStatement() {
            analyzeCurrentToken(TokenType::KEYWORD_RETURN, "Expected 'return' keyword", TokenType::RETURN_STATEMENT);
            analyzeAnyExpression();
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::RETURN_STATEMENT);
            statements.push_back(TokenType::RETURN_STATEMENT);
        }

        // Function that tries to anticipate what statement to expect based on the
        // Token the cursor is pointing to whenever this function is called
        // and from that hint start analyzing and asserting whether the 
        // statement is sintactically correct or not
        void predictStatement() {
            switch (getToken().type) {
                case TokenType::KEYWORD_VAR:
                    analyzeDefVarStatement();
                    break;
                case TokenType::KEYWORD_IF:
                    analyzeConditionStatement();
                    break;
                case TokenType::KEYWORD_FUNCTION:
                    analyzeDefFunctionStatement();
                    break;
                case TokenType::KEYWORD_RETURN:
                    analyzeReturnStatement();
                    break;
                case TokenType::KEYWORD_WHILE:
                    analyzeWhileStatement();
                    break;
                case TokenType::IDENTIFIER:
                    moveToNextToken();
                    if (getToken().type == TokenType::LEFT_PAREN) {
                        moveCursorTo(getCursor() - 1);
                        analyzeFunctionCallStatement();
                    } else {
                        moveCursorTo(getCursor() - 1);
                        analyzeAssignStatement();
                    }
                    break;
                default:
                    throw std::make_pair(TokenType::UNKNOWN,std::runtime_error("Unknown statement start declaration"));
                    break;
            }
        }

        // Helper function that lets the Token cursor ignore all the tokens
        // that follow a syntax problem detection untill it reaches a token that
        // delimits a new statement definition (e.g. the usage of a semicolon, any
        // brace or parenthesis) or if it reaches the end of the tokens vector
        void jumpError() {
            while (!isCursorOutOfBound()) {
                std::unordered_set<TokenType>::iterator it = TokenDefinitions().new_statement_delimiters.find(getToken().type);
                if (it == TokenDefinitions().new_statement_delimiters.end()) {
                    moveToNextToken();
                } else {
                    moveToNextToken();
                    break;
                }
            }
        }
        
        // Overloaded function that behaves the exact way as the original function
        // but by looking for a specific Token provided as argument
        void jumpError(TokenType safe_token) {
            int sentinel_pos = getCursor();
            while (!isCursorOutOfBound()) {
                if (getToken().type == safe_token) {
                    break;
                } else {
                    moveToNextToken();
                }
            }
            moveCursorTo(sentinel_pos);
            jumpError();
        }
    public:
        // A parser may always receive a list of tokens previously analyzed by a lexer
        // Thus, defining the constructor with a lexer instance results appropiate
        Parser(Lexer lx) {
            tokens = lx.getTokens();
            // Keeps analyzing the tokens until the cursor reaches the end of the vector
            while (!isCursorOutOfBound()) {
                // Attempts to categorize the type of statement
                try {
                    predictStatement();
                }
                // Whenever one of the statement-case analysis fails and throws the
                // std::runtime_error, we catch the error and save it on a reserved
                // errors vector to keep a track of them and assert whether the code
                // is sintactically correct or not. However, the analysis does not stop here
                // but continues attempting to categorize the rest of the code
                catch (const std::pair<TokenType,std::runtime_error> err) {
                    // std::cerr << err.second.what() << std::endl;
                    errors.push_back({err.first, err.second.what()});
                    jumpError();
                }
            }
        }

        // Returns the vector of errors found after the parse analysis
        std::vector<std::pair<TokenType,std::string>> getErrors() { return errors; }

        // Returns the string representation of a given TokenType
        std::string getStatementToken(TokenType statement) {
            switch (statement) {
                case TokenType::CONDITION_STATEMENT:
                    return "CONDITION_STATEMENT";
                    break;
                case TokenType::LOOP_STATEMENT:
                    return "LOOP_STATEMENT";
                    break;
                case TokenType::DEF_FUNCTION_STATEMENT:
                    return "DEF_FUNCTION_STATEMENT";
                    break;
                case TokenType::DEF_VAR_STATEMENT:
                    return "DEF_VAR_STATEMENT";
                    break;
                case TokenType::ASSIGN_STATEMENT:
                    return "ASSIGN_STATEMENT";
                    break;
                default:
                    return "UNKNOWN_STATEMENT";
                    break;
            }
        }

        // Prints out the statement found (for debugging purposes)
        void printStatement(TokenType statement) {
            switch (statement) {
                    case TokenType::CONDITION_STATEMENT:
                        std::cout << "Condition statement found";
                        break;
                    case TokenType::LOOP_STATEMENT:
                        std::cout << "Loop statement found";
                        break;
                    case TokenType::DEF_FUNCTION_STATEMENT:
                        std::cout << "Function declaration statement found";
                        break;
                    case TokenType::DEF_VAR_STATEMENT:
                        std::cout << "Variable declaration statement found";
                        break;
                    case TokenType::ASSIGN_STATEMENT:
                        std::cout << "Variable assignation statement found";
                        break;
                    default:
                        std::cout << "UNKNOWN statement identifier";
                        break;
                }
        }

        // Prints out all the main statements gathered by the parser
        // Note: all inner statements will not be displayed as our statements
        // vector only stores the 'independent' parent statements that form the code
        void printStatements() {
            for (TokenType statement : statements) {
                printStatement(statement);
                std::cout << std::endl;
            }
        }

        // Prints out all the errors found by the parser analysis (if exist)
        void printErrors() {
            if (errors.size() == 0) {
                std::cout << "CONGRATS! No parse errors found" << std::endl;
            } else {
                int error_index = 1;
                for (std::pair<TokenType,std::string> error : errors) {
                    std::cout << "Error " << error_index << ": " << std::endl;
                    std::cout << error.second << std::endl;
                    std::cout << "In statement: " << getStatementToken(error.first) << std::endl << std::endl;
                    ++error_index;
                }
            }
        }
};

#endif