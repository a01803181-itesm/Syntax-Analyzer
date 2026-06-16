// Syntax Analyzer Created By Project Team 4:
// Alexander Mejia Tovar (A01803181)
// Santiago Abraham Rios Palacios (A018102727)
// Cassandra Vanessa Padilla Barovier (A01753547)

// The current project aims to perform a syntax analysis for a established
// programming language in two main stages: lexical and sintactical analysis,
// meaning that this syntax analysis does not reach the scope to semantically
// analyze the codes.

#include "experiment_configs.hpp"

int main() {
    int main_result = 0;
    Directories::clearDir(Directories::HTML_OUTPUTS);
    Directories::clearDir(Directories::CSVS);
    std::vector<std::pair<DataOrientedExpCases,int>> experiments{
        {DataOrientedExpCases::_1KB, 100},
        {DataOrientedExpCases::_3KB, 50},
        {DataOrientedExpCases::_5KB, 25},
        {DataOrientedExpCases::_10KB, 15},
        {DataOrientedExpCases::_30KB, 7},
        {DataOrientedExpCases::_50KB, 5},
        {DataOrientedExpCases::_100KB, 3}
    };
    const int average = 1;
    Experiment *data_oriented_exp = nullptr;
    Experiment *sequential_oriented_exp = nullptr;
    std::map<DataOrientedExpCases,double> sequential_experiment_times;
    std::map<DataOrientedExpCases,double> concurrent_experiment_times;
    for (int i = 0; i < average; ++i) {
        for (const std::pair<DataOrientedExpCases,int> &experiment : experiments) {
            data_oriented_exp = new DataOrientedExp(experiment.first, experiment.second);
            data_oriented_exp->briefSummary();
            sequential_oriented_exp = new SequentialOrientedExp(data_oriented_exp->getTitle(), data_oriented_exp->getData(), data_oriented_exp->getEntries());
            sequential_oriented_exp->briefSummary();
            main_result = main_result && data_oriented_exp->getReturnStatus() && sequential_oriented_exp->getReturnStatus();
            concurrent_experiment_times[experiment.first] += data_oriented_exp->getTime();
            sequential_experiment_times[experiment.first] += sequential_oriented_exp->getTime();
            delete data_oriented_exp;
            delete sequential_oriented_exp;
        }
    }
    // std::cout << std::endl << "Speed-Ups Summary:" << std::endl;
    for (int i = 0; i < 0; ++i) {
        std::cout << Experiment::getExperimentCaseString(experiments[i].first) << " Experiment Case" << std::endl;
        sequential_experiment_times[experiments[i].first] = sequential_experiment_times[experiments[i].first] / average;
        concurrent_experiment_times[experiments[i].first] = concurrent_experiment_times[experiments[i].first] / average;
        std::cout << "Average Sequential Time Elapsed: " << sequential_experiment_times[experiments[i].first] << std::endl;
        std::cout << "Average Concurrent Time Elapsed: " << concurrent_experiment_times[experiments[i].first] << std::endl;
        std::cout << "Speed-Up: " << Experiment::getSpeedUp(sequential_experiment_times[experiments[i].first], concurrent_experiment_times[experiments[i].first]) << std::endl << std::endl;
    }
    
    Process plot_sequential_vs_multithread = Process(LR"(c:\users\alexi\anaconda3\python.exe plot_seq_vs_multithread.py)");
    main_result = main_result && plot_sequential_vs_multithread.getTerminateStatus();
    return main_result;
}