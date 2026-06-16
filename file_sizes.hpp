#ifndef FILE_SIZES_HPP
#define FILE_SIZES_HPP

#include <string>
#include <cstdint>
#include <math.h>

struct Size {
    inline static const uintmax_t _0KB = 0;
    inline static const uintmax_t _1KB = std::powl(2, 10);
    inline static const uintmax_t _2KB = 2 * _1KB;
    inline static const uintmax_t _3KB = 3 * _1KB;
    inline static const uintmax_t _5KB = 5 * _1KB;
    inline static const uintmax_t _10KB = 2 * _5KB;
    inline static const uintmax_t _30KB = 3 * _10KB;
    inline static const uintmax_t _50KB = 5 * _10KB;
    inline static const uintmax_t _100KB = 2 * _50KB;
};

struct ComparisonBySize {
    bool operator()(const std::filesystem::directory_entry &a, const std::filesystem::directory_entry &b) const {
        return a.file_size() < b.file_size();
    }
};

struct FileSize {
    uintmax_t size;
    std::string notation;
    FileSize *next;
    FileSize(uintmax_t _size, std::string _notation, FileSize &_next) : size{_size}, notation{_notation}, next{&_next} {}
    FileSize(uintmax_t _size, std::string _notation) : size{_size}, notation{_notation}, next{nullptr} {}
    bool operator<(const FileSize &other) const {
        return size < other.size;
    }
};

struct FileSizes {
    inline static FileSize _100KB = FileSize(Size::_100KB, "100KB");
    inline static FileSize _50KB = FileSize(Size::_50KB, "50KB", _100KB);
    inline static FileSize _10KB = FileSize(Size::_10KB, "10KB", _50KB);
    inline static FileSize _5KB = FileSize(Size::_5KB, "5KB", _10KB);
    inline static FileSize _3KB = FileSize(Size::_3KB, "3KB", _5KB);
    inline static FileSize _2KB = FileSize(Size::_2KB, "2KB", _3KB);
    inline static FileSize _1KB = FileSize(Size::_1KB, "1KB", _2KB);
    inline static FileSize _0KB = FileSize(Size::_0KB, "0KB", _1KB);
};

#endif