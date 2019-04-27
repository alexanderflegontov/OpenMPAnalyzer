#include "bench_lib.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
//#include <fstream>
#include "omp.h"

#include <unistd.h>
#include <fcntl.h>
//#include <errno.h>


const std::string OUTPUT_FILE = "benchmark1.out";

static int numThreads = 4;
static bool IsFound = false;
static bool test_mode = false;


void WriteResultLog(const uint64_t numThreads,
                    const uint64_t numCombs,
                    const double duration_time,
                    const uint64_t CodeLen,
                    const uint64_t numRegions,
                    const uint64_t block_size)
{
    std::string const &RESULT_CONTEXT = std::to_string(numThreads) + ","
                                      + std::to_string(numCombs) + ","
                                      + std::to_string(duration_time) + ","
                                      + std::to_string(CodeLen) + ","
                                      + std::to_string(numRegions) + ","
                                      + std::to_string(block_size) + "\n";

    int fd = open(OUTPUT_FILE.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    if(fd == -1)
    {
        MSG("Error open file: " << OUTPUT_FILE.c_str() <<  std::endl);
    }
    auto writedSize = write(fd, RESULT_CONTEXT.c_str(), RESULT_CONTEXT.length());
    if(writedSize == -1)
    {
        MSG("Error write to file: " << OUTPUT_FILE.c_str() <<  std::endl);
    }

    if(close(fd) == -1)
    {
        MSG("Error close file: " << OUTPUT_FILE.c_str() <<  std::endl);
    }
}

std::pair<double, std::string> Sequent_hack(const char* hash,
                                            const size_t MinCodeLen,
                                            const size_t MaxCodeLen,
                                            std::string const &alphabet)
{
    MSG("####### Sequent_hack #######" << std::endl);
    const size_t lenAlp = alphabet.length();
    uint64_t tmpMaxCombin = pow(lenAlp, MaxCodeLen);

#ifdef PRINT_ITER_LOG
    static size_t PrintInterval = 100;
    if((MaxCodeLen > 5) && (tmpMaxCombin > 1000)){
        double print_pwr = std::log10(tmpMaxCombin);
        PrintInterval = std::pow(10, static_cast<size_t>(print_pwr-1));
    }
    MSG("# PrintInterval = " << PrintInterval << std::endl);
#endif

    int save_iter = -1;
    double trigger_time = 0;
    const double start_time = omp_get_wtime();

    IsFound = false;
    std::string pwd;
    char* comb = new char[MaxCodeLen];
    unsigned char* strRetDigistHash = new unsigned char[HASH_LENGTH_MAX];

    for(auto CodeLen = MinCodeLen; !IsFound && CodeLen <= MaxCodeLen; ++CodeLen)
    {
        const uint64_t maxCountIter = pow(lenAlp, CodeLen);
        for(auto iter = 0; iter < maxCountIter; ++iter)
        {
            uint64_t tmpIter = iter;
            //MSG("current:");
            for(auto len = 0; len < CodeLen; ++len)
            //for(auto len = (CodeLen-1); len >= 0; --len)
            {
                const uint64_t index = tmpIter % lenAlp;
                comb[len] = alphabet[index];
                //MSG(index);
                tmpIter /= lenAlp;
            }
            //MSG(std::endl);
            //MSG("Comb = " << std::string(comb, CodeLen) << std::endl);

            GetHash256(comb, CodeLen, strRetDigistHash);

            if(memcmp(hash, strRetDigistHash, HASH_LENGTH_MAX) == 0)
            {
                trigger_time = omp_get_wtime() - start_time;
                //MSG("--------------START_Trigger_Time[tid = " << trigger_tid << "] = " << endtime << "sec. comb=" << comb << std::endl);
                pwd = std::string(comb, CodeLen);
                IsFound = true;
                save_iter = iter;
                //CodeLen = MaxCodeLen + 1;
                break;
            }

#ifdef PRINT_ITER_LOG
            if(iter % PrintInterval == 0)
            {
                MSG("------------iteration = " << iter << "." <<  std::endl);
                CurrentPrint(CodeLen, iter, maxCountIter, start_time); 
            }
#endif
            //double h2 = omp_get_wtime() - start_time;
            //MSG("One hash iteration calcTime = " << h2 - h1 << std::endl);
        }
    }
    delete[] comb;
    delete[] strRetDigistHash;

    const double end_time = omp_get_wtime();
    const double duration_time = end_time - start_time;
    MSG("------------Trigger_Time[main] = " << trigger_time << "sec." <<  std::endl);
    MSG("------------TotalTime = " << duration_time << "sec."<< std::endl);
    MSG(" -----------diff(TotalTime, Trigger_Time)= " << (duration_time - trigger_time) << "sec." << std::endl);
    MSG("------------save_iter = " << save_iter <<  std::endl);
    return std::make_pair(duration_time, pwd);
}

std::pair<double, std::string> Parallel_hack(const char* hash,
                                             const size_t MinCodeLen,
                                             const size_t MaxCodeLen,
                                             std::string const &alphabet,
                                             const uint64_t block_size)
{
    MSG("####### Parallel_hack #######" << std::endl);
    const size_t lenAlp = alphabet.length();
    auto info = CalculateInfo(MinCodeLen, MaxCodeLen, alphabet, block_size);
    const auto numRegions = std::get<0>(info);
    const auto numCombs = std::get<1>(info);
    const auto numLostCombs = std::get<2>(info);
    MSG("# numThreads = " << numThreads << std::endl);
    MSG("# numRegions = " << numRegions << std::endl);
    MSG("# numCombs = " << numCombs << std::endl);
    MSG("# lost combs = " << numLostCombs << std::endl);
    MSG("# block_size[regionCount=100] = " << numCombs / 100 << std::endl);

#ifdef PRINT_ITER_LOG
    static size_t PrintInterval = 100;
    if((MaxCodeLen > 5) && (numCombs > 1000)){
        double print_pwr = std::log10(numCombs);
        PrintInterval = std::pow(10, static_cast<size_t>(print_pwr-1));
    }
    MSG("# PrintInterval = " << PrintInterval << std::endl);
#endif

    int trigger_tid = -1, save_iter = -1;
    double trigger_time = 0;
    const double start_time = omp_get_wtime();
    omp_set_num_threads(numThreads);
    IsFound = false;
    std::string pwd;

    char* comb = new char[MaxCodeLen * block_size];
    unsigned char* strRetDigistHash = new unsigned char[HASH_LENGTH_MAX * block_size];

    for(auto CodeLen = MinCodeLen; !IsFound && CodeLen <= MaxCodeLen; ++CodeLen)
    {
        const uint64_t maxCountIter = pow(lenAlp, CodeLen);
    	//MSG("thread #" << omp_get_thread_num() << std::endl);

    	const auto iterBlockMax = maxCountIter / block_size;
        for(auto iterBlock = 0; iterBlock < iterBlockMax; ++iterBlock)
        {
            const auto block_offset = block_size * iterBlock;
            #pragma omp parallel for //num_threads(block_size)
            for(auto iter = 0; iter < block_size; ++iter)
            {
                char* pComb = comb + MaxCodeLen * iter;
                const uint64_t iteration = block_offset + iter;
                uint64_t tmpIter = iteration;
                //MSG("current:");
                for(auto len = 0; len < CodeLen; ++len)
                //for(auto len = (CodeLen-1); len >= 0; --len)
                {
                    const uint64_t index = tmpIter % lenAlp;
                    pComb[len] = alphabet[index];
                    //MSG(index);
                    tmpIter /= lenAlp;
                }
                //MSG(std::endl);
                auto pRetDigistHash = strRetDigistHash + HASH_LENGTH_MAX * iter;
                GetHash256(pComb, CodeLen, pRetDigistHash);

                if(memcmp(hash, pRetDigistHash, HASH_LENGTH_MAX) == 0)
                {
                    trigger_time = omp_get_wtime() - start_time;
                    trigger_tid = omp_get_thread_num();
                    //MSG("--------------START_Trigger_Time[tid = " << trigger_tid << "] = " << endtime << "sec. comb=" << comb << std::endl);
                    pwd = std::string(pComb, CodeLen);
                    IsFound = true;
                    save_iter = iteration;
                    iter = block_size;
                    #pragma omp cancel for
                }

#ifdef PRINT_ITER_LOG
                #pragma omp critical
                {
                    if(iteration % PrintInterval == 0)
                    {
                        MSG("------------iteration = " << iteration << "." <<  std::endl);
                        CurrentPrintParallel(CodeLen, iteration, maxCountIter, start_time); 
                    }
                }
#endif
                //double h2 = omp_get_wtime() - start_time;
                //MSG("One hash iteration calcTime = " << h2 - h1 << std::endl);
            }
        }
    }
    delete[] comb;
    delete[] strRetDigistHash;

    const double end_time = omp_get_wtime();
    const double duration_time = end_time - start_time;
    MSG("------------Trigger_Time[tid = " << trigger_tid << "] = " << trigger_time << "sec." <<  std::endl);
    MSG("------------TotalTime = " << duration_time << "sec."<< std::endl);
    MSG(" -----------diff(TotalTime, Trigger_Time)= " << (duration_time - trigger_time) << "sec." << std::endl);
    MSG("------------save_iter = " << save_iter <<  std::endl);
    WriteResultLog(numThreads, numCombs, duration_time, MaxCodeLen, numRegions, block_size);
    return std::make_pair(duration_time, pwd);
}

int main(int argc, char* argv[])
{
    const int PASSWORD_LENGTH_MAX = 10;
    const int ALPHABET_SET_MAX_COUNT = 100;
    const int PASSWORD_LENGTH_MIN = 1;

    std::string password, hash, alphabet = "123";
    size_t MaxCodeLen = 0, MinCodeLen = 0;
    uint64_t block_size = 1;

    MSG("Welcome to decoder!" << std::endl);
    switch(argc)
    {
        case 1:{
            MSG("Give me following:" << std::endl);
            MSG(std::setw(5) << "1. " <<  "sha256 hash" << " :string" << std::endl);
            MSG(std::setw(5) << "2. " <<  "MinCodeLen" << " :int" << std::endl);
            MSG(std::setw(5) << "3. " <<  "MaxCodeLen" << " :int" << std::endl);
            MSG(std::setw(5) << "4. " <<  "alphabet" << " :string" << std::endl);
            MSG(std::setw(5) << "5. " <<  "test-mode(run sequence alg also)" << " :string" << std::endl);
            MSG(std::setw(5) << "6. " <<  "numThreads" << " :int" << std::endl);
            MSG(std::setw(5) << "7. " <<  "block_size" << " :int" << std::endl);
            MSG("####################" << std::endl);
            return 1;
        }
        case 8:{
            block_size = std::stoull(argv[(7)]);
            if(block_size <= 0)
            {
                block_size = 1;
            }                
        }
        case 7:{
            numThreads = std::stoull(argv[(6)]);
            if(numThreads <= 0)
            {
                numThreads = 1;
            }                
        }
        case 6:{
            if(strcmp(argv[(5)], "test-mode") == 0)
            {
                test_mode = true;
            }
        }
        case 5:{
            alphabet = std::string(argv[(4)]);
            if(strlen(argv[(4)])+1 >  ALPHABET_SET_MAX_COUNT)
            {
                alphabet = std::string(argv[(4)]).substr(0, ALPHABET_SET_MAX_COUNT);
            }
        }
        case 4:{
            MaxCodeLen = std::stoull(argv[(3)]);
            if( !((PASSWORD_LENGTH_MIN <= MaxCodeLen) && (MaxCodeLen <= PASSWORD_LENGTH_MAX)) )
            {
                MaxCodeLen = 3; 
            }
        }
        case 3:{
            MinCodeLen = std::stoull(argv[(2)]);
            if( !((PASSWORD_LENGTH_MIN <= MinCodeLen) && (MinCodeLen <= PASSWORD_LENGTH_MAX)) )
            {
                MinCodeLen = 1; 
            }
        }
        case 2:{
            hash = std::string(argv[(1)]);
	        if(!CheckValidHash(hash) || (hash.length() != HASH_LENGTH_MAX*2))
            {
                MSG("Please input hash with length of " << HASH_LENGTH_MAX*2 << " chars!" << std::endl);
	            return 1;
            }
            break; 
        }
        default:
            break;
    }

    MSG("####### Input parameters:" << std::endl);
    MSG("  Your hash " << hash << std::endl);
    MSG("  Your params for searching: " << std::endl);
    MSG("    MinCodeLen: " << MinCodeLen << std::endl);
    MSG("    MaxCodeLen: " << MaxCodeLen << std::endl);
    MSG("    Alphabet for search: " << alphabet << std::endl);
    MSG("    block_size: " << block_size << std::endl);
    MSG("    test_mode: " << std::boolalpha << test_mode << std::endl);

    MSG(" Let's hack the hash! " << std::endl);
    MSG(" Please wait .." << std::endl << std::endl);
    std::string digistHash = HexToDigit(hash);
    auto ret_pair = Parallel_hack(digistHash.c_str(), MinCodeLen, MaxCodeLen, alphabet, block_size);
    const double parallTimeWork = ret_pair.first;
    const std::string pwdParall = ret_pair.second;
    if(IsFound){
        MSG(" Successful!" << std::endl);
        MSG(" passwordPar = " << pwdParall << std::endl);
    }else{
        MSG(" Not Found!" << std::endl);
    }

#ifndef PRINT_LOG
    std::cout << pwdParall << std::endl;
#endif

    if(test_mode){
        MSG(" Please wait .." << std::endl << std::endl);
        ret_pair = Sequent_hack(digistHash.c_str(), MinCodeLen, MaxCodeLen, alphabet);
        const double sequentTimeWork = ret_pair.first;
        std::string pwdSequent = ret_pair.second;
        if(IsFound){
            MSG(" Successful!" << std::endl);
            MSG(" passwordSeq = " << pwdSequent << std::endl);
        }else{
            MSG(" Not Found!" << std::endl);
            pwdSequent += "_ThisIsSalt";
        }

        MSG(std::endl << " Check the results ...");
        if(pwdParall.compare(pwdSequent) == 0)
        {
            MSG("Successfully!!!" << std::endl);
            PrintSpeedUpResults(sequentTimeWork, parallTimeWork);
        }
        else
        {
            MSG("Warning!!! Something went wrong." << std::endl);
        }
    }
   //system("pause");
   return 0;
}
