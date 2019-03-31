#include <iostream>
#include <cstring>
#include <cstdlib>
#include "omp.h"
#include <fstream>
//#include <windows.h>

#include <cmath>
#include <iomanip>
#include <openssl/sha.h> 
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <cctype>
//#define _GNU_SOURCE
//#include <sched.h>

const int ALPHABET_SET_MAX_COUNT = 100;
const char DEFAULT_ALPHABET[ALPHABET_SET_MAX_COUNT] = "123";
const int PASSWORD_LENGTH_MIN = 1;
static const int PASSWORD_LENGTH_MAX = 10;
static const int HASH_LENGTH_MAX = SHA256_DIGEST_LENGTH;
static int numThreadsLvl1 = 1;
static int numThreadsLvl2 = 1;
static bool IsFound = false;
static bool test_mode = false;


//#define PRINT_LOG

/*
void BindThreadsOnCores()
{
    #pragma omp parallel default(shared)
	{
		DWORD_PTR mask = (1 << omp_get_thread_num());
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}
}
*/
/*
std::cout << "sizeof(pid_t) = " << sizeof(pid_t) << std::endl;

void BindThreadsOnCores()
{
    cpu_set_t cpuset;
    CPU_ZERO(&set);
    const uint32_t numCPU = 32;
    for(auto i = 0; i < numCPU; ++i)
    {
        CPU_SET(i, &cpuset);
    }

    #pragma omp parallel default(shared)
	{
        //#define handle_error_en(en, msg) \
        //        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)
        //pthread_t thread = pthread_self();
        //int res = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
        //if(res != 0)
        //{
        //    handle_error_en(s, "pthread_setaffinity_np");
        //}

		uint32_t mask = (1 << omp_get_thread_num());
        uint64_t tid = getpid()

		sched_setaffinity(tid, sizeof(cpu_set_t), mask);
	}
}
*/

void PrintSpeedUpResults(double sequentTimeWork, double parallTimeWork)
{
	std::cout << "\n Who is faster? ...";
	if (parallTimeWork < sequentTimeWork)
		std::cout << " Parallel algorithm" << std::endl;
	else
		std::cout << " Sequential algorithm" << std::endl;

	//std::cout.precision(3);
	//std::cout.setf(std::ios::fixed);
    std::cout << "# sequentTimeWork: "  << sequentTimeWork << std::endl << "# parallTimeWork: " <<  parallTimeWork << std::endl;
	std::cout << "# Speedup: "  << sequentTimeWork / parallTimeWork << std::endl;
}

bool CheckValidHash(std::string const &hash)
{
    bool bOk = false;
    if((!hash.empty()) && (hash.length() % 2 == 0))
    {
        auto isxDigest = [](char ch){ return !isxdigit(ch); };
        bOk = std::find_if(hash.begin(), hash.end(), isxDigest) == hash.end();
        if(!bOk)
        {
            std::cout << "The hash has non-hexagonal char(s)!" << std::endl;
        }
    }
    else
    {
        std::cout << "The hash hasn't odd length!" << std::endl;
    }
    return bOk;
}

inline void GetHexHash256(const char comb[], const int len, char * retval)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
 
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digest, &ctx);
 
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        sprintf(&retval[i*2], "%02x", (unsigned int)digest[i]);
    //std::cout << "SHA256_v2 digest: " << retval << " - " << digest << std::endl;
}

inline std::string GetHexHash256(const char comb[], const int len)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
 
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digest, &ctx);
 
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    //std::cout << "SHA256 hexstring: " << ss.str() << " - " << digest << std::endl;
    return ss.str();
}

inline void GetHash256(const char comb[], const int len, unsigned char* digest)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digest, &ctx);
}

inline std::string DigitToHexString(const unsigned char* digest)
{
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    //std::cout << "SHA256 hexstring: " << ss.str() << " - " << digest << std::endl;
    return ss.str(); 
}

inline void DigitToHexString(const unsigned char* digest, char* retval)
{
   for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        sprintf(&retval[i*2], "%02x", static_cast<int>(digest[i]));
    //std::cout << "SHA256 hexstring: " << retval << " - " << digest << std::endl;
}

// HexToAscii
std::string HexToDigit(std::string const &strHex)
{
    std::string strDigit;
    size_t len = strHex.length();
    strDigit.reserve(len / 2);
    for (auto i = 0; i < len; i += 2)
    {
        std::string const byte = strHex.substr(i, 2);
        char chr = static_cast<char>(std::stoul(byte, nullptr, 16));
        strDigit.push_back(chr);
    }
    //std::cout << "SHA256 hexstring: " << strHex << " to digest: " << strDigit << std::endl;
    return strDigit;
}

