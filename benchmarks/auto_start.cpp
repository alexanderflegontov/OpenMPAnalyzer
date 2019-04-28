#include "bench_lib.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include "omp.h"
#include <unordered_map>
#include <fstream>

const std::string &TRIGGER_EXE = "/home/hp/Documents/pintool_trigger.exe";

const std::string &PIN_PATH = "/home/hp/Documents/pin-3.5-97503-gac534ca30-gcc-linux/";
const std::string &PIN_EXE = PIN_PATH + "pin";

const std::string &PINTOOL_PATH= PIN_PATH + "source/tools/ManualExamples/obj-intel64/";
const std::string &PINTOOL_NAME = "myPinTool.so";

const std::string &RUN_WITH_PIN = TRIGGER_EXE +
                                " " + PIN_EXE +
                                " " + PINTOOL_PATH + PINTOOL_NAME + " ";

const std::string &BENCHMARK1_NAME = "/home/hp/Documents/benchmarks/benchmark1";
const std::string &BENCHMARK2_NAME = "/home/hp/Documents/benchmarks/benchmark2_all";
const std::string &TEST_MODE = "tm_off";

static std::unordered_map<uint8_t, std::string> PASSWORD_MAP =
{
    { 6, "123456" },
    { 7, "1234567" },
    { 8, "12345678" },
    { 9, "123456789" },
    { 10, "1234567890" },
    { 11, "12345678901" },
    { 12, "123456789012" }
};

static std::unordered_map<uint8_t, std::string> create_hashed_map()
{
    std::unordered_map<uint8_t, std::string> map;
    std::string file_name = "hashes.txt";
    const std::string &command = "./gethash ";
    std::system(("rm " + file_name).c_str());
    for(auto it = PASSWORD_MAP.cbegin(); it != PASSWORD_MAP.cend(); ++it)
    {
        const std::string &password = it->second;
        const std::string &cmd = command + password;
        std::system((cmd + " >> " + file_name).c_str());
    }

    std::ifstream myfile(file_name);
    if(myfile.is_open())
    {
        auto it = PASSWORD_MAP.cbegin();
        while (!myfile.eof())
        {
            std::string line;
            std::getline(myfile, line);
            if(!line.empty()){
                if(it == PASSWORD_MAP.cend())
                {
                    std::cout << "PASSWORD_MAP != map" << std::endl;
                    exit(2);
                }
                map.emplace(std::make_pair(it->first, line));
                ++it;
            }
        }
        myfile.close();
    }else{
        std::cout << "Could not open file:" << file_name << std::endl;
        exit(1);
    }
    return map;
}

static std::unordered_map<uint8_t, std::string> HASHED_PASSWORD_MAP = create_hashed_map();

double RunBenchmark1(bool underPin)
{
    const double start_time = omp_get_wtime();

    const std::vector<uint8_t> num_threads = {2, 4, 8};
    const std::vector<uint8_t> code_length = {6, 7, 8, 9, 10, 11, 12};
    const std::vector<uint8_t> num_sections = {1, 2, 4, 8, 32, 64, 128};
    const std::string &ALPHABET = "1234567890";

    std::string run_params;
    if(underPin)
    {
        run_params.assign(RUN_WITH_PIN);
    }
    for(auto it_th = num_threads.cbegin(); it_th != num_threads.cend(); ++it_th)
    {
        const std::string &numThreads = std::to_string(*it_th);
        for(auto it_codelen = code_length.cbegin(); it_codelen != code_length.cend(); ++it_codelen)
        {
            const uint8_t cur_codelen = *it_codelen;
            const std::string &codeLen = std::to_string(cur_codelen);
            const uint64_t maxNumIters = pow(ALPHABET.length(), cur_codelen);
            for(auto it_sec = num_sections.cbegin(); it_sec != num_sections.cend(); ++it_sec)
            {
                const uint64_t block_size = maxNumIters / (*it_sec);
                const std::string &blockSize = std::to_string(block_size);
                std::string hash;

                //const std::string &hash = "$(./gethash 12345678)";
                try
                {
                    hash = HASHED_PASSWORD_MAP.at(cur_codelen);
                }
                catch(const std::exception& e)
                {
                    std::cout << e.what() << '\n';
                    exit(3);
                }

                const std::string &app_params = BENCHMARK1_NAME +
                                                " " + hash +
                                                " " + codeLen +
                                                " " + codeLen +
                                                " " + ALPHABET +
                                                " " + TEST_MODE +
                                                " " + numThreads +
                                                " " + blockSize;
                
                const std::string &system_params = run_params + app_params;
                std::cout << "!!!!!!!! " << system_params << std::endl;
                std::system(system_params.c_str());
            }
        }
    }
    const double end_time = omp_get_wtime();
    const double duration_time = end_time - start_time;
    return duration_time;
}

