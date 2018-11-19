#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "omp.h"
#include <fstream>
//#include <windows.h>

#include <cmath>
#include <iomanip>
#include <openssl/sha.h>
#include <vector>

const int ALPHABET_SET_MAX_COUNT = 100;
const int PASSWORD_LENGTH_MIN = 1;
static const int PASSWORD_LENGTH_MAX = 10;
static const int HASH_LENGTH_MAX = SHA256_DIGEST_LENGTH*2+1;
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

int mycomp(const char* one, const char* two, const int len)
{

    for(int i=0;i<len;++i)
    {
       if(one[i] != two[i])
       {
           return -1;
       }
    } 
    return 0;
}

bool Test(const std::string& pwd_parall, const std::string& pwd_sequent)
{
    bool OK = false;
    
    if(mycomp(pwd_sequent.c_str(), pwd_parall.c_str(), strlen(pwd_sequent.c_str())) == 0)
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

// 3-12 mircoseconds!
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

inline void GetHash256_v2(const char comb[], const int len, char * retval)
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

double Sequent_hack(const char* hash, std::string& pwd, const int MinCodeLen, const int MaxCodeLen, const std::vector<std::string> &alphabets)
{
    int tmpMaxCombin = std::pow(alphabets[0].length(), MaxCodeLen);
    std::cout << "# tmpMaxCombin = " << tmpMaxCombin << std::endl;
    IsFound = false;
    double t2 = 0;
    double t1 = omp_get_wtime();
    char comb[PASSWORD_LENGTH_MAX];
    long long unsigned maxCountIter;
    int CodeLen, iter, len, num_alp, lenAlp;
#ifdef PRINT_LOG   
    //const int PrintInterval = (MaxCodeLen > 5 && std::pow(lenAlp, MaxCodeLen) > 1000)?100:10;
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
            
            for(num_alp = 0; num_alp < alphabets.size(); num_alp++)
            {
                const std::string& curAlphabet = alphabets[num_alp];
                lenAlp = curAlphabet.length();
                
                maxCountIter = std::pow(lenAlp, CodeLen);
                for(iter = 0; iter < maxCountIter; ++iter)
                {
                    int tmpIter = iter;
                    int index;
                    //std::cout << "current:";
                    for(len = 0; len < CodeLen; ++len)
                    //for(len = (CodeLen-1); len >= 0; --len)
                    {
                        index = tmpIter % lenAlp;
                        comb[len] = curAlphabet[index];
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
                        std::cout << "# t2/tmpMaxCombin = " << t2/tmpMaxCombin << " [one iter time]" << std::endl;
                        return t2;
                    }
#ifdef PRINT_LOG
                    if(iter % PrintInterval == 0){
                        //CurrentPrint(CodeLen, iter, maxCountIter, t1); 
                    }
#endif
            }           
        }
    }
    t2 = omp_get_wtime() - t1;
    CurrentPrint(pwd.length(), iter, maxCountIter, t1);
    std::cout << " # t2/tmpMaxCombin" << t2/tmpMaxCombin << std::endl;
    return t2;
}

void getComb(char* pComb, int tmpIter, int CodeLen, int lenAlp, const std::string& curAlphabet)
{
    //std::cout << "current:";
    for(int len = 0; len < CodeLen; ++len)
    //for(len = (CodeLen-1); len >= 0; --len)
    {
        int index = tmpIter % lenAlp;
        pComb[len] = curAlphabet[index];
        //std::cout << index;
        tmpIter /= lenAlp;                
    }
    //std::cout << std::endl;
}		    