inline void CurrentPrint(const int CodeLen, const int iter, const int maxCountIter, time_t start_time)
{
    const double spentTime = (omp_get_wtime()-start_time);
    const double DoneInPercent = (double(iter)/maxCountIter)*100.;
    std::cout << "CodeLen=" << CodeLen 
    << std::setw(3) << " iter= " << iter 
    << "[" << DoneInPercent << "%]" 
    << std::setw(3) << " elapsed " << spentTime << "sec. " 
    << std::setw(3) << " remaining " << ((100.0-DoneInPercent)*(spentTime/DoneInPercent))/(3600.) << " hours "
    <<  std::endl;
}

inline void CurrentPrintParallel(const int CodeLen, const int iter, const int maxCountIter, time_t start_time)
{
    const double spentTime = (omp_get_wtime()-start_time);
    const size_t payloadPerThread = maxCountIter/omp_get_num_threads();
    const size_t ownTidIteration = iter % payloadPerThread; 
    const double DoneInPercent = (ownTidIteration/static_cast<double>(payloadPerThread))*100.;
    std::cout << "CodeLen=" << CodeLen 
    << std::setw(3) << " iter= " << iter 
    << "[" << DoneInPercent << "%]" 
    << std::setw(3) << " elapsed " << spentTime << "sec. " 
    << std::setw(3) << " remaining " << ((100.0-DoneInPercent)*(spentTime/DoneInPercent))/(3600.) << " hours "
    <<  std::endl;
}

std::pair<double, std::string> Sequent_hack(const char* hash, const size_t MinCodeLen, const size_t MaxCodeLen, const std::vector<std::string> &alphabets)
{
    std::cout << "####### Sequent_hack #######" << std::endl;
    IsFound = false;
    std::string pwd;

    uint64_t tmpMaxCombin = 0;
    for(auto i = 0; i < alphabets.size(); ++i)
    {
        uint64_t curMaxCombin = std::pow(alphabets[i].length(), MaxCodeLen);
        tmpMaxCombin += curMaxCombin;
        //std::cout << "# comb for alphabets[" << i << "] = " << curMaxCombin << std::endl;
    }
    std::cout << "# tmpMaxCombin = " << tmpMaxCombin << std::endl;

#ifdef PRINT_LOG
    static size_t PrintInterval = 100;
    if((MaxCodeLen > 5) && (tmpMaxCombin > 1000)){
        double print_pwr = std::log10(tmpMaxCombin);
        PrintInterval = std::pow(10, static_cast<size_t>(print_pwr-1));
    }
    std::cout << "# PrintInterval = " << PrintInterval << std::endl;
#endif
    std::cout << "########" << "start hacking.." << std::endl;
    int save_iter = -1;
    double trigger_time = 0, end_time = 0;
    double start_time = omp_get_wtime();

    char* comb = new char[PASSWORD_LENGTH_MAX];
    unsigned char* strRetDigistHash = new unsigned char[HASH_LENGTH_MAX];

    for(auto CodeLen = MinCodeLen; !IsFound && CodeLen <= MaxCodeLen; ++CodeLen)
    {
        for(auto alp_num = 0; alp_num < alphabets.size(); ++alp_num)
        {
            const std::string& curAlphabet = alphabets[alp_num];
            const uint64_t lenAlp = curAlphabet.length();

            const uint64_t maxCountIter = pow(lenAlp, CodeLen);
            for(auto iter = 0; iter < maxCountIter; ++iter)
            {
                uint64_t tmpIter = iter;
                //std::cout << "current:";
                for(auto len = 0; len < CodeLen; ++len)
                //for(auto len = (CodeLen-1); len >= 0; --len)
                {
                    uint64_t index = tmpIter % lenAlp;
                    comb[len] = curAlphabet[index];
                    //std::cout << index;
                    tmpIter /= lenAlp;
                }
                //std::cout << std::endl;
                //std::cout << "Comb = " << std::string(comb, CodeLen) << std::endl;

                GetHash256(comb, CodeLen, strRetDigistHash);

                if(memcmp(hash, strRetDigistHash, HASH_LENGTH_MAX) == 0)
                {
                    trigger_time = omp_get_wtime() - start_time;
                    //std::cout << "--------------START_Trigger_Time[tid = " << trigger_tid << "] = " << endtime << "sec. comb=" << comb << std::endl;
                    pwd = std::string(comb, CodeLen);
                    IsFound = true;
                    save_iter = iter;
                    //CodeLen = MaxCodeLen + 1;
                    //break;
                    goto function_exit;
                }

#ifdef PRINT_LOG
                if(iter % PrintInterval == 0)
                {
                    std::cout << "------------iteration = " << iter << "." <<  std::endl;
                    CurrentPrint(CodeLen, iter, maxCountIter, start_time); 
                }
#endif
                //double h2 = omp_get_wtime() - start_time;
                //std::cout << "One hash iteration calcTime = " << h2 - h1 << std::endl;
            }
        } // Alphabet
    }
function_exit:
    delete[] comb;
    delete[] strRetDigistHash;

    end_time = omp_get_wtime() - start_time;
    std::cout << "------------Trigger_Time[main] = " << trigger_time << "sec." <<  std::endl;
    std::cout << "------------TotalTime = " << end_time << "sec."<< std::endl;
    std::cout << " -----------diff(TotalTime, Trigger_Time)= " << (end_time - trigger_time) << "sec." << std::endl;
    std::cout << "------------save_iter = " << save_iter <<  std::endl;
    return std::make_pair(end_time, pwd);
}

