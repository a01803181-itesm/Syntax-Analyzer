#include <iostream>
#include <string>
#include <regex>
#include <set>
#include <cctype>
#include <vector>
#include <fstream>
#include "lexer.hpp"
#include "parser.hpp"

void analyzeFiles(std::vector<std::ifstream> &files) {
    int successful_read_files = 0;
    int successful_written_files = 0;
    int unsuccessful_read_files = 0;
    int unsuccessful_written_files = 0;
    for (int i = 0; i < files.size(); ++i) {
        if (files[i].is_open()) {
            std::cout << "File " << i + 1 << " successfully loaded" << std::endl;
            std::ofstream output_file("output_files/tokens_" + std::to_string(i + 1) + ".txt");
            if (output_file.is_open()) {
                std::cout << "Output file " << i + 1 << " successfully loaded" << std::endl;
                std::ostringstream ss;
                ss << files[i].rdbuf();
                std::string code{ss.str()};
                std::cout << "Analyzing code..." << std::endl;
                Lexer lexer(code);
                lexer.writeTokens(output_file);
                output_file.close();
                lexer.printSyntaxErrors();
                Parser parser(lexer);
                std::cout << std::endl << "Final Tokens that build the code" << std::endl;
                parser.printStatements();
                std::cout << std::endl;
                parser.printErrors();
                ++successful_written_files;
            } else {
                std::cout << "Output File " << i + 1 << " couldn't be opened" << std::endl;
                ++unsuccessful_written_files;
            }
            ++successful_read_files;
        } else {
            std::cout << "File " << i + 1 << " couldn't be loaded" << std::endl;
            ++unsuccessful_read_files;
        }
    }

    std::cout << "Execution Summary" << std::endl;
    std::cout << ".txt files successfully analyzed: " << successful_read_files << std::endl;
    std::cout << ".txt files couldn't be analyzed: " << unsuccessful_read_files << std::endl;
    std::cout << ".txt files successfully written: " << successful_written_files << std::endl;
    std::cout << ".txt files couldn't be written: " << unsuccessful_written_files << std::endl;
}

int main() {
    std::vector<std::ifstream> files;
    files.reserve(10);
    files.push_back(std::ifstream("input_files/772_shall_of.txt"));
    files.push_back(std::ifstream("input_files/792_julia_julia.txt"));
    files.push_back(std::ifstream("input_files/832_fruit_where.txt"));
    files.push_back(std::ifstream("input_files/844_bed_next.txt"));
    files.push_back(std::ifstream("input_files/872_bestow_meet.txt"));
    files.push_back(std::ifstream("input_files/896_guildenstern_well.txt"));
    files.push_back(std::ifstream("input_files/904_every_push.txt"));
    files.push_back(std::ifstream("input_files/925_down_loving.txt"));
    files.push_back(std::ifstream("input_files/1012_of_longing.txt"));
    files.push_back(std::ifstream("input_files/1144_i_palsy.txt"));

    std::vector<std::ifstream> file;
    file.reserve(1);
    file.push_back(std::ifstream("files/invalid_code.txt"));

    analyzeFiles(files);

    return 0;
}