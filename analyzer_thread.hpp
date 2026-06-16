#ifndef ANALYZER_THREAD_HPP
#define ANALYZER_THREAD_HPP

#include <cstdint>
#include <thread>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <set>
#include "html_builder.hpp"
#include "timer.hpp"
#include "file_sizes.hpp"
#include "parser.hpp"

class AnalyzerThread {
    Timer timer;
    double time;
    uintmax_t data_size;
    uintmax_t classification;
    std::set<std::filesystem::directory_entry,ComparisonBySize> files;
    std::thread thread;

    void analyzeFiles() {
        for (const std::filesystem::directory_entry &it : files) {
            std::ifstream file(it.path());
        
            if (!file.is_open()) {
                std::cout << "Input File " << it.path().filename() << ".txt couldn't be opened" << std::endl;
            }

            std::ostringstream ss;
            ss << file.rdbuf();
            std::string code{ss.str()};
            Lexer lexer(code);
            Parser parser(lexer);
            try {
                std::string filename = "code_" + it.path().stem().string() + ".html";
                std::string html_title = "CODE: " + it.path().filename().string();
                HTMLBuilder html(filename, html_title, lexer.getTokens(), parser.getErrors(), true);
            } catch (const std::runtime_error err) {
                std::cerr << err.what() << std::endl;
            }
        }
        time = timer.getElapsed();
    }
    public:
    AnalyzerThread() {}
    AnalyzerThread(uintmax_t _size, uintmax_t _classification, std::set<std::filesystem::directory_entry,ComparisonBySize> _files)
    : data_size{_size}, classification{_classification}, timer{Timer()}, files{std::move(_files)} {}
    void startThread() {
        thread = std::thread(&AnalyzerThread::analyzeFiles, this);
    }
    void join() { thread.join(); }
    uintmax_t getData() { return data_size; }
    double getTime() { return time; }
    int getNumberFiles() { return files.size(); }
    void summary() {
        std::cout << "# of bytes processed: " << data_size << std::endl;
        std::cout << "# of files processed: " << files.size() << std::endl;
        std::cout << "Time elapsed: " << time << std::endl << std::endl;
    }
};

#endif