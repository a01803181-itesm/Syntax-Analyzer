#ifndef STYLES_HPP
#define STYLES_HPP

#include <string>
#include <unordered_map>
#include <fstream>

// Enum class to delimit all CSS Styles based on the type of Token
enum class Style {
    IDENTIFIER, OPERATOR, BRACE, PARENTHESIS,
    KEYWORD, OTHER, PUNCTUATION, NUM_VAL, BACKGROUND
};

// Two colour set definitions for light and dark mode themes
struct StyleDefinitions {
    std::string dark_mode_background{"#151724"};
    
    std::string light_mode_background{"#ffffff"};

    std::unordered_map<Style,std::string> light_mode_colours{
        {Style::OPERATOR, "#1d0036"},
        {Style::BRACE, "#666100"},
        {Style::PARENTHESIS, "#003166"},
        {Style::KEYWORD, "#7d3600"},
        {Style::PUNCTUATION, "#333333"},
        {Style::NUM_VAL, "#241b8f"},
        {Style::OTHER, "#001c18"}
    };

    std::unordered_map<Style,std::string> dark_mode_colours{
        {Style::OPERATOR, "#e6c9ff"},
        {Style::BRACE, "#fffa99"},
        {Style::PARENTHESIS, "#99caff"},
        {Style::KEYWORD, "#ffb882"},
        {Style::PUNCTUATION, "#cccccc"},
        {Style::NUM_VAL, "#ffa2a1"},
        {Style::OTHER, "#e3fffb"}
    };

    // Returns the string representation of the enumerated class value for
    // writing them directly on the CSS file
    std::string getStyleClassName(Style style) {
        switch (style) {
            case Style::OPERATOR:
                return "operator";
                break;
            case Style::BRACE:
                return "brace";
                break;
            case Style::PARENTHESIS:
                return "parenthesis";
                break;
            case Style::NUM_VAL:
                return "num-val";
                break;
            case Style::PUNCTUATION:
                return "punctuation";
                break;
            case Style::KEYWORD:
                return "keyword";
                break;
            default:
                return "other";
                break;
        }
    }
};

#endif