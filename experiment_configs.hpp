#ifndef EXPERIMENT_CONFIGS_HPP
#define EXPERIMENT_CONFIGS_HPP

#include <cinttypes>
#include <set>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include "analyzer_thread.hpp"
#include "process.hpp"

enum class DataOrientedExpCases {
    _1KB, _3KB, _5KB, _10KB, _30KB, _50KB, _100KB
};

class Experiment {
    protected:
    std::string title;
    Timer timer;
    uintmax_t total_data_processed;
    std::string output_filename;
    Process python_process;
    int return_status;
    double time;
    std::vector<std::filesystem::directory_entry> entries;
    Experiment(std::string _title, uintmax_t _data, std::vector<std::filesystem::directory_entry> _entries)
    : title{_title}, timer{Timer()}, entries{_entries}, total_data_processed{_data} {}
    Experiment() : timer{Timer()}, total_data_processed{0} {}
    public:
    std::string getTitle() { return title; }
    std::vector<std::filesystem::directory_entry> getEntries() { return entries; }
    uintmax_t getData() { return total_data_processed; }
    double getTime() { return time; }
    int getReturnStatus() { return return_status; }
    inline static double getSpeedUp(double time_sequential, double time_concurrent) {
        return time_sequential / time_concurrent;
    }
    inline static std::string getExperimentCaseString(DataOrientedExpCases _case) {
        switch (_case) {
            case DataOrientedExpCases::_1KB:
                return "1KB";
                break;
            case DataOrientedExpCases::_3KB:
                return "3KB";
                break;
            case DataOrientedExpCases::_5KB:
                return "5KB";
                break;
            case DataOrientedExpCases::_10KB:
                return "10KB";
                break;
            case DataOrientedExpCases::_30KB:
                return "30KB";
                break;
            case DataOrientedExpCases::_50KB:
                return "50KB";
                break;
            case DataOrientedExpCases::_100KB:
                return "100KB";
                break;
            default:
                throw std::runtime_error("Experiment case not found");
                break;
        }
    }
    virtual void briefSummary() = 0;
    virtual void fullSummary() = 0;
    virtual void writeResults() = 0;
    virtual ~Experiment() {};
};

class SequentialOrientedExp : public Experiment {
    void analyzeFiles() {
        Directories::clearDir(Directories::HTML_OUTPUTS);
        for (const std::filesystem::directory_entry &it : entries) {
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
    SequentialOrientedExp(std::string _title, uintmax_t _data, std::vector<std::filesystem::directory_entry> _entries) : Experiment{_title, _data, _entries} {
        std::cout << _title << " Sequential Experiment started" << std::endl;
        std::cout << "Analyzing " << _data << " bytes" << std::endl;
        std::cout << "In " << _entries.size() << " files" << std::endl; 
        analyzeFiles();
        writeResults();
        std::cout << "Sequential Experiment successfully finalized" << std::endl;
    }

    void briefSummary() override {
        std::cout << std::endl;
        std::cout << "SEQUENTIAL EXPERIMENT CASE SUMMARY" << std::endl;
        std::cout << "Data Size Case Experiment: " << title << std::endl;
        std::cout << "# of bytes processed in total: " << total_data_processed << std::endl;
        std::cout << "# of files processed in total: " << entries.size() << std::endl;
        std::cout << "Total time elapsed of finishing the experiment: " << time << std::endl << std::endl;
    }

    void fullSummary() override { briefSummary(); }

    void writeResults() override {
        output_filename = Directories::CSVS.string() + "/sequential_" + title + ".csv";
        std::ofstream file(output_filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to create CSV file");
        }
        file << "title,time,data,n_files\n";
        file << ("Sequential test of " + title + " data case") << "," << time << "," << total_data_processed << "," << entries.size();
        file.close();
    }
};

class DataOrientedExp : public Experiment {
    uintmax_t goal_data_size;
    uintmax_t limit_data_size;
    uintmax_t real_data_size;
    int p;
    std::set<std::filesystem::directory_entry,ComparisonBySize> sorted_entries;
    std::vector<std::unique_ptr<AnalyzerThread>> threads;
    void createNewThread() {
        threads.push_back(std::make_unique<AnalyzerThread>(real_data_size, goal_data_size, sorted_entries));
        threads.back()->startThread();
        total_data_processed += real_data_size;
        real_data_size = 0;
        sorted_entries.clear();
    }
    void joinThreads() {
        for (const std::unique_ptr<AnalyzerThread> &thread : threads) {
            thread->join();
        }
    }
    void traverseDirectory() {
        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(Directories::TESTS)) {
            uintmax_t size = entry.file_size();

            if (size > limit_data_size) continue;

            if (real_data_size > 0 && real_data_size + size >= limit_data_size) {
                createNewThread();
            }

            real_data_size += size;
            sorted_entries.insert(entry);
            entries.push_back(entry);

            if (real_data_size >= goal_data_size) {
                createNewThread();
            }
        }
    }
    void traverseDirectory(int number_threads) {
        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(Directories::TESTS)) {
            if (threads.size() >= number_threads) return;

            uintmax_t size = entry.file_size();

            if (size > limit_data_size) continue;

            if (real_data_size > 0 && real_data_size + size >= limit_data_size) {
                createNewThread();
            }

            real_data_size += size;
            sorted_entries.insert(entry);
            entries.push_back(entry);

            if (real_data_size >= goal_data_size && threads.size() < number_threads) {
                createNewThread();
            }
        }
        if (real_data_size > 0 && threads.size() < number_threads) {
            createNewThread();
        }
        p = std::min((int)std::thread::hardware_concurrency(), (int)threads.size());
    }
    void setCaseConditions(DataOrientedExpCases size) {
        switch (size) {
            case DataOrientedExpCases::_1KB:
                goal_data_size = Size::_1KB;
                limit_data_size = 2 * Size::_1KB;
                title = "1KB";
                break;
            case DataOrientedExpCases::_3KB:
                goal_data_size = Size::_3KB;
                limit_data_size = Size::_3KB + Size::_1KB;
                title = "3KB";
                break;
            case DataOrientedExpCases::_5KB:
                goal_data_size = Size::_5KB;
                limit_data_size = Size::_5KB + Size::_1KB;
                title = "5KB";
                break;
            case DataOrientedExpCases::_10KB:
                goal_data_size = Size::_10KB;
                limit_data_size = 2 * Size::_10KB;
                title = "10KB";
                break;
            case DataOrientedExpCases::_30KB:
                goal_data_size = Size::_30KB;
                limit_data_size = Size::_30KB + Size::_10KB;
                title = "30KB";
                break;
            case DataOrientedExpCases::_50KB:
                goal_data_size = Size::_50KB;
                limit_data_size = Size::_50KB + Size::_10KB;
                title = "50KB";
                break;
            case DataOrientedExpCases::_100KB:
                goal_data_size = Size::_100KB;
                limit_data_size = 2 * Size::_100KB;
                title = "100KB";
                break;
            default:
                throw std::runtime_error("No data oriented experiment case found with such configuration");
                break;
        }
    }
    void writeResults() override {
        output_filename = Directories::CSVS.string() + "/multithread_" + title + "_" + std::to_string(threads.size()) + "_threads.csv";
        std::ofstream file(output_filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to create CSV file");
        }
        file << "title,thread,time,data,n_files\n";
        int index = 1;
        for (const std::unique_ptr<AnalyzerThread> &thread : threads) {
            file << ("Multi-thread test of " + title + " data case. Thread #" + std::to_string(index)) << "," << index << "," << thread->getTime() << "," << thread->getData() << "," << thread->getNumberFiles() << "\n";
            ++index;
        }
        file.close();
    }
    
