#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

// Enum class that defines all the possible tokens within a code
// to handle better switch cases
enum class TokenType {
    // Regular Grammar (RG) Tokens
    // Keywords
    KEYWORD_VAR, KEYWORD_IF, KEYWORD_ELSE, KEYWORD_WHILE, 
    KEYWORD_FUNCTION, KEYWORD_RETURN, KEYWORD_TRUE, KEYWORD_FALSE,

    // Operators
    // Arithmetical
    OP_SUM, OP_SUBT, OP_MULT, OP_DIV, OP_EQUALS,
    // Logical
    OP_IS_EQUAL, OP_IS_DIFFERENT, OP_LESS_THAN, OP_GREATER_THAN,
    OP_LESS_THAN_EQUAL, OP_GREATER_THAN_EQUAL,

    // Identifier and numerical values (including integer and decimal representations)
    IDENTIFIER, VAL_NUM,

    // Punctuation delimiter symbols
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, SEMICOLON, COMMA,

    // Context-Free Grammar (CFG) Tokens
    // Statements
    DEF_VAR_STATEMENT, ASSIGN_STATEMENT, CONDITION_STATEMENT, LOOP_STATEMENT,
    DEF_FUNCTION_STATEMENT, RETURN_STATEMENT, FUNCTION_CALL_STATEMENT,

    // Expressions
    ANY_EXPRESSION,
    ARITHMETIC_EXP, LOGICAL_EXP, FUNCTION_CALL_EXP,

    // Reserved token when detecting syntax problems
    UNKNOWN
};

// Structure designed to bind a token with user's string usages
// E.g. myVar is interpreted as an identifier, therefore, the Token struct follows the structure
// Token(TokenType::IDENTIFIER, "myVar")
struct Token {
    TokenType type;
    std::string value;
};

// Class containing all the token definitions categorized depending on their
// functionality
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
        std::unordered_set<TokenType> new_statement_delimiters{
            TokenType::SEMICOLON,
            TokenType::RIGHT_BRACE,
            TokenType::KEYWORD_VAR,
            TokenType::KEYWORD_IF,
            TokenType::KEYWORD_WHILE,
            TokenType::KEYWORD_FUNCTION,
            TokenType::KEYWORD_RETURN
        };
        // Identifier definition as a Regular Expression using regex standard header
        // and using our identifier syntax definition:
        // letter(letter + digit + underscore)^*
        std::pair<const TokenType, std::regex> identifier{
            TokenType::IDENTIFIER, std::regex(R"([a-zA-Z][a-zA-Z0-9_]*)")
        };
        // Numerical value definition as a Regular Expression
        // according to our definition:
        // digit^+(.digit^+)?
        std::pair<const TokenType, std::regex> val_num{
            TokenType::VAL_NUM, std::regex(R"([0-9]+(\.[0-9]+)?)")
        };
        std::unordered_set<char> other_symbols{' ', '\n'};
};

#endif