// one for iteration = 12-14 and 45 microseconds 
double Parallel_hack(const char* hash, std::string& pwd, const int MinCodeLen, const int MaxCodeLen, const std::vector<std::string> &alphabets, const int block_size)
{
    //BindThreadsOnCores();
    omp_set_num_threads(numThreadsLvl2);
    std::cout << "# numThreadsLvl1 = " << numThreadsLvl1 << std::endl;
    std::cout << "# numThreadsLvl2(block_size) = " << numThreadsLvl2 << std::endl;
    const int totalNumThreads = numThreadsLvl1 * numThreadsLvl2;
    std::cout << "# totalNumThreads = " << totalNumThreads << std::endl;

    std::cout << "########" << std::endl;
    int totalSumCombs = 0;
    for(int i = 0; i < alphabets.size(); ++i)
    {
       int curMaxCombin = std::pow(alphabets[i].length(), MaxCodeLen);
       totalSumCombs += curMaxCombin;
       std::cout << "#comb for alphabets[" << i << "] = " << curMaxCombin << std::endl;
    }
    std::cout << "# totalSumCombs = " << totalSumCombs << std::endl;

#ifdef PRINT_LOG
    int PrintInterval = 10;
    if(MaxCodeLen > 5 && totalSumCombs> 1000){
        PrintInterval = 100;
        if(totalSumCombs> 10000)
        {
            PrintInterval = 1000;
        }
    }
#endif

    int tid_trigger = -1, save_iter = -1, cancel_count = 0;
    double trigger_time = 0, t2 = 0, endtime = 0;
    
    double t1 = omp_get_wtime();
    IsFound = false;
    const int numALPs = alphabets.size();
    const int numThreadLvl1 = numALPs;
    //block_size = numThreadsLvl2;    
    char* comb       = new char[totalNumThreads * PASSWORD_LENGTH_MAX];
    char* strRetHash = new char[totalNumThreads * HASH_LENGTH_MAX];


    for(int CodeLen = MinCodeLen; !IsFound && CodeLen <= MaxCodeLen; ++CodeLen)
    {
	//std::cout << "thread #" << omp_get_thread_num() << std::endl;
        #pragma omp parallel for num_threads(numThreadsLvl1) //lastprivate(num_alp)  private(lenAlp) //collapse(2)
        for(int num_alp = 0; num_alp < alphabets.size(); ++num_alp)
        { 
            const std::string& curAlphabet = alphabets[num_alp];
            int lenAlp = curAlphabet.length();
            const long long unsigned  maxCountIter = std::pow(lenAlp, CodeLen);
            const int PID = omp_get_thread_num();
	    //#pragma omp critical
	    {
              //std::cout << "1 GET_NUM_THREADS = " << omp_get_num_threads() << "  TID=" << PID << " curAlphabet= "<<curAlphabet<< std::endl;
	    }

	    const int iterBlockMax = maxCountIter/block_size;
	    for(int iterBlock = 0; iterBlock < iterBlockMax; ++iterBlock)
	    {
	        #pragma omp parallel for //num_threads(block_size) // numThreadsLvl2
	        for(int iter = 0; iter < block_size; ++iter)
	        {
		    //double h1 = omp_get_wtime() - t1;
                    //const int blockOffset = PASSWORD_LENGTH_MAX * iter;
                    const long long unsigned curTotalThreadNum = PID * numThreadsLvl2 + iter;
                    char* pComb = comb + curTotalThreadNum * PASSWORD_LENGTH_MAX;
                    int tmpIter = iterBlock * block_size + iter;
		    //std::cout << "current:";
		    for(int len = 0; len < CodeLen; ++len)
		    //for(len = (CodeLen-1); len >= 0; --len)
		    {
		        int index = tmpIter % lenAlp;
		        pComb[len] = curAlphabet[index];
		        //std::cout << index;
		        tmpIter /= lenAlp;                
		    }
		    //std::cout << std::endl;
		    
                     //#pragma omp critical
		     { 
		         //std::cout << "GetNumThreads = " << omp_get_num_threads() << "  pid=" << PID << "  tid=" << omp_get_thread_num() <<" curAlphabet= "<<curAlphabet << " pComb= "<< std::string(pComb, CodeLen) << std::endl;
	             }

                    char* strRetHashTMP = strRetHash + (HASH_LENGTH_MAX * curTotalThreadNum);
		    //std::string strRetHash =

		    GetHash256(pComb, CodeLen, strRetHashTMP);		

		   if(memcmp(hash, strRetHashTMP, CodeLen) == 0) // .c_str()
		   {
		        endtime = omp_get_wtime() - t1;
		        tid_trigger = omp_get_thread_num();
		        //std::cout << "--------------START_Trigger_Time[tid = " << tid_trigger << "] = " << endtime << "sec. comb=" << comb << std::endl;
                        std::cout << " It was found in curAlphabet[" << num_alp << "] = " << curAlphabet << std::endl;   
                        pwd = std::string(pComb, CodeLen);
		        IsFound = true;
		        //break;
		        save_iter = iter = maxCountIter+1;
		        trigger_time = omp_get_wtime()-t1;
		        //#pragma omp cancel for
		        ++cancel_count;
		    }
		
                   if(IsFound == true)
                   {
                       //#pragma omp cancel for
                   }
#ifdef PRINT_LOG
		    #pragma omp critical
		    {
		        if(iter % PrintInterval == 0){
		            //CurrentPrintParallel(CodeLen, iter, maxCountIter, t1); 
		        }
		    }
#endif  
		    //double h2 = omp_get_wtime() - t1;    
		    //std::cout << "One hash iteration calcTime = " << h2 - h1 << std::endl;
	        } // block_size	
            } // iterBlockMax

/*                
            int r = maxCountIter % block_size;
                if(r!=0)
                {
			#pragma omp parallel for num_threads(r)
			for(int iter = 0; iter < r; ++iter)
			{
			    //double h1 = omp_get_wtime() - t1;
		            //const int blockOffset = PASSWORD_LENGTH_MAX * iter;
		            const long long unsigned curTotalThreadNum = PID * numThreadsLvl2 + 0;
		            char* pComb = comb + curTotalThreadNum * PASSWORD_LENGTH_MAX;
		            int tmpIter = iterBlockMax * block_size + iter;
			    //std::cout << "current:";
			    for(int len = 0; len < CodeLen; ++len)
			    //for(len = (CodeLen-1); len >= 0; --len)
			    {
				int index = tmpIter % lenAlp;
				pComb[len] = curAlphabet[index];
				//std::cout << index;
				tmpIter /= lenAlp;                
			    }
			    //std::cout << std::endl;
			    
		             //#pragma omp critical
			     { 
				 //std::cout << "GetNumThreads = " << omp_get_num_threads() << "  pid=" << PID << "  tid=" << omp_get_thread_num() <<" curAlphabet= "<<curAlphabet << " pComb= "<< std::string(pComb, CodeLen) << std::endl;
			     }

		            char* strRetHashTMP = strRetHash + (HASH_LENGTH_MAX * curTotalThreadNum);
			    //std::string strRetHash =

			    GetHash256(pComb, CodeLen, strRetHashTMP);		

			   if(memcmp(hash, strRetHashTMP, CodeLen) == 0) // .c_str()
			   {
				endtime = omp_get_wtime() - t1;
				tid_trigger = omp_get_thread_num();
				//std::cout << "--------------START_Trigger_Time[tid = " << tid_trigger << "] = " << endtime << "sec. comb=" << comb << std::endl;
		                std::cout << " It was found in curAlphabet[" << num_alp << "] = " << curAlphabet << std::endl;   
		                pwd = std::string(pComb, CodeLen);
				IsFound = true;
				//break;
				save_iter = iter = maxCountIter+1;
				trigger_time = omp_get_wtime()-t1;
				//#pragma omp cancel for
				++cancel_count;
			    }
		
		           if(IsFound == true)
		           {
		               //#pragma omp cancel for
		           }
			}
                } // r
*/


        } // alphabets.size()
      
    }
    delete[] comb;
    delete[] strRetHash;

    t2 = omp_get_wtime() - t1;
    std::cout << "--------------START_Trigger_Time[tid = " << tid_trigger << "] = " << trigger_time << "sec." <<  std::endl;
    std::cout << "------------TotalTime = " << t2 << "sec. \n ------------diff(spend trigger)= " << t2-trigger_time << "sec." << std::endl; 
    std::cout << "------------cancel_count = " << cancel_count <<  std::endl; 
    CurrentPrintParallel(pwd.length(), save_iter, -1, t1);
    
    return t2;
}


