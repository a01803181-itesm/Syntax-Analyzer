#ifndef CSS_BUILDER_HPP
#define CSS_BUILDER_HPP

#include "directories.hpp"
#include "styles.hpp"

// Struct (which behaves as a class with its attributes set as public by default)
// Used to create the CSS file that will define the styles for the rest of the HTML
// files based on the theme mode the user requests
struct CSSBuilder {
    StyleDefinitions styles;
    std::string file_name;
    CSSBuilder(std::string filename, bool darkMode) : file_name{filename} {
        std::ofstream file(Directories::HTML_OUTPUTS + "/" + filename);
        if (!file.is_open()) {
            throw std::runtime_error("CSS stylesheet couldn't be created");
        }
        std::string content;
        if (darkMode) {
            content += "body {\n\tbackground-color: " + styles.dark_mode_background + ";\n\t";
            content += "font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;\n\t";
            content += "color: " + styles.dark_mode_colours.find(Style::OTHER)->second + ";\n}\n\n";
            content += generateStyleSheet(styles.dark_mode_colours);
        } else {
            content += "body {\n\tbackground-color: " + styles.light_mode_background + ";\n\t";
            content += "font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;\n}\n\n";
            content += "color: " + styles.light_mode_colours.find(Style::OTHER)->second + ";\n}\n\n";
            content += generateStyleSheet(styles.light_mode_colours);
        }
        file << content;
        file.close();
    }
    private:
        // Iteratively generates the class style definitions based on the colour palette provided
        std::string generateStyleSheet(std::unordered_map<Style,std::string>& colours) {
            std::string builder{};
            for (std::pair<Style,std::string> colour : colours) {
                builder += "." + styles.getStyleClassName(colour.first) + " {\n\t";
                builder += "color: " + colour.second + ";\n}\n\n";
            }
            return builder;
        }
};

#endif