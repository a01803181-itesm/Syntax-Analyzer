#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer {
    std::chrono::time_point<std::chrono::steady_clock> start;
    public:
    Timer() : start{std::chrono::steady_clock::now()} {}

    double getElapsed() { 
        std::chrono::time_point<std::chrono::steady_clock> flag = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = flag - start;
        return elapsed.count();
    }

    ~Timer() {}
};

#endif