void PrintRegionAndCombCount(const size_t MinCodeLen, const size_t MaxCodeLen, const std::vector<std::string> &alphabets, const uint64_t block_size)
{
    uint64_t regionCount = 0;
    uint64_t combCount = 0;
    for(auto CodeLen = MinCodeLen; CodeLen <= MaxCodeLen; ++CodeLen)
    {
        for(auto alp_num = 0; alp_num < alphabets.size(); ++alp_num)
        {
            const std::string& curAlphabet = alphabets[alp_num];
            const uint64_t lenAlp = curAlphabet.length();

            const uint64_t maxCountIter = pow(lenAlp, CodeLen);
            combCount += maxCountIter;

            const auto iterBlockMax = maxCountIter / block_size;
            regionCount += iterBlockMax;
        }
    }
    std::cout << "# regionCount = " << regionCount << std::endl;
    std::cout << "# combCount = " << combCount << std::endl;
}

void getComb(char* pComb, int tmpIter, const int CodeLen, const int lenAlp, const std::string& curAlphabet)
{
    //std::cout << "current:";
    for(auto len = 0; len < CodeLen; ++len)
    //for(len = (CodeLen-1); len >= 0; --len)
    {
        uint64_t index = tmpIter % lenAlp;
        pComb[len] = curAlphabet[index];
        //std::cout << index;
        tmpIter /= lenAlp;    
    }
    //std::cout << std::endl;
}

std::string sha256(const char comb[], const int len)
{
    unsigned char digitHash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, comb, len);
    SHA256_Final(digitHash, &sha256);
    std::string str = std::string(reinterpret_cast<const char*>(digitHash), SHA256_DIGEST_LENGTH);
    return str;
}

