#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "omp.h"
#include <fstream>
//#include <windows.h>

#include <cmath>
#include <iomanip>
#include <openssl/sha.h> 

const int ALPHABET_SET_MAX_COUNT = 100;
const int PASSWORD_LENGTH_MIN = 1;
static const int PASSWORD_LENGTH_MAX = 10;
static const int HASH_LENGTH_MAX = SHA256_DIGEST_LENGTH*2+1;
static int numThreads = 4;
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

bool Test(const std::string& pwd_sequent, const std::string& pwd_parall)
{
	bool OK = false;
	//if (pwd_sequent.compare(pwd_parall))
    if(memcmp(pwd_sequent.c_str(), pwd_parall.c_str(), strlen(pwd_sequent.c_str())) == 0)
    {
        OK = true;
    }

//std::cout << "pwd_sequent = " << pwd_sequent << " l=" << strlen(pwd_sequent.c_str()) << std::endl;
//std::cout << "pwd_parall  = " << pwd_parall << " l="<< strlen(pwd_parall.c_str()) << std::endl;
//std::cout << "pwd_compare  = " << pwd_sequent.compare(pwd_parall) << std::endl;

	std::cout << "\n Check the results ...";
	if (OK != true)
		std::cout << "Warning!!! Something went wrong." << std::endl;
	else
		std::cout << "Successfully!!!" << std::endl;

	return OK;
}

inline void PrintSpeedUpResults(double sequentTimeWork, double parallTimeWork)
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

// It requires extension of file: .rar 
inline bool CheckValidHash( const char* Hash, int hashLen )
{
    if (Hash != NULL) 
    {
       if(hashLen % 2 == 0)
       {
          return true;
       }
       else
       {
          std::cout << " HASH hasn't odd length!" << std::endl;
       }
    }
    else
    {
         std::cout << " HASH pointer is NULL!" << std::endl;
    }
    
    return false;
}

inline void GetHash256(const char comb[], const int len, char *& retval)
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    //char retval[HASH_LENGTH_MAX];
//    std::cout << "comb[" <<  len << "]: " << (unsigned char*)comb << std::endl;

    SHA256((unsigned char*)comb, len, (unsigned char*)&digest);
    for(int j = 0; j < SHA256_DIGEST_LENGTH; ++j)
         sprintf(&retval[j*2], "%02x", (unsigned int)digest[j]);
    //std::cout << "SHA256_v2 digest: " << retval << " - " << digest << std::endl;
    //return std::string(retval);    
}

inline void GetHash256_v2(const char comb[], const int len, char *& retval)
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

inline void CurrentPrint(const int CodeLen, const int iter, const int maxCountIter, time_t t1)
{
    const double time = (omp_get_wtime()-t1);
    const double DoneInPercent = (double(iter)/maxCountIter)*100.;
    std::cout << "CodeLen=" << CodeLen 
    << std::setw(3) << " iter= " << iter 
    << "[" << DoneInPercent << "%]" 
    << std::setw(3) << " elapsed " << time << "sec. " 
    << std::setw(3) << " remaining " << ((100.0-DoneInPercent)*(time/DoneInPercent))/(3600.) << " hours "
    <<  std::endl;
}

inline void CurrentPrintParallel(const int CodeLen, const int iter, const int maxCountIter, time_t t1)
{
    const double time = (omp_get_wtime()-t1);
    const int ownThreadIter = iter % (maxCountIter/omp_get_num_threads()); 
    const double DoneInPercent = (ownThreadIter/(maxCountIter/double(omp_get_num_threads())))*100.;
    std::cout << "CodeLen=" << CodeLen 
    << std::setw(3) << " iter= " << iter 
    << "[" << DoneInPercent << "%]" 
    << std::setw(3) << " elapsed " << time << "sec. " 
    << std::setw(3) << " remaining " << ((100.0-DoneInPercent)*(time/DoneInPercent))/(3600.) << " hours "
    <<  std::endl;
}