double RunBenchmark2(bool underPin)
{
    const double start_time = omp_get_wtime();

    const std::vector<uint8_t> num_threads = {2, 4, 8};
    const std::vector<uint8_t> code_length = {6, 7, 8, 9, 10, 11, 12};
    const std::vector<uint8_t> num_sections = {1, 2, 4, 8, 32, 64, 128};
    const std::string &ALPHABETS_PATH = "/home/hp/Documents/benchmarks/conf4";
    const uint8_t ALPHABETS_LENGTH = 6; // ALPHABETS_LENGTH foreach in ALPHABETS_PATH

    std::string run_params;
    if(underPin)
    {
        run_params.assign(RUN_WITH_PIN);
    }
    for(auto it_th = num_threads.cbegin(); it_th != num_threads.cend(); ++it_th)
    {
        const std::string &numThreads = std::to_string(*it_th);
        for(auto it_codelen = code_length.cbegin(); it_codelen != code_length.cend(); ++it_codelen)
        {
            const uint8_t cur_codelen = *it_codelen;
            const std::string &codeLen = std::to_string(cur_codelen);
            const uint64_t maxNumIters = pow(ALPHABETS_LENGTH, cur_codelen);
            for(auto it_sec = num_sections.cbegin(); it_sec != num_sections.cend(); ++it_sec)
            {
                const uint64_t block_size = maxNumIters / (*it_sec);
                const std::string &blockSize = std::to_string(block_size);
                std::string hash;

                //const std::string &hash = "$(./gethash 12345678)";
                try
                {
                    hash = HASHED_PASSWORD_MAP.at(cur_codelen);
                }
                catch(const std::exception& e)
                {
                    std::cout << e.what() << '\n';
                    exit(3);
                }

                const std::string &app_params = BENCHMARK2_NAME +
                                                " " + hash +
                                                " " + codeLen +
                                                " " + codeLen +
                                                " " + ALPHABETS_PATH +
                                                " " + TEST_MODE +
                                                " " + numThreads +
                                                " " + numThreads +
                                                " " + blockSize;
                
                const std::string &system_params = run_params + app_params;
                std::cout << "!!!!!!!! " << system_params << std::endl;
                std::system(system_params.c_str());
            }
        }
    }
    const double end_time = omp_get_wtime();
    const double duration_time = end_time - start_time;
    return duration_time;
}

void PrintTimeInHMSFormat(const double duration_time)
{
    int hours = int(duration_time) / 3600;
    int minutes = (int(duration_time) - (hours * 3600)) / 60;
    int seconds = int(duration_time) - (hours * 3600) - (minutes * 60);
    MSG(hours << " hours " << minutes << " minutes " << seconds << " seconds." << std::endl);
}


int main(int argc, char* argv[])
{
    MSG("Welcome to auto run for benchmarks!" << std::endl);
    double duration_time = 0.;
    size_t benchmark_number = 0;
    bool bRunUnderPin = false;

    switch(argc)
    {
        case 1:{
            MSG("Give me following:" << std::endl);
            MSG(std::setw(5) << "1. " <<  "benchmark number" << " :int" << std::endl);
            MSG(std::setw(5) << "2. " <<  "bRunUnderPin" << " :bool" << std::endl);
            MSG( "####################" << std::endl);
            return 1;
        }
        case 3:{
            if(strcmp(argv[(2)], "true") == 0)
            {
                bRunUnderPin = true;
            }
        }
        case 2:{
            benchmark_number = std::stoull(argv[(1)]);
            break; 
        }
        default:
            break;
    }

    MSG("####### Input parameters:" << std::endl);
    MSG("  Benchmark number #" << benchmark_number << std::endl);
    MSG("  Run Under Pin: " << std::boolalpha << bRunUnderPin << std::endl);

    switch(benchmark_number)
    {
        case 0:{
            MSG("You haven't chosen any benchmark!" << std::endl);
            break;
        }
        case 1:{
            duration_time = RunBenchmark1(bRunUnderPin);
            break;
        }
        case 2:{
            duration_time = RunBenchmark2(bRunUnderPin);
            break; 
        }
        default:
            MSG("You have chosen unknown benchmark!" << std::endl);
            break;
    }
    
    MSG("total duration_time = " << duration_time << " seconds." << std::endl);
    PrintTimeInHMSFormat(duration_time);
   //system("pause");
   return 0;
}
