#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include "tokens.hpp"

class Lexer {
    private:
        std::vector<Token> tokens;
    public:
        Lexer(std::string code) {
            std::string current_string{};
            int pointer = 0;
            while (pointer < code.length()) {
                auto symbol_it = TokenDefinitions().symbols.find(code[pointer]);
                auto op_it = TokenDefinitions().operators.find(code.substr(pointer, 1));
                auto other_it = TokenDefinitions().other_symbols.find(code[pointer]);
                if (symbol_it != TokenDefinitions().symbols.end()) {
                    if (current_string.length() > 0) {
                        pushBackToken(current_string);
                    }
                    pushBackToken(TokenDefinitions().symbols.find(code[pointer])->second, code.substr(pointer, 1));
                    current_string.clear();
                } else if (op_it != TokenDefinitions().operators.end() || code[pointer] == '!') {
                    if (current_string.length() > 0) {
                        pushBackToken(current_string);
                    }
                    if (code[pointer + 1] == '=') {
                        switch(code[pointer]) {
                            case '=':
                                pushBackToken(TokenType::OP_IS_EQUAL, "==");
                                break;
                            case '!':
                                pushBackToken(TokenType::OP_IS_DIFFERENT, "!=");
                                break;
                            case '<':
                                pushBackToken(TokenType::OP_LESS_THAN_EQUAL, "<=");
                                break;
                            case '>':
                                pushBackToken(TokenType::OP_GREATER_THAN_EQUAL, ">=");
                                break;
                        }
                        ++pointer;
                    } else {
                        pushBackToken(TokenDefinitions().operators.find(code.substr(pointer, 1))->second, code.substr(pointer, 1));
                        current_string.clear();
                    }
                } else if (other_it != TokenDefinitions().other_symbols.end()) {
                    pushBackToken(current_string);
                    current_string.clear();
                } else {
                    current_string += code[pointer];
                }
                ++pointer;
            }
        }

        std::vector<Token> getTokens() { return tokens; }

        void pushBackToken(TokenType type, std::string value) {
            tokens.push_back({type, value});
        }

        void pushBackToken(std::string input) {
            if (!input.empty()) {
                Token new_token;
                if (std::regex_match(input, TokenDefinitions().identifier.second)) {
                    // std::cout << "POTENTIAL IDENTIFIER DETECTED!!" << std::endl;
                    if (TokenDefinitions().keywords.find(input) != TokenDefinitions().keywords.end()) {
                        // std::cout << "NOT AN IDENTIFIER BUT A KEYWORD!" << std::endl;
                        new_token = {TokenDefinitions().keywords.find(input)->second, input};
                    } else {
                        // std::cout << "IDENTIFIER VERIFIED!!" << std::endl;
                        new_token = {TokenDefinitions().identifier.first, input};
                    }
                } else if (std::regex_match(input, TokenDefinitions().val_num.second)) {
                    // std::cout << "NUMERIC VALUE DETECTED!!" << std::endl;
                    new_token = {TokenDefinitions().val_num.first, input};
                } else {
                    // std::cout << "UNKOWN INPUT DETECTED..." << std::endl;
                    new_token = {TokenType::UNKNOWN, input};
                }
                // std::cout << input << std::endl;
                tokens.push_back(new_token);
            }
        }