int main(int argc, char* argv[]){

    omp_set_nested(1);
    
    //omp_set_max_active_levels(2);
    const char default_alphabet[ALPHABET_SET_MAX_COUNT] = "123";
    char hash[HASH_LENGTH_MAX] = {"/0"};
    std::vector<std::string> alphabets;

    int MaxCodeLen = 0, MinCodeLen = 0;
    int block_size = 1;

    std::cout << "Welcome to decoder!" << std::endl;
    switch(argc)
    {
        case 1:{
            std::cout << "Give me hash" << std::endl;
            return 1;
        }
        case 9:{
            block_size = atoi(argv[(8)]);
            if(block_size <= 0)
            {
                block_size = 1;
            }                
        }
        case 8:{
            numThreadsLvl2 = atoi(argv[(7)]);
            if(numThreadsLvl2 <= 0)
            {
                numThreadsLvl2 = 1;
            }                
        }        
        case 7:{
            numThreadsLvl1 = atoi(argv[(6)]);
            if(numThreadsLvl1 <= 0)
            {
                numThreadsLvl1 = 1;
            }                
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
/*             
            if(strlen(argv[(4)])+1 >  ALPHABET_SET_MAX_COUNT){
                //alphabet = std::string(argv[(4)]).substr(0, ALPHABET_SET_MAX_COUNT);
                memcpy(alphabet, argv[(4)], ALPHABET_SET_MAX_COUNT);
                alphabet[ALPHABET_SET_MAX_COUNT] = '\0';
            }else{
                memcpy(alphabet, argv[(4)], strlen(argv[(4)])+1);
                //alphabet = std::string(argv[(4)]).c_str();
            }
*/

            std::ifstream myfile(argv[(4)]);
            if(myfile.is_open())
            {
                std::string line;
                while ( ! myfile.eof() )
                {
                    std::getline(myfile, line);
		    if( ! line.empty()){
                       alphabets.push_back(line);
		    }
                }
                myfile.close();
            }else{
                alphabets.push_back(default_alphabet);
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
    std::cout << "    Alphabets for search [" << alphabets.size() << "]: " << std::endl;
    for(int i = 0; i < alphabets.size(); i++)
    {
        std::cout << "Alphabet[" << i << "]: " << alphabets[i] << std::endl;
    }   
    std::cout << "   end Alphabets"<< std::endl;
    std::cout << "  block_size: " << block_size << std::endl;

    //if(test_mode == false)
    //    std::cout << "  Mask for search: " << mask << std::endl;
    std::cout << "  test_mode: " << test_mode << std::endl;    
    
    
    // check valid hash
    if(CheckValidHash(hash, std::strlen(hash)))
    {
        std::cout << " Let's hack hash! " << std::endl;
        std::string pwdP = "";
        std::cout << " Please wait .." << std::endl << std::endl;
        double parallTimeWork = Parallel_hack(hash, pwdP, MinCodeLen, MaxCodeLen, alphabets, block_size);
        if(IsFound == true){
            std::cout << " Successful!" << std::endl; 
            std::cout << " passwordP = " << pwdP << std::endl; 
        }else{
            std::cout << " Is not Found " << std::endl;
        }
        
        if(test_mode){
            std::string pwdS = "";
            std::cout << " Please wait .." << std::endl << std::endl;
            double sequentTimeWork = Sequent_hack(hash, pwdS, MinCodeLen, MaxCodeLen, alphabets);
            if(IsFound == true){
                std::cout << " Successful!" << std::endl; 
                std::cout << " passwordS = " << pwdS << std::endl; 
            }else{
                std::cout << " Is not Found " << std::endl;
		pwdS += "_ThisISSOLD";
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