// one for iteration = 12-14 and 45 microseconds
std::pair<double, std::string> Parallel_hack(const char* hash, const size_t MinCodeLen, const size_t MaxCodeLen, const std::vector<std::string> &alphabets, const uint64_t block_size)
{
    std::cout << "####### Parallel_hack #######" << std::endl;
    //BindThreadsOnCores();
    numThreadsLvl1 = numThreadsLvl2;
    omp_set_num_threads(numThreadsLvl1);
    IsFound = false;
    std::string pwd;

    std::cout << "# numThreadsLvl1 = " << numThreadsLvl1 << std::endl;
    std::cout << "# numThreadsLvl2(for block_size) = " << numThreadsLvl2 << std::endl;
    const int totalNumThreads = numThreadsLvl1 * numThreadsLvl2;
    std::cout << "# totalNumThreads = " << totalNumThreads << std::endl;

    std::cout << "#######" << std::endl;
    PrintRegionAndCombCount(MinCodeLen, MaxCodeLen, alphabets, block_size);

    std::cout << "########" << std::endl;
    std::cout << "# num idle threads(lvl1) = " << alphabets.size() % numThreadsLvl1 << std::endl;
    uint64_t totalSumCombs = 0;
    for(auto i = 0; i < alphabets.size(); ++i)
    {
        uint64_t curMaxCombin = std::pow(alphabets[i].length(), MaxCodeLen);
        totalSumCombs += curMaxCombin;
        std::cout << "#comb for alphabets[" << i << "] = " << curMaxCombin << std::endl;
        std::cout << "# lost combs = " << curMaxCombin % block_size << std::endl;
        std::cout << "# num idle threads(lvl2) = " << block_size % numThreadsLvl2 << std::endl;
        //std::cout << "# block_size[regionCount=100] = " << curMaxCombin / 100 << std::endl;
    }

#ifdef PRINT_LOG
    static size_t PrintInterval = 100;
    if((MaxCodeLen > 5) && (totalSumCombs > 1000)){
        double print_pwr = std::log10(totalSumCombs);
        PrintInterval = std::pow(10, static_cast<size_t>(print_pwr-1));
    }
    std::cout << "# PrintInterval = " << PrintInterval << std::endl;
#endif
    std::cout << "########" << "start hacking.." << std::endl;

    int trigger_tid = -1, save_iter = -1, cancel_count = 0;
    double trigger_time = 0, end_time = 0;
    double start_time = omp_get_wtime();

    char* comb = new char[PASSWORD_LENGTH_MAX * totalNumThreads];
    unsigned char* strRetDigistHash = new unsigned char[HASH_LENGTH_MAX * totalNumThreads];

    for(auto CodeLen = MinCodeLen; !IsFound && CodeLen <= MaxCodeLen; ++CodeLen)
    {
        #pragma omp parallel num_threads(numThreadsLvl1)
	    {
    	//std::cout << "thread #" << omp_get_thread_num() << std::endl;
        //BindThreadsOnCores();
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
                    char* pComb = comb + PASSWORD_LENGTH_MAX * curTotalThreadNum;
                    unsigned char* pRetDigistHashmb = strRetDigistHash + PASSWORD_LENGTH_MAX * curTotalThreadNum;
                    /*
                    #pragma omp critical
                    {
                        std::cout << "GET_NUM_THREADS = " << omp_get_num_threads() << "  TID_lv1=" << TID_lv1 << "  TID_lv2=" << TID_lv2 << " curAlphabet= " << curAlphabet << std::endl;
                    }
                    */
                    uint64_t iteration = block_size * iterBlock + iter;
                    uint64_t tmpIter = iteration;
                    //std::cout << "current:";
                    for(auto len = 0; len < CodeLen; ++len)
                    //for(auto len = (CodeLen-1); len >= 0; --len)
                    {
                        uint64_t index = tmpIter % lenAlp;
                        pComb[len] = curAlphabet[index];
                        //std::cout << index;
                        tmpIter /= lenAlp;
                    }
                    //std::cout << std::endl;

                    //#pragma omp critical
		            {
		                //std::cout << "GetNumThreads = " << omp_get_num_threads() << "  TID_lv1=" << TID_lv1 << "  TID_lv2=" << TID_lv2 << " curAlphabet= " << curAlphabet << " pComb= "<< std::string(pComb, CodeLen) << std::endl;
	                }

                    GetHash256(pComb, CodeLen, pRetDigistHashmb);

        		    //const std::string strRetHash = sha256(pComb, CodeLen);

		            //if(!strRetHash.compare(hash))
            		//if(memcmp(hash, strRetHash.c_str(), strRetHash.size()) == 0)
                    if(memcmp(hash, pRetDigistHashmb, HASH_LENGTH_MAX) == 0)
                    {
                        trigger_time = omp_get_wtime() - start_time;
                        trigger_tid = omp_get_thread_num();
                        //std::cout << "--------------START_Trigger_Time[tid = " << trigger_tid << "] = " << endtime << "sec. comb=" << comb << std::endl;
                        std::cout << " It was found in curAlphabet[" << alp_num << "] = " << curAlphabet << std::endl;   

                        pwd = std::string(pComb, CodeLen);
                        IsFound = true;
                        save_iter = iteration;
                        iter = maxCountIter+1;
                        //#pragma omp cancel for
                        ++cancel_count;
                    }

/*
                   if(IsFound == true)
                   {
                       #pragma omp cancel for
                   }
*/

    #ifdef PRINT_LOG
                    #pragma omp critical
                    {
                        if(iteration % PrintInterval == 0)
                        {
                            std::cout << "------------iteration = " << iteration << "." <<  std::endl;
                            CurrentPrintParallel(CodeLen, iteration, maxCountIter, start_time); 
                        }
                    }
    #endif
                    //double h2 = omp_get_wtime() - start_time;
                    //std::cout << "One hash iteration calcTime = " << h2 - h1 << std::endl;
                }
            }
        } // for by alphabets
        } // #pragma omp parallel
    }
    delete[] comb;
    delete[] strRetDigistHash;

    end_time = omp_get_wtime() - start_time;
    std::cout << "------------Trigger_Time[tid = " << trigger_tid << "] = " << trigger_time << "sec." <<  std::endl;
    std::cout << "------------TotalTime = " << end_time << "sec."<< std::endl;
    std::cout << " -----------diff(TotalTime, Trigger_Time)= " << (end_time - trigger_time) << "sec." << std::endl;
    std::cout << "------------save_iter = " << save_iter <<  std::endl;
    std::cout << "------------cancel_count = " << cancel_count <<  std::endl;
    return std::make_pair(end_time, pwd);
}