    void plotAndSaveResults() {
        // Your environment variable should look like this:
        // "PYTHON_EXEC": "c:\\users\\myUser\\anaconda3\\python.exe"
        // Notice that the use of Anaconda Python interpreter is preferred for this project
        // since it contains by default the matplotlib and numpy libraries required
        // to plot and manage the results of the experiments
        std::string python_executable = std::getenv("PYTHON_EXEC");
        std::string command = " plot_multithread_data.py " + output_filename + " " + title;
        std::wstring wcommand(command.begin(), command.end());
        python_process = Process(wcommand);
        return_status = python_process.getTerminateStatus();
    }
    public:
    DataOrientedExp(DataOrientedExpCases _case) : real_data_size{0}, Experiment() {
        Directories::clearDir(Directories::HTML_OUTPUTS);
        setCaseConditions(_case);
        std::cout << title << " Concurrent Experiment started" << std::endl;
        traverseDirectory();
        joinThreads();
        writeResults();
        plotAndSaveResults();
    }
    DataOrientedExp(DataOrientedExpCases _case, int _threads) : real_data_size{0}, Experiment() {
        Directories::clearDir(Directories::HTML_OUTPUTS);
        setCaseConditions(_case);
        std::cout << title << " Concurrent Experiment started" << std::endl;
        std::cout << "Generating " << _threads << " threads" << std::endl;
        traverseDirectory(_threads);
        std::cout << "All " << _threads << " threads generated successfully" << std::endl << std::endl;
        joinThreads();
        writeResults();
        plotAndSaveResults();
    }
    void briefSummary() override {
        std::cout << std::endl;
        std::cout << "MULTI-THREAD EXPERIMENT CASE SUMMARY" << std::endl;
        std::cout << "Data Size Case Experiment: " << goal_data_size << " bytes" << std::endl;
        std::cout << "# of threads created: " << threads.size() << std::endl;
        std::cout << "# of CPU cores used: " << p << std::endl;
        std::cout << "# of bytes processed in total: " << total_data_processed << std::endl;
        std::cout << "# of files processed in total: " << entries.size() << std::endl;
        std::cout << "Total time elapsed of finishing the experiment: " << timer.getElapsed() << std::endl << std::endl;
    }
    void fullSummary() override {
        briefSummary();
        int index = 1;
        for (const std::unique_ptr<AnalyzerThread> &it : threads) {
            std::cout << "Thread " << index << " summary" << std::endl;
            it->summary();
            ++index;
        }
    }
};

#endif