double Sequent_hack(const char* hash, std::string& pwd, const int MinCodeLen, const int MaxCodeLen, const char alphabet[], const int lenAlp)
{
    IsFound = false;
    double t2 = 0;
    double t1 = omp_get_wtime();
    char comb[PASSWORD_LENGTH_MAX];
    long long int maxCountIter;
    int CodeLen, iter, len, blockSize;
#ifdef PRINT_LOG   
    //const int PrintInterval = (MaxCodeLen > 5 && pow(lenAlp, MaxCodeLen) > 1000)?100:10;
    int tmpMaxCombin = pow(lenAlp, MaxCodeLen);
    int PrintInterval = 10;
    if(MaxCodeLen > 5 && tmpMaxCombin> 1000){
        PrintInterval = 100;
        if(tmpMaxCombin> 10000)
        {
            PrintInterval = 1000;
        }
    }    
#endif

    char* strRetHash = new char[HASH_LENGTH_MAX];
    //char strRetHash[HASH_LENGTH_MAX];

    for(CodeLen = MinCodeLen; CodeLen <= MaxCodeLen; ++CodeLen)
    {
        maxCountIter = pow(lenAlp, CodeLen);
        blockSize = maxCountIter / numThreads;
        for(iter = 0; iter < maxCountIter; ++iter)
        {
            int tmpIter = iter;
            int index;
            //std::cout << "current:";
            for(len = 0; len < CodeLen; ++len)
            //for(len = (CodeLen-1); len >= 0; --len)
            {
                index = tmpIter % lenAlp;
                comb[len] = alphabet[index];
                //std::cout << index;
                tmpIter /= lenAlp;                
            }
            //std::cout << std::endl;
            comb[CodeLen] = '\0';
            
            //std::string strRetHash = 
                GetHash256(comb,CodeLen, strRetHash);

            if(memcmp(hash, strRetHash, len) == 0) // .c_str()
            {
                t2 = omp_get_wtime() - t1;
                pwd = comb;
                CurrentPrint(CodeLen, iter, maxCountIter, t1);
                IsFound = true;
                return t2;
            }
#ifdef PRINT_LOG
            if(iter % PrintInterval == 0){
                //CurrentPrint(CodeLen, iter, maxCountIter, t1); 
            }
#endif           
        }
    }
    t2 = omp_get_wtime() - t1;
    CurrentPrint(pwd.length(), iter, maxCountIter, t1);
    return t2;
}

double Parallel_hack(const char* hash, std::string& pwd, const int MinCodeLen, const int MaxCodeLen, const char alphabet[], const int lenAlp)
{
    //BindThreadsOnCores();

    omp_set_num_threads(numThreads);
    std::cout << "Num_threads = " << numThreads << std::endl;
    IsFound = false;
    int tid_trigger = -1, save_iter = -1, cancel_count = 0;
    double trigger_time = 0, t2 = 0, endtime = 0;
    double t1 = omp_get_wtime();
    //char* strRetHash = new char[HASH_LENGTH_MAX];
    //int maxCountIter;
    int len, blockSize, iter, CodeLen;
#ifdef PRINT_LOG
    int tmpMaxCombin = pow(lenAlp, MaxCodeLen);
    int PrintInterval = 10;
    if(MaxCodeLen > 5 && tmpMaxCombin> 1000){
        PrintInterval = 100;
        if(tmpMaxCombin> 10000)
        {
            PrintInterval = 1000;
        }
    }
#endif
    
   
    for(CodeLen = MinCodeLen; !IsFound && CodeLen <= MaxCodeLen; ++CodeLen)
    {
        const int maxCountIter = pow(lenAlp, CodeLen);
        //blockSize = maxCountIter / numThreads;
        #pragma omp parallel
        {
            char* comb = new char[PASSWORD_LENGTH_MAX];
            //char comb[PASSWORD_LENGTH_MAX];
            //char strRetHash[HASH_LENGTH_MAX];
            char* strRetHash = new char[HASH_LENGTH_MAX];
                
            //std::cout << "thread #" << omp_get_thread_num() << std::endl;
            // //if(blockSize > 1000)// schedule(dynamic, 15) // num_threads(3)
            #pragma omp for lastprivate(iter) private(len) //nowait//private(iter, len, comb, strRetHash) 
            for(iter = 0; iter < maxCountIter; iter++)
            {
                int tmpIter = iter;
                //std::cout << "current:";
                for(len = 0; len < CodeLen; ++len)
                //for(len = (CodeLen-1); len >= 0; --len)
                {
                    int index = tmpIter % lenAlp;
                    comb[len] = alphabet[index];
                    //std::cout << index;
                    tmpIter /= lenAlp;                
                }
                //std::cout << std::endl;
                comb[CodeLen] = '\0';
                
                //std::string strRetHash = 
                    GetHash256(comb, CodeLen, strRetHash);
                
                if(memcmp(hash, strRetHash, len) == 0) // .c_str()
                {
                    endtime = omp_get_wtime() - t1;
                    tid_trigger = omp_get_thread_num();
                    std::cout << "--------------START_Trigger_Time[tid = " << tid_trigger << "] = " << endtime << "sec. comb=" << comb << std::endl;
                    pwd = comb;
                    IsFound = true;

                    //break;
                    save_iter = iter = maxCountIter+1;
                    trigger_time = omp_get_wtime()-t1;
                    #pragma omp cancel for
                    ++cancel_count;
                }
                
#ifdef PRINT_LOG
                #pragma omp critical
                {
                    if(iter % PrintInterval == 0){
                        //CurrentPrintParallel(CodeLen, iter, maxCountIter, t1); 
                    }
                }
#endif  
                //#pragma omp cancellation point parallel
                //std::cout << "{}thread id #" << omp_get_thread_num() << std::endl;
                //std::cout << "thread id ## " << omp_get_thread_num() << " comb = " << comb << std::endl;
            }
            #pragma omp cancel parallel
        }      
    }
    t2 = omp_get_wtime() - t1;
    std::cout << "--------------START_Trigger_Time[tid = " << tid_trigger << "] = " << trigger_time << "sec." <<  std::endl;
    std::cout << "------------TotalTime = " << t2 << "sec. \n ------------diff(spend trigger)= " << t2-trigger_time << "sec." << std::endl; 
    std::cout << "------------cancel_count = " << cancel_count <<  std::endl; 
    CurrentPrintParallel(pwd.length(), save_iter, -1, t1);
    return t2;
}


