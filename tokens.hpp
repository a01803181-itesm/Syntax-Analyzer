#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

enum class TokenType {
    // Regular Grammar Tokens
    KEYWORD_VAR, KEYWORD_IF, KEYWORD_ELSE, KEYWORD_WHILE, 
    KEYWORD_FUNCTION, KEYWORD_RETURN, KEYWORD_TRUE, KEYWORD_FALSE,

    OP_SUM, OP_SUBT, OP_MULT, OP_DIV, OP_EQUALS, 
    OP_IS_EQUAL, OP_IS_DIFFERENT, OP_LESS_THAN, OP_GREATER_THAN,
    OP_LESS_THAN_EQUAL, OP_GREATER_THAN_EQUAL,

    IDENTIFIER, VAL_NUM,

    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, SEMICOLON, COMMA,

    // Context-Free Grammar Tokens

    DEF_VAR_STATEMENT, ASSIGN_STATEMENT, CONDITION_STATEMENT, LOOP_STATEMENT,
    DEF_FUNCTION_STATEMENT, RETURN_STATEMENT, FUNCTION_CALL_STATEMENT,

    ANY_EXPRESSION,

    ARITHMETIC_EXP, LOGICAL_EXP, FUNCTION_CALL_EXP,

    // Reserved token when detecting errors
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class TokenDefinitions {
    public:
        std::unordered_map<char, TokenType> symbols{
            {'(', TokenType::LEFT_PAREN},
            {')', TokenType::RIGHT_PAREN},
            {'{', TokenType::LEFT_BRACE},
            {'}', TokenType::RIGHT_BRACE},
            {';', TokenType::SEMICOLON},
            {',', TokenType::COMMA}
        };
        std::unordered_map<std::string, TokenType> keywords{
            {"var", TokenType::KEYWORD_VAR},
            {"if", TokenType::KEYWORD_IF},
            {"else", TokenType::KEYWORD_ELSE},
            {"while", TokenType::KEYWORD_WHILE},
            {"function", TokenType::KEYWORD_FUNCTION},
            {"return", TokenType::KEYWORD_RETURN},
            {"true", TokenType::KEYWORD_TRUE},
            {"false", TokenType::KEYWORD_FALSE},
        };
        std::unordered_map<std::string, TokenType> operators{
            {"+", TokenType::OP_SUM},
            {"-", TokenType::OP_SUBT},
            {"*", TokenType::OP_MULT},
            {"/", TokenType::OP_DIV},
            {"=", TokenType::OP_EQUALS},
            {"<", TokenType::OP_LESS_THAN},
            {">", TokenType::OP_GREATER_THAN},
            {"<=", TokenType::OP_LESS_THAN_EQUAL},
            {">=", TokenType::OP_GREATER_THAN_EQUAL},
            {"==", TokenType::OP_IS_EQUAL},
            {"!=", TokenType::OP_IS_DIFFERENT}
        };
        std::unordered_set<TokenType> logical_operators{
            TokenType::OP_LESS_THAN,
            TokenType::OP_GREATER_THAN,
            TokenType::OP_IS_EQUAL,
            TokenType::OP_IS_DIFFERENT,
            TokenType::OP_LESS_THAN_EQUAL,
            TokenType::OP_GREATER_THAN_EQUAL
        };
        std::unordered_set<TokenType> arithmetical_operators{
            TokenType::OP_SUM,
            TokenType::OP_SUBT,
            TokenType::OP_MULT,
            TokenType::OP_DIV,
        };
        std::unordered_set<TokenType> any_expression{
            TokenType::VAL_NUM,
            TokenType::IDENTIFIER,
            TokenType::ARITHMETIC_EXP,
            TokenType::LOGICAL_EXP,
            TokenType::FUNCTION_CALL_EXP
        };
        std::unordered_set<TokenType> any_statement{
            TokenType::DEF_VAR_STATEMENT,
            TokenType::ASSIGN_STATEMENT,
            TokenType::CONDITION_STATEMENT,
            TokenType::LOOP_STATEMENT
        };
        std::unordered_set<TokenType> new_statement_delimiters{
            TokenType::SEMICOLON,
            TokenType::RIGHT_BRACE,
            TokenType::KEYWORD_VAR,
            TokenType::KEYWORD_IF,
            TokenType::KEYWORD_WHILE,
            TokenType::KEYWORD_FUNCTION,
            TokenType::KEYWORD_RETURN
        };
        std::pair<const TokenType, std::regex> identifier{
            TokenType::IDENTIFIER, std::regex(R"([a-zA-Z][a-zA-Z0-9_]*)")
        };
        std::pair<const TokenType, std::regex> val_num{
            TokenType::VAL_NUM, std::regex(R"([0-9]+(\.[0-9]+)?)")
        };
        std::unordered_set<char> other_symbols{' ', '\n'};
};

#endif