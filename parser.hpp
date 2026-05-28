#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include <stack>
#include "lexer.hpp"

class Parser {
    private:
        std::vector<Token> tokens;
        int token_cursor = 0;
        std::vector<TokenType> statements;
        std::vector<TokenType> expression_builder;
        std::vector<std::pair<TokenType,std::string>> errors;

        Token getToken() { return tokens[token_cursor]; }
        void moveCursorTo(int pos) { token_cursor = pos; }
        int getCursor() { return token_cursor; }
        bool isCursorOutOfBound() { return getCursor() >= tokens.size(); } 
        void moveToNextToken() { ++token_cursor; }
        bool matchCurrentTokenWith(TokenType type) { return getToken().type == type; }

        void analyzeCurrentToken(std::unordered_set<TokenType> tokens_set, std::string error, TokenType statement_error) {
            auto it = tokens_set.find(tokens[token_cursor].type);
            if (it != tokens_set.end()) {
                moveToNextToken();
            } else {
                throw std::make_pair(statement_error,std::runtime_error(error));
            }
        }

        void analyzeCurrentToken(TokenType type, std::string error, TokenType statement_error) {
            if (matchCurrentTokenWith(type)) {
                moveToNextToken();
            } else {
                throw std::make_pair(statement_error, std::runtime_error(error));
            }
        }

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

        void analyzeFunctionArguments() {
            analyzeAnyExpression();
            if (getToken().type == TokenType::COMMA) {
                while (getToken().type == TokenType::COMMA) {
                    analyzeCurrentToken(TokenType::COMMA, "Expected ',' delimiter", TokenType::FUNCTION_CALL_EXP);
                    analyzeAnyExpression();
                }
            }
        }

        void analyzePossibleFunctionCall() {
            if (getToken().type == TokenType::LEFT_PAREN) {
                analyzeCurrentToken(TokenType::LEFT_PAREN, "Expected '(' delimiter", TokenType::ANY_EXPRESSION);
                if (getToken().type != TokenType::RIGHT_PAREN)
                    analyzeFunctionArguments();
                analyzeCurrentToken(TokenType::RIGHT_PAREN, "Expected ')' delimiter", TokenType::ANY_EXPRESSION);
            }
        }

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
                analyzeAnyExpression();
            } else if (TokenDefinitions().logical_operators.find(getToken().type) != TokenDefinitions().logical_operators.end()) {
                analyzeCurrentToken(TokenDefinitions().logical_operators, "Expected an operator", TokenType::ANY_EXPRESSION);
                analyzeAnyExpression();
            }
        }

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

        void analyzeDefVarStatement() {
            std::cout << "Asserting whether " << getStatementToken(TokenType::DEF_VAR_STATEMENT) << " is correctly defined" << std::endl;
            analyzeCurrentToken(TokenType::KEYWORD_VAR, "expected 'var' keyword", TokenType::DEF_VAR_STATEMENT);
            analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::DEF_VAR_STATEMENT);
            if (getToken().type == TokenType::OP_EQUALS) {
                analyzeCurrentToken(TokenType::OP_EQUALS, "expected '=' operator", TokenType::DEF_VAR_STATEMENT);
                analyzeAnyExpression();
            }
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::DEF_VAR_STATEMENT);
            std::cout << "Indeed it is correctly defined" << std::endl;
            statements.push_back(TokenType::DEF_VAR_STATEMENT);
        }

        void analyzeAssignStatement() {
            std::cout << "Asserting whether " << getStatementToken(TokenType::ASSIGN_STATEMENT) << " is correctly defined" << std::endl;
            analyzeCurrentToken(TokenType::IDENTIFIER, "Expected an identifier", TokenType::ASSIGN_STATEMENT);
            analyzeCurrentToken(TokenType::OP_EQUALS, "Expected '=' operator", TokenType::ASSIGN_STATEMENT);
            analyzeAnyExpression();
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::ASSIGN_STATEMENT);
            std::cout << "Indeed it is correctly defined" << std::endl;
            statements.push_back(TokenType::ASSIGN_STATEMENT);
        }

        void analyzeConditionStatement() {
            std::cout << "Asserting whether " << getStatementToken(TokenType::CONDITION_STATEMENT) << " is correctly defined" << std::endl;
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
        }

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

        void analyzeReturnStatement() {
            analyzeCurrentToken(TokenType::KEYWORD_RETURN, "Expected 'return' keyword", TokenType::RETURN_STATEMENT);
            analyzeAnyExpression();
            analyzeCurrentToken(TokenType::SEMICOLON, "Expected ';' delimiter", TokenType::RETURN_STATEMENT);
            statements.push_back(TokenType::RETURN_STATEMENT);
        }

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

        void jumpError() {
            while (!isCursorOutOfBound()) {
                auto it = TokenDefinitions().new_statement_delimiters.find(getToken().type);
                if (it == TokenDefinitions().new_statement_delimiters.end()) {
                    moveToNextToken();
                } else {
                    moveToNextToken();
                    break;
                }
            }
        }
        
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
    std::vector<std::pair<TokenType, std::string>> getErrors() { return errors; }
        Parser(Lexer lx) {
            tokens = lx.getTokens();
            std::cout << "Starting parse analysis" << std::endl << std::endl;
            while (!isCursorOutOfBound()) {
                try {
                    predictStatement();
                } catch (const std::pair<TokenType,std::runtime_error> err) {
                    std::cerr << err.second.what() << std::endl;
                    errors.push_back({err.first, err.second.what()});
                    jumpError();
                }
                std::cout << std::endl;
            }
            std::cout << "Parse analysis finished ;)" << std::endl;
        }

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

        void printStatements() {
            for (TokenType statement : statements) {
                printStatement(statement);
                std::cout << std::endl;
            }
        }

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