#ifndef HTML_BUILDER_HPP
#define HTML_BUILDER_HPP

#include "css_builder.hpp"
#include "lexer.hpp"

// Class that creates the HTML file for the specified .txt file after being analyzed
// by the Lexer and Parser objects and linking the styles created by the
// CSSBuilder instance for better appearance
class HTMLBuilder {
    CSSBuilder stylesheet;
    std::string content;

    // Returns the required tabs to maintain a correct indentation
    // of the code
    std::string insertTabs(int number) {
        std::string builder{};
        while (number--) {
            builder += "\t";
        }
        return builder;
    }

    public:
        HTMLBuilder(std::string filename, std::string webpage_title, std::vector<Token> tokens, std::vector<std::pair<TokenType,std::string>> errors, bool darkMode) : stylesheet{"generated_stylesheet.css", darkMode} {
            std::ofstream file(Directories::HTML_OUTPUTS + "/" + filename);
            if (!file.is_open()) {
                throw std::runtime_error("HTML file couldn't be created");
            }
            // Writes the conventional html elements and tags
            file << "<!DOCTYPE html>\n";
            file << "<html>\n";
            file << "\t<head>\n";
            file << "\t\t<meta charset=\"UTF-8\">\n";
            file << "\t\t<link rel=\"stylesheet\" href=\"" + stylesheet.file_name + "\">\n";
            file << "\t\t<title>" + webpage_title + "</title>\n";
            file << "\t</head>\n";
            file << "\t<body>\n";
            file << "\t\t<h1>" + webpage_title + "</h1>\n";
            file << "\t\t<hr>\n";
            file << "\t\t<pre>\n";
            int tabs = 0;
            int token_counter = 0;
            // Traverses the tokens vector to display each of them on the html file
            // correctly indentated based on their category with their specified colour
            // class style using <pre> html tag for not affecting indentation presentation
            // and span classes to bind the token value to its style class without adding
            // semantic meaning to the html file
            for (Token token : tokens) {
                switch(token.type) {
                    case TokenType::IDENTIFIER:
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::IDENTIFIER) + "\">" + token.value + "</span>";
                        break;
                    case TokenType::KEYWORD_VAR:
                    case TokenType::KEYWORD_RETURN:
                    case TokenType::KEYWORD_FUNCTION:
                    case TokenType::KEYWORD_IF:
                    case TokenType::KEYWORD_ELSE:
                    case TokenType::KEYWORD_WHILE:
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::KEYWORD) + "\">" + token.value + "</span> ";
                        break;
                    case TokenType::VAL_NUM:
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::NUM_VAL) + "\">" + token.value + "</span>";
                        break;
                    case TokenType::LEFT_PAREN:
                    case TokenType::RIGHT_PAREN:
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::PARENTHESIS) + "\">" + token.value + "</span>";
                        break;
                    case TokenType::LEFT_BRACE:
                        ++tabs;
                        file << " <span class=\"" + stylesheet.styles.getStyleClassName(Style::BRACE) + "\">" + token.value + "</span>\n" + insertTabs(tabs);
                        break;
                    case TokenType::RIGHT_BRACE:
                        file.seekp(-1, std::ios::cur);
                        --tabs;
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::BRACE) + "\">" + token.value + "</span>\n" + insertTabs(tabs);
                        break;
                    case TokenType::COMMA:
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::PUNCTUATION) + "\">" + token.value + "</span> ";
                        break;
                    case TokenType::SEMICOLON:
                        file << "<span class=\"" + stylesheet.styles.getStyleClassName(Style::PUNCTUATION) + "\">" + token.value + "</span>\n" + insertTabs(tabs);
                        break;
                    case TokenType::OP_DIV:
                    case TokenType::OP_EQUALS:
                    case TokenType::OP_GREATER_THAN:
                    case TokenType::OP_GREATER_THAN_EQUAL:
                    case TokenType::OP_IS_DIFFERENT:
                    case TokenType::OP_IS_EQUAL:
                    case TokenType::OP_LESS_THAN:
                    case TokenType::OP_LESS_THAN_EQUAL:
                    case TokenType::OP_MULT:
                    case TokenType::OP_SUBT:
                    case TokenType::OP_SUM:
                        file << " <span class=\"" + stylesheet.styles.getStyleClassName(Style::OPERATOR) + "\">" + token.value + "</span> ";
                        break;
                }
                ++token_counter;
            }
            file << "\t\t</pre>\n";
            // If errors were found, displays them at the end of the file as an ordered list
            if (!errors.empty()) {
                file << "\t\t<hr>\n";
                file << "\t\t<h2>Error. " + std::to_string(errors.size()) + " Syntax Problems found</h2>\n";
                file << "\t\t<ol>\n";
                for (std::pair<TokenType,std::string> error : errors) {
                    file << "\t\t\t<li>" + error.second + "</li>\n";
                }
                file << "\t\t</ol>\n";
            }
            file << "\t</body>\n";
            file << "</html>";
            file.close();
        }
};

#endif