int main(int argc, char* argv[]){

    omp_set_nested(1);
    //omp_set_max_active_levels(2);

    std::string password, hash;
    size_t MaxCodeLen = 0, MinCodeLen = 0;
    uint64_t block_size = 1;
    std::vector<std::string> alphabets;

    std::cout << "Welcome to decoder!" << std::endl;
    switch(argc)
    {
        case 1:{
            std::cout << "Give me following:" << std::endl;
            std::cout << std::setw(5) << "1. " <<  "sha256 hash" << " :string" << std::endl;
            std::cout << std::setw(5) << "2. " <<  "MinCodeLen" << " :int" << std::endl;
            std::cout << std::setw(5) << "3. " <<  "MaxCodeLen" << " :int" << std::endl;
            std::cout << std::setw(5) << "4. " <<  "abspath to alphabets" << " :string" << std::endl;
            std::cout << std::setw(5) << "5. " <<  "test-mode(run sequence alg also)" << " :string" << std::endl;
            std::cout << std::setw(5) << "6. " <<  "numThreadsLvl1" << " :int" << std::endl;
            std::cout << std::setw(5) << "7. " <<  "numThreadsLvl2" << " :int" << std::endl;
            std::cout << std::setw(5) << "8. " <<  "block_size" << " :int" << std::endl;
            std::cout <<  "####################" << std::endl;
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
	        if(hash.length() != HASH_LENGTH_MAX*2)
            {
                std::cout << "Please input hash with length of " << HASH_LENGTH_MAX*2 << " chars!" << std::endl;
	            return 1;
            }
            break; 
        }
        default:
            break;
    }
    
    std::cout << "  Your hash " << hash << std::endl;
    std::cout << "  Your params for searching: " << std::endl;
    std::cout << "    MinCodeLen: " << MinCodeLen << std::endl;
    std::cout << "    MaxCodeLen: " << MaxCodeLen << std::endl;
    std::cout << "    Alphabets for search [" << alphabets.size() << "]: " << std::endl;
    for(auto i = 0; i < alphabets.size(); ++i)
    {
        std::cout << "Alphabet[" << i << "]: " << alphabets[i] << std::endl;
    }
    std::cout << "   end Alphabets"<< std::endl;
    //if(test_mode == false)
    //    std::cout << "  Mask for search: " << mask << std::endl;
    std::cout << "    block_size: " << block_size << std::endl;  
    std::cout << "    test_mode: " << std::boolalpha << test_mode << std::endl;    

    if(CheckValidHash(hash))
    {
        std::cout << " Let's hack the hash! " << std::endl;
        std::cout << " Please wait .." << std::endl << std::endl;
        std::string digistHash = HexToDigit(hash);
        auto ret_pair = Parallel_hack(digistHash.c_str(), MinCodeLen, MaxCodeLen, alphabets, block_size);
        double parallTimeWork = ret_pair.first;
        std::string pwdParall = ret_pair.second;
        if(IsFound){
            std::cout << " Successful!" << std::endl; 
            std::cout << " passwordPar = " << pwdParall << std::endl; 
        }else{
            std::cout << " Not Found!" << std::endl;
        }

        if(test_mode){
            std::cout << " Please wait .." << std::endl << std::endl;
            ret_pair = Sequent_hack(digistHash.c_str(), MinCodeLen, MaxCodeLen, alphabets);
            double sequentTimeWork = ret_pair.first;
            std::string pwdSequent = ret_pair.second;
            if(IsFound){
                std::cout << " Successful!" << std::endl; 
                std::cout << " passwordSeq = " << pwdSequent << std::endl; 
            }else{
                std::cout << " Not Found!" << std::endl;
		        pwdSequent += "_ThisIsSalt";
            }

            std::cout << std::endl << " Check the results ...";
            if (pwdParall.compare(pwdSequent) == 0)
            {
                std::cout << "Successfully!!!" << std::endl;
                PrintSpeedUpResults(sequentTimeWork, parallTimeWork);
            }
            else
            {
                std::cout << "Warning!!! Something went wrong." << std::endl;
            }
        }
    }else{
        //std::cout << " error! " << std::endl;
    }
   //system("pause");
   return 0;
}

