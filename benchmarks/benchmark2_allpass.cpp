#include "bench_lib.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include <fstream>
#include "omp.h"

#include <unordered_set>
#include <unistd.h>
#include <fcntl.h>
//#include <errno.h>


const std::string OUTPUT_FILE = "benchmark2_all.out";

static int numThreadsLvl1 = 1;
static int numThreadsLvl2 = 1;


void WriteResultLog(const uint64_t numThreads,
                    const uint64_t numCombs,
                    const double duration_time,
                    const uint64_t CodeLen,
                    const uint64_t numRegionsLvl2,
                    const uint64_t block_size,
                    const uint64_t numAlphabets,
                    const uint64_t oneAlphabetLen)
{
    std::string const &RESULT_CONTEXT = std::to_string(numThreads) + ","
                                      + std::to_string(numCombs) + ","
                                      + std::to_string(duration_time) + ","
                                      + std::to_string(CodeLen) + ","
                                      + std::to_string(numRegionsLvl2) + ","
                                      + std::to_string(block_size) + ","
                                      + std::to_string(numAlphabets) + ","
                                      + std::to_string(oneAlphabetLen) + "\n";

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

std::pair<double, std::vector<std::string>> Sequent_hack(const char* hash,
                                                         const size_t MinCodeLen,
                                                         const size_t MaxCodeLen,
                                                         const std::vector<std::string> &alphabets)
{
    MSG("####### Sequent_hack #######" << std::endl);

#ifdef PRINT_ITER_LOG
    uint64_t tmpMaxCombin = 0;
    for(auto i = 0; i < alphabets.size(); ++i)
    {
        uint64_t curMaxCombin = std::pow(alphabets[i].length(), MaxCodeLen);
        tmpMaxCombin += curMaxCombin;
        //MSG("# comb for alphabets[" << i << "] = " << curMaxCombin << std::endl;
    }
    MSG("# tmpMaxCombin = " << tmpMaxCombin << std::endl);

    static size_t PrintInterval = 100;
    if((MaxCodeLen > 5) && (tmpMaxCombin > 1000)){
        double print_pwr = std::log10(tmpMaxCombin);
        PrintInterval = std::pow(10, static_cast<size_t>(print_pwr-1));
    }
    MSG("# PrintInterval = " << PrintInterval << std::endl);
#endif
    MSG("########" << "start hacking.." << std::endl);

    const double start_time = omp_get_wtime();

    std::vector<std::string> vec_pwds;
    char* comb = new char[MaxCodeLen];
    unsigned char* strRetDigistHash = new unsigned char[HASH_LENGTH_MAX];

    for(auto CodeLen = MinCodeLen; CodeLen <= MaxCodeLen; ++CodeLen)
    {
        for(auto alp_num = 0; alp_num < alphabets.size(); ++alp_num)
        {
            const std::string& curAlphabet = alphabets[alp_num];
            const uint64_t lenAlp = curAlphabet.length();

            const uint64_t maxCountIter = pow(lenAlp, CodeLen);
            for(auto iter = 0; iter < maxCountIter; ++iter)
            {
                uint64_t tmpIter = iter;
                //MSG("current:");
                for(auto len = 0; len < CodeLen; ++len)
                //for(auto len = (CodeLen-1); len >= 0; --len)
                {
                    const uint64_t index = tmpIter % lenAlp;
                    comb[len] = curAlphabet[index];
                    //MSG(index);
                    tmpIter /= lenAlp;
                }
                //MSG(std::endl);
                //MSG("Comb = " << std::string(comb, CodeLen) << std::endl);

                GetHash256(comb, CodeLen, strRetDigistHash);

                if(memcmp(hash, strRetDigistHash, HASH_LENGTH_MAX) == 0)
                {
                    vec_pwds.push_back(std::string(comb, CodeLen));
                    //break; alp_num = alphabets.size(); CodeLen = MaxCodeLen + 1;
                    //goto function_exit;
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
        } // Alphabets
    }
//function_exit:
    delete[] comb;
    delete[] strRetDigistHash;

    const double end_time = omp_get_wtime();
    const double duration_time = end_time - start_time;
    MSG("------------TotalTime = " << duration_time << "sec."<< std::endl);
    return std::make_pair(duration_time, vec_pwds);
}

std::pair<double, std::vector<std::string>> Parallel_hack(const char* hash,
                                                          const size_t MinCodeLen,
                                                          const size_t MaxCodeLen,
                                                          const std::vector<std::string> &alphabets,
                                                          const uint64_t block_size)
{
    MSG("####### Parallel_hack #######" << std::endl);
    auto info = CalculateInfo(MinCodeLen, MaxCodeLen, alphabets, block_size);
    const auto numRegionsLvl2 = std::get<0>(info);
    const auto numRegionsLvl1 = 1;
    const auto numRegions = numRegionsLvl1 + numRegionsLvl2;
    const auto numCombs = std::get<1>(info);
    const auto numLostCombs = std::get<2>(info);

    numThreadsLvl1 = numThreadsLvl2;
    MSG("# numThreadsLvl1 = " << numThreadsLvl1 << std::endl);
    MSG("# numThreadsLvl2(for block_size) = " << numThreadsLvl2 << std::endl);
    const auto totalNumThreads = numThreadsLvl1 * numThreadsLvl2;
    MSG("# totalNumThreads = " << totalNumThreads << std::endl);
    MSG("#######" << std::endl);

    MSG("# numRegionsLvl2 = " << numRegionsLvl2 << std::endl);
    MSG("# numCombs = " << numCombs << std::endl);
    MSG("# lost combs = " << numLostCombs << std::endl);
    MSG("# block_size[regionCount=100] = " << numCombs / 100 << std::endl);

    MSG("########" << std::endl);
    MSG("# num idle threads(lvl1) = " << alphabets.size() % numThreadsLvl1 << std::endl);
    uint64_t totalSumCombs = 0;
    for(auto i = 0; i < alphabets.size(); ++i)
    {
        const uint64_t curMaxCombin = std::pow(alphabets[i].length(), MaxCodeLen);
        totalSumCombs += curMaxCombin;
        MSG("#comb for alphabets[" << i << "] = " << curMaxCombin << std::endl);
        MSG("# lost combs = " << curMaxCombin % block_size << std::endl);
        MSG("# num idle threads(lvl2) = " << block_size % numThreadsLvl2 << std::endl);
        //MSG("# block_size[regionCount=100] = " << curMaxCombin / 100 << std::endl;
    }

#ifdef PRINT_ITER_LOG
    static size_t PrintInterval = 100;
    if((MaxCodeLen > 5) && (numCombs > 1000)){
        double print_pwr = std::log10(numCombs);
        PrintInterval = std::pow(10, static_cast<size_t>(print_pwr-1));
    }
    MSG("# PrintInterval = " << PrintInterval << std::endl);
#endif
    MSG("########" << "start hacking.." << std::endl);

    //int cancel_count = 0;
    const double start_time = omp_get_wtime();

    omp_set_num_threads(numThreadsLvl1);
    std::vector<std::string> vec_pwds;
    std::vector<std::string> vec_pwds_per_thread[totalNumThreads];
    char* comb = new char[MaxCodeLen * totalNumThreads];
    unsigned char* strRetDigistHash = new unsigned char[HASH_LENGTH_MAX * totalNumThreads];

    for(auto CodeLen = MinCodeLen; CodeLen <= MaxCodeLen; ++CodeLen)
    {
        #pragma omp parallel num_threads(numThreadsLvl1)
        {
        //MSG("thread #" << omp_get_thread_num() << std::endl);
        #pragma omp for //collapse(2)
        for(auto alp_num = 0; alp_num < alphabets.size(); ++alp_num)
        {
            const std::string curAlphabet = alphabets[alp_num];
            const uint64_t lenAlp = curAlphabet.length();
            const uint64_t maxCountIter = std::pow(lenAlp, CodeLen);
            const int TID_lv1 = omp_get_thread_num();
            const uint64_t offsetThreadLvl1 = TID_lv1 * numThreadsLvl2;

            const auto iterBlockMax = maxCountIter / block_size;
            for(auto iterBlock = 0; iterBlock < iterBlockMax; ++iterBlock)
            {
                //#pragma omp parallel for //num_threads(block_size)
                #pragma omp parallel for num_threads(numThreadsLvl2)
                for(auto iter = 0; iter < block_size; ++iter)
                {
                    const int TID_lv2 = omp_get_thread_num();
                    const uint64_t curTotalThreadNum = offsetThreadLvl1 + TID_lv2;
                    char* pComb = comb + MaxCodeLen * curTotalThreadNum;
                    auto pRetDigistHash = strRetDigistHash + HASH_LENGTH_MAX * curTotalThreadNum;
                    const uint64_t iteration = block_size * iterBlock + iter;
                    /*
                    #pragma omp critical
                    {
                        MSG("GET_NUM_THREADS = " << omp_get_num_threads() << "  TID_lv1=" << TID_lv1 << "  TID_lv2=" << TID_lv2 << " curAlphabet= " << curAlphabet << std::endl);
                    }
                    */
                    uint64_t tmpIter = iteration;
                    //MSG("current:");
                    for(auto len = 0; len < CodeLen; ++len)
                    //for(auto len = (CodeLen-1); len >= 0; --len)
                    {
                        const uint64_t index = tmpIter % lenAlp;
                        pComb[len] = curAlphabet[index];
                        //MSG(index);
                        tmpIter /= lenAlp;
                    }
                    //MSG(std::endl);

                    //#pragma omp critical
                    {
                      //MSG("GetNumThreads = " << omp_get_num_threads() << "  TID_lv1=" << TID_lv1 << "  TID_lv2=" << TID_lv2 << " curAlphabet= " << curAlphabet << " pComb= "<< std::string(pComb, CodeLen) << std::endl);
                    }

                    GetHash256(pComb, CodeLen, pRetDigistHash);

                    if(memcmp(hash, pRetDigistHash, HASH_LENGTH_MAX) == 0)
                    {
                        MSG(" It was found in curAlphabet[" << alp_num << "] = " << curAlphabet << std::endl);
                        vec_pwds_per_thread[curTotalThreadNum].push_back(std::string(pComb, CodeLen));
                        //#pragma omp cancel for
                        //++cancel_count;
                    }
/*
                   if(IsFound == true)
                   {
                       #pragma omp cancel for
                   }
*/
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
        } // for each alphabet
        } // #pragma omp parallel
    }
    delete[] comb;
    delete[] strRetDigistHash;

    const double end_time = omp_get_wtime();
    const double duration_time = end_time - start_time;
    MSG("------------TotalTime = " << duration_time << "sec."<< std::endl);
    //MSG("------------cancel_count = " << cancel_count <<  std::endl);
    // numThreadsLvl1 = numThreadsLvl2;
    WriteResultLog(numThreadsLvl1, numCombs, duration_time, MaxCodeLen, numRegionsLvl2, block_size, alphabets.size(), alphabets[0].length());

    for(auto i = 0; i < totalNumThreads; ++i)
    {
        if(vec_pwds_per_thread[i].size() != 0)
        {
            vec_pwds.insert(vec_pwds.cend(), vec_pwds_per_thread[i].cbegin(), vec_pwds_per_thread[i].cend());
        }
    }
    return std::make_pair(duration_time, vec_pwds);
}

int main(int argc, char* argv[])
{
    omp_set_nested(1);
    //omp_set_max_active_levels(2);
    const int ALPHABET_SET_MAX_COUNT = 100;
    const char DEFAULT_ALPHABET[ALPHABET_SET_MAX_COUNT] = "123";
    const int PASSWORD_LENGTH_MIN = 1;
    const int PASSWORD_LENGTH_MAX = 10;

    std::string hash, password;
    size_t MaxCodeLen = 0, MinCodeLen = 0;
    std::vector<std::string> alphabets;
    uint64_t block_size = 1;
    bool test_mode = false;

    MSG("Welcome to decoder!" << std::endl);
    switch(argc)
    {
        case 1:{
            MSG("Give me following:" << std::endl);
            MSG(std::setw(5) << "1. " <<  "sha256 hash" << " :string" << std::endl);
            MSG(std::setw(5) << "2. " <<  "MinCodeLen" << " :int" << std::endl);
            MSG(std::setw(5) << "3. " <<  "MaxCodeLen" << " :int" << std::endl);
            MSG(std::setw(5) << "4. " <<  "abspath to alphabets" << " :string" << std::endl);
            MSG(std::setw(5) << "5. " <<  "test-mode(run sequence alg also)" << " :string" << std::endl);
            MSG(std::setw(5) << "6. " <<  "numThreadsLvl1" << " :int" << std::endl);
            MSG(std::setw(5) << "7. " <<  "numThreadsLvl2" << " :int" << std::endl);
            MSG(std::setw(5) << "8. " <<  "block_size" << " :int" << std::endl);
            MSG( "####################" << std::endl);
            return 1;
        }
        case 9:{
            block_size = std::stoull(argv[(8)]);
            if(block_size <= 0)
            {
                block_size = 1;
            }                
        }
        case 8:{
            numThreadsLvl2 = std::stoull(argv[(7)]);
            if(numThreadsLvl2 <= 0)
            {
                numThreadsLvl2 = 1;
            }                
        }
        case 7:{
            numThreadsLvl1 = std::stoull(argv[(6)]);
            if(numThreadsLvl1 <= 0)
            {
                numThreadsLvl1 = 1;
            }
        }
        case 6:{
            if(strcmp(argv[(5)], "test-mode") == 0)
            {
                test_mode = true;
            }
        }
        case 5:{
            //alphabet = std::string(argv[(4)]);
            //if(strlen(argv[(4)])+1 >  ALPHABET_SET_MAX_COUNT)
            //{
            //    alphabet = std::string(argv[(4)]).substr(0, ALPHABET_SET_MAX_COUNT);
            //}
            std::ifstream myfile(argv[(4)]);
            if(myfile.is_open())
            {
                std::string line;
                while (!myfile.eof())
                {
                    std::getline(myfile, line);
                    if(!line.empty()){
                        alphabets.push_back(line);
                    }
                }
                myfile.close();
            }else{
                alphabets.push_back(DEFAULT_ALPHABET);
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
    MSG("    Alphabets for search [" << alphabets.size() << "]: " << std::endl);
    for(auto i = 0; i < alphabets.size(); ++i)
    {
        MSG("Alphabet[" << i << "]: " << alphabets[i] << std::endl);
    }
    MSG("   end Alphabets"<< std::endl);
    MSG("    block_size: " << block_size << std::endl);
    MSG("    test_mode: " << std::boolalpha << test_mode << std::endl);


    MSG(" Let's hack the hash! " << std::endl);
    MSG(" Please wait .." << std::endl << std::endl);
    std::string digistHash = HexToDigit(hash);
    auto ret_pair = Parallel_hack(digistHash.c_str(), MinCodeLen, MaxCodeLen, alphabets, block_size);
    const double parallTimeWork = ret_pair.first;
    std::vector<std::string> vect_pwdParall = ret_pair.second;
    bool IsFound = !vect_pwdParall.empty();
    if(IsFound){
        MSG(" Successful!" << std::endl);

        MSG(" Parallel_hack found the passwords: " << std::endl);
        for(auto i = 0; i < vect_pwdParall.size(); ++i)
        {
            MSG("passwordPar[" << i << "]: " << vect_pwdParall[i] << std::endl);
        }

#ifndef PRINT_LOG
        for(auto i = 0; i < vect_pwdParall.size(); ++i)
        {
            std::cout << vect_pwdParall[i] << std::endl;
        }
#endif
    }else{
        MSG(" Not Found!" << std::endl);
    }

    if(test_mode){
        MSG(" Please wait .." << std::endl << std::endl);
        ret_pair = Sequent_hack(digistHash.c_str(), MinCodeLen, MaxCodeLen, alphabets);
        const double sequentTimeWork = ret_pair.first;
        std::vector<std::string> vect_pwdSequent = ret_pair.second;
        IsFound = !vect_pwdSequent.empty();
        if(IsFound){
            MSG(" Successful!" << std::endl);

            MSG(" Sequent_hack found the passwords: " << std::endl);
            for(auto i = 0; i < vect_pwdSequent.size(); ++i)
            {
                MSG("passwordSeq[" << i << "]: " << vect_pwdSequent[i] << std::endl);
            }
        }else{
            MSG(" Not Found!" << std::endl);
        }

        MSG(std::endl << " Check the results ...");
        std::unordered_set<std::string> passSeqSet(vect_pwdSequent.cbegin(), vect_pwdSequent.cend());
        bool isOneFound = false;
        for(auto item : vect_pwdParall)
        {
            if(passSeqSet.count(item) == 0)
            {
                MSG(item << " - not found in sequent hack." << std::endl);
            }
            else
            {
                isOneFound = true;
            }
        }

        if (isOneFound)
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