        void printTokens() {
            std::cout << "# of tokens collected: " << tokens.size() << std::endl;
            for (Token token : tokens) {
                switch(token.type) {
                    case TokenType::SEMICOLON:
                        std::cout << "SEMICOLON: ";
                        break;
                    case TokenType::COMMA:
                        std::cout << "COMMA: ";
                        break;
                    case TokenType::LEFT_BRACE:
                        std::cout << "LEFT_BRACE: ";
                        break;
                    case TokenType::RIGHT_BRACE:
                        std::cout << "RIGHT_BRACE: ";
                        break;
                    case TokenType::LEFT_PAREN:
                        std::cout << "LEFT_PAREN: ";
                        break;
                    case TokenType::RIGHT_PAREN:
                        std::cout << "RIGHT_PAREN: ";
                        break;
                    case TokenType::OP_SUM:
                        std::cout << "OP_SUM: ";
                        break;
                    case TokenType::OP_SUBT:
                        std::cout << "OP_SUBT: ";
                        break;
                    case TokenType::OP_MULT:
                        std::cout << "OP_MULT: ";
                        break;
                    case TokenType::OP_DIV:
                        std::cout << "OP_DIV: ";
                        break;
                    case TokenType::OP_EQUALS:
                        std::cout << "OP_EQUALS: ";
                        break;
                    case TokenType::OP_IS_EQUAL:
                        std::cout << "OP_IS_EQUAL: ";
                        break;
                    case TokenType::OP_IS_DIFFERENT:
                        std::cout << "OP_IS_DIFFERENT: ";
                        break;
                    case TokenType::OP_LESS_THAN:
                        std::cout << "OP_LESS_THAN: ";
                        break;
                    case TokenType::OP_GREATER_THAN:
                        std::cout << "OP_GREATER_THAN: ";
                        break;
                    case TokenType::KEYWORD_VAR:
                        std::cout << "VAR: ";
                        break;
                    case TokenType::KEYWORD_ELSE:
                        std::cout << "ELSE: ";
                        break;
                    case TokenType::KEYWORD_IF:
                        std::cout << "IF: ";
                        break;
                    case TokenType::KEYWORD_TRUE:
                        std::cout << "TRUE: ";
                        break;
                    case TokenType::KEYWORD_FALSE:
                        std::cout << "FALSE: ";
                        break;
                    case TokenType::KEYWORD_FUNCTION:
                        std::cout << "FUNCTION: ";
                        break;
                    case TokenType::KEYWORD_RETURN:
                        std::cout << "RETURN: ";
                        break;
                    case TokenType::KEYWORD_WHILE:
                        std::cout << "WHILE: ";
                        break;
                    case TokenType::IDENTIFIER:
                        std::cout << "IDENTIFIER: ";
                        break;
                    case TokenType::VAL_NUM:
                        std::cout << "VAL_NUM: ";
                        break;
                    default:
                        std::cout << "UNKNOWN: ";
                        break;
                }
                std::cout << token.value << std::endl;
            }
        }
        void writeTokens(std::ofstream& output_file) {
            output_file << "# of tokens collected: " << tokens.size() << std::endl;
            for (Token token : tokens) {
                switch(token.type) {
                    case TokenType::SEMICOLON:
                        output_file << "SEMICOLON: ";
                        break;
                    case TokenType::COMMA:
                        output_file << "COMMA: ";
                        break;
                    case TokenType::LEFT_BRACE:
                        output_file << "LEFT_BRACE: ";
                        break;
                    case TokenType::RIGHT_BRACE:
                        output_file << "RIGHT_BRACE: ";
                        break;
                    case TokenType::LEFT_PAREN:
                        output_file << "LEFT_PAREN: ";
                        break;
                    case TokenType::RIGHT_PAREN:
                        output_file << "RIGHT_PAREN: ";
                        break;
                    case TokenType::OP_SUM:
                        output_file << "OP_SUM: ";
                        break;
                    case TokenType::OP_SUBT:
                        output_file << "OP_SUBT: ";
                        break;
                    case TokenType::OP_MULT:
                        output_file << "OP_MULT: ";
                        break;
                    case TokenType::OP_DIV:
                        output_file << "OP_DIV: ";
                        break;
                    case TokenType::OP_EQUALS:
                        output_file << "OP_EQUALS: ";
                        break;
                    case TokenType::OP_IS_EQUAL:
                        output_file << "OP_IS_EQUAL: ";
                        break;
                    case TokenType::OP_IS_DIFFERENT:
                        output_file << "OP_IS_DIFFERENT: ";
                        break;
                    case TokenType::OP_LESS_THAN:
                        output_file << "OP_LESS_THAN: ";
                        break;
                    case TokenType::OP_GREATER_THAN:
                        output_file << "OP_GREATER_THAN: ";
                        break;
                    case TokenType::KEYWORD_VAR:
                        output_file << "VAR: ";
                        break;
                    case TokenType::KEYWORD_ELSE:
                        output_file << "ELSE: ";
                        break;
                    case TokenType::KEYWORD_IF:
                        output_file << "IF: ";
                        break;
                    case TokenType::KEYWORD_TRUE:
                        output_file << "TRUE: ";
                        break;
                    case TokenType::KEYWORD_FALSE:
                        output_file << "FALSE: ";
                        break;
                    case TokenType::KEYWORD_FUNCTION:
                        output_file << "FUNCTION: ";
                        break;
                    case TokenType::KEYWORD_RETURN:
                        output_file << "RETURN: ";
                        break;
                    case TokenType::KEYWORD_WHILE:
                        output_file << "WHILE: ";
                        break;
                    case TokenType::IDENTIFIER:
                        output_file << "IDENTIFIER: ";
                        break;
                    case TokenType::VAL_NUM:
                        output_file << "VAL_NUM: ";
                        break;
                    default:
                        output_file << "UNKNOWN: ";
                        break;
                }
                output_file << token.value << std::endl;
            }
        }
        void printSyntaxErrors() {
            int errors = 0;
            for (Token token : tokens) {
                if (token.type == TokenType::UNKNOWN) {
                    std::cout << "SYNTAX ERROR DETECTED!!" << std::endl;
                    std::cout << token.value << std::endl;
                    ++errors;
                }
            }
            if (errors == 0) {
                std::cout << "CONGRATS!! No syntax errors detected :)" << std::endl;
            } else {
                std::cout << "Total errors found: " << errors << std::endl;
            }
        }
};

#endif