int main(int argc, char* argv[]){

    int MaxCodeLen = 0, MinCodeLen = 0;

    //std::cout.precision(3);
    //std::cout.setf(std::ios::fixed);
    omp_set_nested(2);
    
    //char* alphabet = new char[ALPHABET_SET_MAX_COUNT]; //{'1', '2', '3'};
    char alphabet[ALPHABET_SET_MAX_COUNT] = "123";
    char hash[HASH_LENGTH_MAX] = {"/0"};
    std::string password = "";

    std::cout << "Welcome to decoder!" << std::endl;
    switch(argc)
    {
        case 1:{
            std::cout << "Give me hash" << std::endl;
            return 1;
        }
        case 6:{/*
            if(!( PASSWORD_LENGTH_MIN <= std::strlen(argv[(5)]) && std::strlen(argv[(5)]) <= PASSWORD_LENGTH_MAX)){
                std::memcpy(&mask, argv[(5)], PASSWORD_LENGTH_MAX);
                mask[PASSWORD_LENGTH_MAX] = '\0';
            }else{
                std::memcpy(&mask, argv[(5)], sizeof(argv[(5)]));
            }*/
            if(strcmp(argv[(5)], "test-mode") == 0){
                test_mode = true;
            }
        }
        case 5:{
             
            if(strlen(argv[(4)])+1 >  ALPHABET_SET_MAX_COUNT){
                //alphabet = std::string(argv[(4)]).substr(0, ALPHABET_SET_MAX_COUNT);
                memcpy(alphabet, argv[(4)], ALPHABET_SET_MAX_COUNT);
                alphabet[ALPHABET_SET_MAX_COUNT] = '\0';
            }else{
                memcpy(alphabet, argv[(4)], strlen(argv[(4)])+1);
                //alphabet = std::string(argv[(4)]).c_str();
            }
        }
        case 4:{
            MaxCodeLen = atoi(argv[(3)]);
        }        
        case 3:{
            MinCodeLen = atoi(argv[(2)]);
        }        
        case 2:{
	    if(strlen(argv[(1)])+1 > HASH_LENGTH_MAX)
            {
                std::cout << "PLease input hash with length up to " << HASH_LENGTH_MAX <<" chars !" << std::endl;
	        return 1;
            }
            memcpy(hash, argv[(1)], strlen(argv[(1)]));
            break; 
        }
        default:
            break;
    }
    
    // checks
    if(!( PASSWORD_LENGTH_MIN <= MinCodeLen && MinCodeLen <= PASSWORD_LENGTH_MAX)){
        MinCodeLen = 1; 
    }
    
    if(!( PASSWORD_LENGTH_MIN <= MaxCodeLen && MaxCodeLen <= PASSWORD_LENGTH_MAX)){
        MaxCodeLen = 3; 
    }
    
    std::cout << "  Your hash " << hash << std::endl;
    std::cout << "  Your params for searching: " << std::endl;
    std::cout << "    MinCodeLen: " << MinCodeLen << std::endl;
    std::cout << "    MaxCodeLen: " << MaxCodeLen << std::endl;
    std::cout << "    Alphabet for search: " << alphabet << std::endl;
    //if(test_mode == false)
    //    std::cout << "  Mask for search: " << mask << std::endl;
    std::cout << "  test_mode: " << test_mode << std::endl;    
    
    
    // check valid hash
    if(CheckValidHash(hash, std::strlen(hash)))
    {
        std::cout << " Let's hack hash! " << std::endl;
        std::string pwdP = "";
        std::cout << " Please wait .." << std::endl << std::endl;
        double parallTimeWork = Parallel_hack(hash, pwdP, MinCodeLen, MaxCodeLen, alphabet, std::strlen(alphabet));
        if(IsFound == true){
            std::cout << " Successful!" << std::endl; 
            std::cout << " passwordP = " << pwdP << std::endl; 
        }else{
            std::cout << " Is not Found " << std::endl;
        }
        
        if(test_mode){
            std::string pwdS = "";
            std::cout << " Please wait .." << std::endl << std::endl;
            double sequentTimeWork = Sequent_hack(hash, pwdS, MinCodeLen, MaxCodeLen, alphabet, std::strlen(alphabet));
            if(IsFound == true){
                std::cout << " Successful!" << std::endl; 
                std::cout << " passwordS = " << pwdS << std::endl; 
            }else{
                std::cout << " Is not Found " << std::endl;
            }
            
            if (Test(pwdP, pwdS)){
                PrintSpeedUpResults(sequentTimeWork, parallTimeWork);
            }
        }

    }else{
        //std::cout << " error! " << std::endl;
    }

   //system("pause");
   return 0;
}

