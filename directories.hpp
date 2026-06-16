#ifndef DIRECTORIES_HPP
#define DIRECTORIES_HPP

#include <filesystem>
#include <string>

struct Directories {
    inline static const std::filesystem::path TESTS = std::filesystem::path("./test");
    inline static const std::filesystem::path CSVS = std::filesystem::path("./csv");
    inline static const std::string HTML_OUTPUTS{"./html"};

    inline static void clearDir(std::filesystem::path dir) {
        std::filesystem::remove_all(dir);
        std::filesystem::create_directory(dir);
    }

    inline static void clearDir(std::string dir) {
        std::filesystem::remove_all(dir);
        std::filesystem::create_directory(dir);
    }
};

#endif