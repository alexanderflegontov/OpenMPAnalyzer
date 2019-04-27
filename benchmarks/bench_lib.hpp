#ifndef __BENCH_LIB_HPP__
#define __BENCH_LIB_HPP__

//#include <windows.h>
#include <openssl/sha.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
//#include <fstream>
#include "omp.h"

const int HASH_LENGTH_MAX = SHA256_DIGEST_LENGTH;

#define PRINT_LOG
//#define PRINT_ITER_LOG

#ifdef PRINT_LOG
#define MSG(str) do { std::cout << str ; } while( false )
#else
#define MSG(str) do { } while ( false )
#endif

bool CheckValidHash(std::string const &hash)
{
    bool bOk = false;
    if((!hash.empty()) && (hash.length() % 2 == 0))
    {
        auto isxDigest = [](char ch){ return !isxdigit(ch); };
        bOk = std::find_if(hash.begin(), hash.end(), isxDigest) == hash.end();
        if(!bOk)
        {
            MSG("The hash has non-hexagonal char(s)!" << std::endl);
        }
    }
    else
    {
        MSG("The hash hasn't odd length!" << std::endl);
    }
    return bOk;
}

void PrintSpeedUpResults(double sequentTimeWork, double parallTimeWork)
{
    MSG("\n Who is faster? ...");
    if (parallTimeWork < sequentTimeWork)
        MSG(" Parallel algorithm" << std::endl);
    else
        MSG(" Sequential algorithm" << std::endl);

    //std::cout.precision(3);
    //std::cout.setf(std::ios::fixed);
    MSG("# sequentTimeWork: "  << sequentTimeWork << std::endl << "# parallTimeWork: " <<  parallTimeWork << std::endl);
    MSG("# Speedup: "  << sequentTimeWork / parallTimeWork << std::endl);
}

/* oldest code
inline void GetHexHash256(const char comb[], const int len, char * retval)
{
    unsigned char digest[HASH_LENGTH_MAX];
 
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digest, &ctx);
 
    for (int i = 0; i < HASH_LENGTH_MAX; ++i)
        sprintf(&retval[i*2], "%02x", (unsigned int)digest[i]);
    //MSG("SHA256_v2 digest: " << retval << " - " << digest << std::endl);
}

inline std::string GetHexHash256(const char comb[], const int len)
{
    unsigned char digest[HASH_LENGTH_MAX];
 
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digest, &ctx);
 
    std::stringstream ss;
    for (int i = 0; i < HASH_LENGTH_MAX; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    //MSG("SHA256 hexstring: " << ss.str() << " - " << digest << std::endl);
    return ss.str();
}
*/

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
    //MSG("SHA256 hexstring: " << strHex << " to digest: " << strDigit << std::endl);
    return strDigit;
}

inline void getComb(char* pComb, int tmpIter, const int CodeLen, const int lenAlp, const std::string& curAlphabet)
{
    //MSG("current:");
    for(auto len = 0; len < CodeLen; ++len)
    //for(len = (CodeLen-1); len >= 0; --len)
    {
        const uint64_t index = tmpIter % lenAlp;
        pComb[len] = curAlphabet[index];
        //MSG(index);
        tmpIter /= lenAlp;    
    }
    //MSG(std::endl);
}

inline void GetHash256(const char comb[], const int len, unsigned char* digest)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digest, &ctx);
}

inline std::string GetHash256(const char comb[], const int len)
{
    unsigned char digitHash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, comb, len);
    SHA256_Final(digitHash, &ctx);
    std::string str = std::string(reinterpret_cast<const char*>(digitHash), SHA256_DIGEST_LENGTH);
    return str;
}

inline std::string DigitToHexString(const unsigned char* digest)
{
    std::stringstream ss;
    for (int i = 0; i < HASH_LENGTH_MAX; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    //MSG("SHA256 hexstring: " << ss.str() << " - " << digest << std::endl);
    return ss.str(); 
}

inline void DigitToHexString(const unsigned char* digest, char* retval)
{
   for (int i = 0; i < HASH_LENGTH_MAX; ++i)
        sprintf(&retval[i*2], "%02x", static_cast<int>(digest[i]));
    //MSG("SHA256 hexstring: " << retval << " - " << digest << std::endl);
}

inline void CurrentPrint(const int CodeLen, const int iter, const int maxCountIter, time_t start_time)
{
    const double spentTime = (omp_get_wtime()-start_time);
    const double DoneInPercent = (double(iter)/maxCountIter)*100.;
    MSG("CodeLen=" << CodeLen 
    << std::setw(3) << " iter= " << iter 
    << "[" << DoneInPercent << "%]" 
    << std::setw(3) << " elapsed " << spentTime << "sec. " 
    << std::setw(3) << " remaining " << ((100.0-DoneInPercent)*(spentTime/DoneInPercent))/(3600.) << " hours "
    <<  std::endl);
}

inline void CurrentPrintParallel(const int CodeLen, const int iter, const int maxCountIter, time_t start_time)
{
    const double spentTime = (omp_get_wtime()-start_time);
    const size_t payloadPerThread = maxCountIter/omp_get_num_threads();
    const size_t ownTidIteration = iter % payloadPerThread; 
    const double DoneInPercent = (ownTidIteration/static_cast<double>(payloadPerThread))*100.;
    MSG("CodeLen=" << CodeLen 
    << std::setw(3) << " iter= " << iter 
    << "[" << DoneInPercent << "%]" 
    << std::setw(3) << " elapsed " << spentTime << "sec. " 
    << std::setw(3) << " remaining " << ((100.0-DoneInPercent)*(spentTime/DoneInPercent))/(3600.) << " hours "
    <<  std::endl);
}

std::tuple<uint64_t, uint64_t, uint64_t> CalculateInfo(const size_t MinCodeLen,
                                                       const size_t MaxCodeLen,
                                                       std::string const &alphabet,
                                                       const uint64_t block_size)
{
    uint64_t numRegions = 0;
    uint64_t numCombs = 0;
    uint64_t numLostCombs = 0;
    const size_t lenAlp = alphabet.length();
    for(auto CodeLen = MinCodeLen; CodeLen <= MaxCodeLen; ++CodeLen)
    {
        const uint64_t currentMaxNumIters = pow(lenAlp, CodeLen);
        numRegions += currentMaxNumIters / block_size;
        numCombs += currentMaxNumIters;
        numLostCombs += currentMaxNumIters % block_size;
    }
    return std::make_tuple(numRegions, numCombs, numLostCombs);
}

std::tuple<uint64_t, uint64_t, uint64_t> CalculateInfo(const size_t MinCodeLen,
                                                       const size_t MaxCodeLen,
                                                       const std::vector<std::string> &alphabets,
                                                       const uint64_t block_size)
{
    uint64_t numRegionsLvl2 = 0;
    uint64_t numCombs = 0;
    uint64_t numLostCombs = 0;
    for(auto CodeLen = MinCodeLen; CodeLen <= MaxCodeLen; ++CodeLen)
    {
        for(auto alp_num = 0; alp_num < alphabets.size(); ++alp_num)
        {
            const std::string& curAlphabet = alphabets[alp_num];
            const uint64_t lenAlp = curAlphabet.length();

            const uint64_t currentMaxNumIters = pow(lenAlp, CodeLen);
            numRegionsLvl2 += currentMaxNumIters / block_size;
            numCombs += currentMaxNumIters;
            numLostCombs += currentMaxNumIters % block_size;
        }
    }
    return std::make_tuple(numRegionsLvl2, numCombs, numLostCombs);
}

#endif // __BENCH_LIB_HPP__
