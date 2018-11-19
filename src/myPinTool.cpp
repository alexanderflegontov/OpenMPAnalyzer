// 03.04.2018

/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2017 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
#include <stdio.h>
#include "pin.H"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>

#include <string.h> // memset
#include <iomanip>
#include <map> 
#include "MyPinToolDefs.h"

BOOL isKeyADD = false;
/////// The control defines
#define PIN_MODE_PROBE
//#define THREAD_ADDITION_OUT



//==============================================================
//  Analysis Routines
//==============================================================
// Note:  threadid+1 is used as an argument to the PIN_GetLock()
//        routine as a debugging aid.  This is the value that
//        the lock is set to, so it must be non-zero.

// lock serializes access to the output file.
PIN_LOCK pinLock;


#ifdef PIN_MODE_PROBE

#define _OutFileNameProbe "myPinTool.out"
KNOB<string> KnobOutputFileProbe(KNOB_MODE_WRITEONCE, "pintool",
    "o", _OutFileNameProbe, "specify output file name");
FILE* pinProbeOutFile;



void LOG_PINTOOL(const char *fmt, ...){	
  printf("[MyPinTool]:%s \n", fmt);
}

map<int, int> numIterLoopTH_map;// th, num_iter
map<int, FILE*> OutTH_map;
static int num_threads = 0;  

string GetFilePathForTHOut(int threadid){
	return "INFO/TH"+ToString(threadid)+".out";
}

// search or open 
FILE* getThreadFile(int threadid){
        map<int, FILE*>::iterator it;
        it = OutTH_map.find(threadid);
        
        FILE* fileOutTH = NULL;
        if (it != OutTH_map.end()){
            // found
            fileOutTH = it->second;    
        }else{
            string filepath = GetFilePathForTHOut(threadid);
            fileOutTH = fopen(filepath.c_str(), "w");
            OutTH_map.insert( pair<int, FILE*>(threadid, fileOutTH) ); 
            ++num_threads;
        }
	return fileOutTH;
}

// close
void closeAllFileAllThreads(){

	string filepathConfig = "config.out";
	FILE* fileOutConfig = fopen(filepathConfig.c_str(), "w");
	fprintf(fileOutConfig, "num_threads=%d\n", (int)OutTH_map.size());
	fflush(fileOutConfig);


    for (map<int, FILE*>::iterator it = OutTH_map.begin(); it != OutTH_map.end(); ++it)
    {
	//LOG_PINTOOL("close threadFile[%d] - %p", it->first, it->second);
        cout <<"[" <<it->first <<"]" << " : " << it->second << endl;
        fclose(it->second);

	int threadid = it->first;
	string filePathTHout = GetFilePathForTHOut(threadid);
	static const char* strLineOUT =  "name_out=%s\n";
	fprintf(fileOutConfig, strLineOUT, filePathTHout.c_str());
	fflush(fileOutConfig);

	cout << "TH["<<threadid<<"]"<< " NumIter = " <<numIterLoopTH_map[threadid] << endl;
    }
}

VOID ADDLOG(OS_THREAD_ID threadid, const char* NAME_LOG_OPER){
    static const char* strLineOUT =  "TH=%u OP=%s TM=%f \n";
    double dt = PortableGetTime();
	fprintf(pinProbeOutFile, strLineOUT, threadid, NAME_LOG_OPER, dt);
	fflush(pinProbeOutFile);
    
/////         //////////////////////
        FILE* fileOutTH = getThreadFile(threadid);
        fprintf(fileOutTH, strLineOUT, threadid, NAME_LOG_OPER, dt);
        fflush(fileOutTH);
  
    if (strcmp(NAME_LOG_OPER, LOG_OPER_TWO[7])==0){ // Main_End
        closeAllFileAllThreads();
    }
    
//////////////////////////////    
}


// This routine is executed each time REPLACE_AFTER_AND_BEFORE_FUNC[numFunc] is called.
VOID AB_OMP_Function(UINT32 logFuncId)
{
	OS_THREAD_ID threadid = PIN_GetTid(); // Get system identifier of the current thread
	ADDLOG(threadid, LOG_OPER_TWO[logFuncId]);
}

// This routine is executed each time REPLACE_FUNC[NumFunc] is called.
VOID Before_OMP_Function(UINT32 logFuncId, UINT32 bAfter=0)
{
	OS_THREAD_ID threadid = PIN_GetTid();  // Get system identifier of the current thread
if(bAfter==0){
	ADDLOG(threadid, LOG_OPER[logFuncId]);
}else{
string strTMP = LOG_OPER[logFuncId];
	strTMP = "_AF"+strTMP;
	ADDLOG(threadid, strTMP.c_str());
}
}

/*
static VOID InstrumentTrace(TRACE trace, VOID *v)
{
    // Visit every basic block in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
    	if(BBL_IsCycle(bbl)){
		break;
	}
        //BBL_InsertCall(bbl, IPOINT_ANYWHERE, (AFUNPTR)addTotal, 
        //               IARG_UINT32, BBL_NumIns(bbl), 
        //               IARG_END);
        
    }
}*/

// Pin calls this function every time a new img is loaded.
// It is best to do probe replacement when the image is loaded,
// because only one thread knows about the image at this time.
VOID ImageLoad( IMG img, VOID *v )
{
	// HERE ALREADY IS MAIN THREAD!!!!!!!!!!!
    //OS_THREAD_ID threadid = PIN_GetTid();
    //cout << "[ImageLoad]:  threadid = " << threadid << endl;

    // Walk through the symbols in the symbol table.

    for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym))
    {	
        string undFuncName = PIN_UndecorateSymbolName(SYM_Name(sym), UNDECORATION_NAME_ONLY);
        //cout << "[DETECTED]: "<<undFuncName.c_str()<< endl;
if(isKeyADD){
	cout << "[ImageLoad]: Func is Detected: " << undFuncName << endl;
}    
/*
        if(undFuncName.find(_OMP_FN) != std::string::npos)
        {
            cout << "[ImageLoad]: Func is Detected: _OMP_FN == ATTENTION!!! in ImageLoad" << endl;
            cout << "Before_OMP_FN -------------- "<<undFuncName.c_str()<< endl;
            //RTN allocRtn = RTN_FindByName( img, undFuncName.c_str() );
            RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));		    
            if (RTN_Valid(allocRtn))
            {
                if(RTN_IsSafeForProbedReplacement(allocRtn))
                {
                    RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)AB_OMP_Function,
                              //IARG_FUNCARG_ENTRYPOINT_VALUE,
                               IARG_UINT32, OFFSET_PAYLOAD_F, IARG_END);

                    PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                        undFuncName.c_str(),PIN_PARG_END());

                    RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)AB_OMP_Function,
                              IARG_PROTOTYPE, proto_malloc,
                              //IARG_FUNCARG_ENTRYPOINT_VALUE, // IARG_FUNCRET_EXITPOINT_VALUE
                               IARG_UINT32, OFFSET_PAYLOAD_F+1, IARG_END);
                     //numParallelSections++;
                    cout << "[ImageLoad]: Before_OMP_FN ++++++++++++++DONE  "<<undFuncName.c_str()<< endl;
                }
                else
                {
                    cout << "[ImageLoad]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is not safe." << endl;
                }
            }
            else
            {
                cout << "[ImageLoad]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is invalid." << endl;
            }
        } else {
*/
            bool bNotFound = true;

            
            for (UINT32 iter = 0; iter < NUM_FUNC_REPLACE; ++iter){
                if(strcmp(undFuncName.c_str(), REPLACE_FUNC[iter])==0)
                {
                    //RTN allocRtn = RTN_FindByName( img, undFuncName.c_str() );
                    RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));		    
                    if (RTN_Valid(allocRtn))
                    {
                        if(RTN_IsSafeForProbedReplacement(allocRtn))
                        {
if(strcmp(undFuncName.c_str(), GOMP_parallel_end)!=0){
                              //  Find the GOMP_Function function.
                            RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)Before_OMP_Function,
                                     //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                     /*IARG_THREAD_ID,*/ IARG_UINT32, iter, IARG_UINT32,0, IARG_END);
}else{

	                    PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void*), CALLINGSTD_DEFAULT,
					undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());
			
                            RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, 						(AFUNPTR)Before_OMP_Function,
					IARG_PROTOTYPE, proto_malloc,
                                      //IARG_FUNCARG_ENTRYPOINT_VALUE, // 						IARG_FUNCRET_EXITPOINT_VALUE
                                      /*IARG_THREAD_ID,*/ IARG_UINT32, iter,IARG_UINT32, 0, IARG_END);
}
			}
                        else
                        {
                            cout << "[ImageLoad]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is not safe." << endl;
                        }
                    }
                    bNotFound = false;
                    break;
                }
            }
        

if(bNotFound){
            for (UINT32 iter = 1; iter < NUM_FUNC_REPLACE_TWO; ++iter){
                if(strcmp(undFuncName.c_str(), REPLACE_AFTER_AND_BEFORE_FUNC[iter])==0)
                {
                    RTN allocRtn = RTN_FindByName( img, undFuncName.c_str() );
                    //RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));
                    cout << "[ImageLoad]: WAY2 -------------- " << undFuncName.c_str()<< endl;				
                    if (RTN_Valid(allocRtn))
                    {
                        if(RTN_IsSafeForProbedReplacement(allocRtn))
                        {
                            RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)AB_OMP_Function,
                                      //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                      /*IARG_THREAD_ID,*/ IARG_UINT32, iter*2+OFFSET_PAYLOAD_F, IARG_END);

                            PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                                undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

                            RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)AB_OMP_Function,
                                      IARG_PROTOTYPE, proto_malloc,
                                      //IARG_FUNCARG_ENTRYPOINT_VALUE, // IARG_FUNCRET_EXITPOINT_VALUE
                                      /*IARG_THREAD_ID,*/ IARG_UINT32, iter*2+1+OFFSET_PAYLOAD_F, IARG_END);
                            cout << "[ImageLoad]: WAY2 ++++++++++++++DONE " << undFuncName.c_str()<< endl;
                        }
                        else
                        {
                            cout << "[ImageLoad]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is not safe." << endl;
                        }
                    }
                    else
                    {
                        cout << "[ImageLoad]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is invalid." << endl;
                    }
                    //bNotFound = false;
                    break;
                }
            }//for
}// if(bNotFound)


	//} // end if-else
    } // for
}
static int BBLCount = 0;
VOID BBL_COUNT(){
	BBLCount++;

	OS_THREAD_ID threadid = PIN_GetTid();
        
	map<int, int>::iterator it;
        it = numIterLoopTH_map.find(threadid);
        
	int numIterForTID = 0;
        if (it != numIterLoopTH_map.end()){
            // found
            numIterForTID = it->second;
	    numIterForTID++;
	    numIterLoopTH_map[threadid] = numIterForTID;
        }else{
		
            numIterLoopTH_map.insert( pair<int, int>(threadid, 1) ); 
            
        }
}

// Pin calls this function every time a new rtn is executed
VOID RoutineProbe(RTN rtn, VOID *v)
{
    std::string nameRTN = RTN_Name(rtn);
    //RTN_Open(rtn);
    
    ////
    //OS_THREAD_ID threadid = PIN_GetTid();
    //cout << "[RoutineProbe]:  threadid = " << threadid << endl;
    /*
if(nameRTN.find(GOMP_parallel) != std::string::npos){
cout << "[RoutineProbe]: Func is Detected: GOMP_parallel == ATTENTION!!! in RoutineProbe" << endl;
}*/
    ////
    
    if(nameRTN.find(_OMP_FN) != std::string::npos)
    {
	//cout << "[RoutineProbe]: RTN name = "<< RTN_Name(rtn) << endl;
	//cout << "[RoutineProbe]: SEC name = "<< SEC_Name(RTN_Sec(rtn)) << endl;
	//cout << "[RoutineProbe]: IMG name = "<< IMG_Name(SEC_Img(RTN_Sec(rtn))) << endl;
    
        cout << "[RoutineProbe]: ####### Func is Detected: " << nameRTN << endl;
        RTN allocRtn = rtn;
		cout << "[RoutineProbe]: Before_OMP_FN -------------- "<<nameRTN.c_str()<< endl;
		//RTN allocRtn = RTN_FindByName( img, nameRTN.c_str() );
		//RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));		    
		if (RTN_Valid(allocRtn))
		{
			if(RTN_IsSafeForProbedReplacement(allocRtn))
			{
				 RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)AB_OMP_Function,
					      //IARG_FUNCARG_ENTRYPOINT_VALUE,
					      /*IARG_THREAD_ID,*/ IARG_UINT32, OFFSET_PAYLOAD_F, IARG_END);

           			 PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
               				 nameRTN.c_str()/*, PIN_PARG(int)*/, PIN_PARG_END());

				 RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)AB_OMP_Function,
					      IARG_PROTOTYPE, proto_malloc,
					      //IARG_FUNCARG_ENTRYPOINT_VALUE, // IARG_FUNCRET_EXITPOINT_VALUE
					      /*IARG_THREAD_ID,*/ IARG_UINT32, OFFSET_PAYLOAD_F+1, IARG_END);
				 //numParallelSections++;
				cout << "[RoutineProbe]: Before_OMP_FN ++++++++++++++ DONE "<<nameRTN.c_str()<< endl;
			} 
			else
			{
			    cout << "[RoutineProbe]: Skip replacing "<< nameRTN <<" in " << IMG_Name(SEC_Img(RTN_Sec(rtn)))<< " since it is not safe." << endl;
			}
		}
		else
		{
		    cout << "[RoutineProbe]: Skip replacing "<< nameRTN <<" in " << IMG_Name(SEC_Img(RTN_Sec(rtn))) << " since it is invalid." << endl;
		}
 /*   
RTN_Open(rtn);
    // Visit every basic block in the trace
    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
    {
	BOOL isCycle = true;//INS_IsLoopType(ins);
    	if(isCycle){
		//BBL foundBBL = bbl;
		//bbl_adr = BBL_Adress(bbl);
		 RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)BBL_COUNT, IARG_END);
		break;
	}
        //BBL_InsertCall(bbl, IPOINT_ANYWHERE, (AFUNPTR)addTotal, 
        //               IARG_UINT32, BBL_NumIns(bbl), 
        //               IARG_END);
        
    }
RTN_Close(rtn);
*/
	}

///////////here//////////
/*
    else {
    	string undFuncName = nameRTN;
    	IMG img = SEC_Img(RTN_Sec(rtn));
            bool bNotFound = true;
            for (UINT32 iter = 1; iter < NUM_FUNC_REPLACE_TWO; ++iter){
                if(strcmp(undFuncName.c_str(), REPLACE_AFTER_AND_BEFORE_FUNC[iter])==0)
                {
                    RTN allocRtn = RTN_FindByName( img, undFuncName.c_str() );
                    //RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));
                    cout << "[RoutineProbe]: WAY2 -------------- " <<endl;				
                    if (RTN_Valid(allocRtn))
                    {
                        if(RTN_IsSafeForProbedReplacement(allocRtn))
                        {
                            RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)AB_OMP_Function,
                                      //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                       IARG_UINT32, iter*2+OFFSET_PAYLOAD_F, IARG_END);

                            PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                                undFuncName.c_str(),  PIN_PARG_END());

                            RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)AB_OMP_Function,
                                      IARG_PROTOTYPE, proto_malloc,
                                      //IARG_FUNCARG_ENTRYPOINT_VALUE, // IARG_FUNCRET_EXITPOINT_VALUE
                                     
IARG_UINT32, iter*2+1+OFFSET_PAYLOAD_F, IARG_END);
                            cout << "[RoutineProbe]: WAY2 ++++++++++++++DONE "<< endl;
                        }
                        else
                        {
                            cout << "[RoutineProbe]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is not safe." << endl;
                        }
                    }
                    else
                    {
                        cout << "[RoutineProbe]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is invalid." << endl;
                    }
                    bNotFound = false;
                    break;
                }
            }
            if(bNotFound)
            for (UINT32 iter = 0; iter < NUM_FUNC_REPLACE; ++iter){
                if(strcmp(undFuncName.c_str(), REPLACE_FUNC[iter])==0)
                {
                    RTN allocRtn = RTN_FindByName( img, undFuncName.c_str() );
                    //RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));		    
                    if (RTN_Valid(allocRtn))
                    {
                        if(RTN_IsSafeForProbedReplacement(allocRtn))
                        {
                              //  Find the GOMP_Function function.
                              RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)Before_OMP_Function,
                                     //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                      IARG_UINT32, iter, IARG_END);
                        }
                        else
                        {
                            cout << "[RoutineProbe]: Skip replacing "<< undFuncName <<" in " << IMG_Name(img) << " since it is not safe." << endl;
                        }
                    }
                    break;
                }
            }
        } // end if-else
*/
/////////////here////////////////
    //RTN_Close(rtn);
}


#else /*PIN_MODE_PROBE*/   //////////////////////////////////////////////////////////////////////////////////////////////////////////////


// The running count of instructions is kept here
// make it static to help the compiler optimize docount
const  UINT8  NUM_THREADS = 255;//255;
static UINT8  curNumThreads = 0;
FILE*  OutFileTH[NUM_THREADS];
static UINT64 numParallelSections = 0;


#define _PinThreadOut "myPinTH"
#ifdef THREAD_ADDITION_OUT
#define _AddOut "myPinV4add.out"
#define _OutFileProcCount "myPinV4pc.out"

KNOB<string> KnobAddOut(KNOB_MODE_WRITEONCE, "pintool",
    "o", _AddOut, "specify output file name");
	
FILE* AddOut;
ofstream outFilePC;

static UINT64 icount = 0;
static UINT64 icountTH[NUM_THREADS];
static UINT64 threadReads[NUM_THREADS];
static UINT64 threadWrites[NUM_THREADS];

// Holds instruction count for a single procedure
typedef struct RtnCount
{
    string _name;
    string _image;
    ADDRINT _address;
    RTN _rtn;
    UINT64 _rtnCount;
    UINT64 _icount;
    struct RtnCount * _next;
} RTN_COUNT;

// Linked list of instruction counts for each routine
static RTN_COUNT * RtnList = 0;
#endif /*THREAD_ADDITION_OUT*/



VOID ADDLOG_TH(THREADID threadid, const char* NAME_LOG_OPER){
	#ifndef THREAD_ADDITION_OUT	
		fprintf(OutFileTH[threadid], "TH=%u OP=%s TM=%f \n", threadid, NAME_LOG_OPER, PortableGetTime());
	#else	
		fprintf(OutFileTH[threadid], "TH=%u OP=%s TM=%f IC=%lu RC=%lu WC=%lu RWC=%lu \n", threadid, NAME_LOG_OPER, PortableGetTime(), 
		icountTH[threadid], threadReads[threadid], threadWrites[threadid], threadReads[threadid]+threadWrites[threadid] );
	#endif /*THREAD_ADDITION_OUT*/

	fflush(OutFileTH[threadid]);
}

// Note that opening a file in a callback is only supported on Linux systems.
// See buffer-win.cpp for how to work around this issue on Windows.
//
// This routine is executed every time a thread is created.
VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    PIN_GetLock(&pinLock, threadid+1);
#ifdef THREAD_ADDITION_OUT
    fprintf(AddOut, "thread(%d) begin\n",threadid);
    fflush(AddOut);
#endif
    ++curNumThreads;
    PIN_ReleaseLock(&pinLock);


    std::string filename = _PinThreadOut + ToString(threadid) + ".out";
    KNOB<std::string> localKnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
   	     "o", filename.c_str(), "specify output file name");

    OutFileTH[threadid] = fopen(localKnobOutputFile.Value().c_str(), "w");

    ADDLOG_TH(threadid, LOG_OPER_TWO[0]);  
} 

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
#ifdef THREAD_ADDITION_OUT
    PIN_GetLock(&pinLock, threadid+1);
    fprintf(AddOut, "thread(%d) end code %d\n",threadid, code);
    fflush(AddOut);
    PIN_ReleaseLock(&pinLock);
#endif
    ADDLOG_TH(threadid, LOG_OPER_TWO[1]);
}

/////////////////////////////////////////////////////////////////////////////////

// This routine is executed each time REPLACE_AFTER_AND_BEFORE_FUNC[numFunc] is called.
VOID GOMP_FunctionAB_JIT(THREADID threadid, UINT32 numFunc)
{
	#ifdef THREAD_ADDITION_OUT
	    PIN_GetLock(&pinLock, threadid+1);
	    fprintf(AddOut, "thread(%d)  %s \n", threadid, LOG_OPER_TWO[numFunc]); //REPLACE_AFTER_AND_BEFORE_FUNC[(numFunc-OFFSET_PAYLOAD_F)/2]);
	    fflush(AddOut);
	    PIN_ReleaseLock(&pinLock);
	#endif
    if(numFunc == OFFSET_PAYLOAD_F){
	++numParallelSections;
    }		
	//cout << "GOMP_FunctionAB_JIT(): threadid ="<< threadid << " numFunc = " << numFunc <<  endl;
	ADDLOG_TH(threadid, LOG_OPER_TWO[numFunc]);
}

// This routine is executed each time REPLACE_FUNC[numFunc] is called.
VOID Before_GOMP_Function_JIT(THREADID threadid, UINT32 numFunc)
{
	#ifdef THREAD_ADDITION_OUT
	    PIN_GetLock(&pinLock, threadid+1);
	    fprintf(AddOut, "thread(%d)  %s \n", threadid, REPLACE_FUNC[numFunc]);
	    fflush(AddOut);
	    PIN_ReleaseLock(&pinLock);
	#endif
	//cout << "Before_GOMP_Function_JIT(): threadid ="<< threadid << " numFunc = " << numFunc <<  endl;   
	ADDLOG_TH(threadid, LOG_OPER[numFunc]);
}

/////////////////////////////////////////////////////////////////////////////////

// This routine is executed once at the end.
VOID Fini(INT32 code, VOID *v)
{
#ifdef THREAD_ADDITION_OUT
    fprintf(AddOut,"Number of threads:%u\n", curNumThreads);
    fprintf(AddOut,"Total number of instructions:%lu\n", icount);

    UINT64 totalReads=0, totalWrites=0, totalReadsWrites=0;
    for(UINT8 numTh=0; numTh < curNumThreads; numTh++){
      fprintf(AddOut,"TH(%u): RC=%lu WC=%lu RWC=%lu \n", numTh,
        threadReads[numTh], threadWrites[numTh], threadReads[numTh]+threadWrites[numTh]);
      totalReads  += threadReads[numTh];
      totalWrites += threadWrites[numTh];
      //fprintf(OutFileTH[0],"Thread(%u) number of instructions:%lu\n", numTh, icountTH[numTh]);
    }
    totalReadsWrites = totalReads+totalWrites;    
    fprintf(AddOut,"total: RC=%lu WC=%lu RWC=%lu \n", totalReads, totalWrites, totalReadsWrites);

    fflush(AddOut);
    fclose(AddOut);

    fprintf(OutFileTH[0],"Total number of instructions:%lu\n", icount);
#endif /*THREAD_ADDITION_OUT*/
    
    fprintf(OutFileTH[0],"\nNumber of threads:%u\n", static_cast<unsigned>(curNumThreads));

#ifdef THREAD_ADDITION_OUT    
    for(UINT8 numTh=0; numTh < curNumThreads; numTh++){	
	fprintf(OutFileTH[0], "TH(%u): IC=%lu RC=%lu WC=%lu RWC=%lu \n", static_cast<unsigned>(numTh), 
		icountTH[numTh], threadReads[numTh], threadWrites[numTh], threadReads[numTh]+threadWrites[numTh] );
    }    	
    fprintf(OutFileTH[0], "total:  IC=%lu RC=%lu WC=%lu RWC=%lu \n", icount, totalReads, totalWrites, totalReadsWrites);
#endif /*THREAD_ADDITION_OUT*/

    fprintf(OutFileTH[0], "numParallelSections = %lu\n", numParallelSections);

    for(INT8 numTh=0; numTh < curNumThreads; numTh++){
                fflush(OutFileTH[numTh]);
		fclose(OutFileTH[numTh]);
    }

#ifdef THREAD_ADDITION_OUT
    outFilePC.open(_OutFileProcCount);

    outFilePC << setw(23) << "Procedure" << " "
          << setw(15) << "Image" << " "
          << setw(18) << "Address" << " "
          << setw(12) << "Calls" << " "
          << setw(12) << "Instructions" << endl;

    for (RTN_COUNT * rc = RtnList; rc; rc = rc->_next)
    {
        if (rc->_icount > 0)
            outFilePC << setw(23) << rc->_name << " "
                  << setw(15) << rc->_image << " "
                  << setw(18) << hex << rc->_address << dec <<" "
                  << setw(12) << rc->_rtnCount << " "
                  << setw(12) << rc->_icount << endl;
    }

    outFilePC.close();
#endif /*THREAD_ADDITION_OUT*/
}


#ifdef THREAD_ADDITION_OUT

// This function is called before every block
// Use the fast linkage for calls
VOID PIN_FAST_ANALYSIS_CALL DoCount(ADDRINT c, THREADID threadid) { 
	icount+= c;
	if(threadid < NUM_THREADS) {
	  icountTH[threadid]+= c;
	}
	else {
	  fprintf(AddOut, "threadid >= %u\n", NUM_THREADS);
          fflush(AddOut);
	}
}
    
// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Insert a call to docount for every bbl, passing the number of instructions.
        // IPOINT_ANYWHERE allows Pin to schedule the call anywhere in the bbl to obtain best performance.
        // Use a fast linkage for the call.
        BBL_InsertCall(bbl, IPOINT_ANYWHERE, AFUNPTR(DoCount), IARG_FAST_ANALYSIS_CALL, IARG_UINT32, BBL_NumIns(bbl), IARG_THREAD_ID, IARG_END);
    }
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr, THREADID threadid)
{
    ++threadReads[threadid];
    //fprintf(trace,"%p: R %p\n", ip, addr);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr, THREADID threadid)
{
    ++threadWrites[threadid];
    //fprintf(trace,"%p: W %p\n", ip, addr);
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
		IARG_THREAD_ID,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
		IARG_THREAD_ID,
                IARG_END);
        }
    }
}



// This function is called before every instruction is executed
VOID Rtn_docount(UINT64 * counter)
{
    (*counter)++;
}

const char * StripPath(const char * path)
{
    const char * file = strrchr(path,'/');
    if (file)
        return file+1;
    else
        return path;
}
#endif /*THREAD_ADDITION_OUT*/

// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v)
{
#ifdef THREAD_ADDITION_OUT    
    // Allocate a counter for this routine
    RTN_COUNT * rc = new RTN_COUNT;

    // The RTN goes away when the image is unloaded, so save it now
    // because we need it in the fini
    rc->_name = RTN_Name(rtn);

    rc->_image = StripPath(IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
    rc->_address = RTN_Address(rtn);
    rc->_icount = 0;
    rc->_rtnCount = 0;

    // Add to list of routines
    rc->_next = RtnList;
    RtnList = rc;
            
    RTN_Open(rtn);

             
    // Insert a call at the entry point of a routine to increment the call count
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)Rtn_docount, IARG_PTR, &(rc->_rtnCount), IARG_END);
    
    // For each instruction of the routine
    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
    {
        // Insert a call to docount to increment the instruction counter for this rtn
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)Rtn_docount, IARG_PTR, &(rc->_icount), IARG_END);
    }

      std::string nameRTN = rc->_name;
#else     
      std::string nameRTN = RTN_Name(rtn);
      RTN_Open(rtn);
#endif /*THREAD_ADDITION_OUT*/

    if(nameRTN.find(_OMP_FN) != std::string::npos)
    {
         RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)GOMP_FunctionAB_JIT,
                      //IARG_FUNCARG_ENTRYPOINT_VALUE,
                      IARG_THREAD_ID, IARG_UINT32, OFFSET_PAYLOAD_F, IARG_END);

         RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)GOMP_FunctionAB_JIT,
                      //IARG_FUNCARG_ENTRYPOINT_VALUE, // IARG_FUNCRET_EXITPOINT_VALUE
                      IARG_THREAD_ID, IARG_UINT32, OFFSET_PAYLOAD_F+1, IARG_END);
         //numParallelSections++;
    }
    else 
    {
		bool bNotFound = true;
		for (UINT32 iter = 1; iter < NUM_FUNC_REPLACE_TWO; ++iter){
			if(strcmp(nameRTN.c_str(), REPLACE_AFTER_AND_BEFORE_FUNC[iter])==0)
			{
				RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)GOMP_FunctionAB_JIT,
                      //IARG_FUNCARG_ENTRYPOINT_VALUE,
                      IARG_THREAD_ID, IARG_UINT32, 2*iter +OFFSET_PAYLOAD_F, IARG_END);

				RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)GOMP_FunctionAB_JIT,
                      //IARG_FUNCARG_ENTRYPOINT_VALUE, // IARG_FUNCRET_EXITPOINT_VALUE
                      IARG_THREAD_ID, IARG_UINT32, 2*iter+1+OFFSET_PAYLOAD_F, IARG_END);
					 
				bNotFound = false;
				break;
			}
		}
		if(bNotFound)
		for (UINT32 iter = 0; iter < NUM_FUNC_REPLACE; ++iter){
			if(strcmp(nameRTN.c_str(), REPLACE_FUNC[iter])==0)
			{
				//  Find the REPLACE_FUNC[iter] function.
				RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)Before_GOMP_Function_JIT,
						 //IARG_FUNCARG_ENTRYPOINT_VALUE,
						 IARG_THREAD_ID, IARG_UINT32, iter, IARG_END);
				 break;
			}
		}
    }

    RTN_Close(rtn);
}

#endif /*THREAD_ADDITION_OUT*/


/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of malloc calls in the guest application\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}



/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
int main(INT32 argc, CHAR **argv)
{
/*
cout << "argc = " << argc << endl;
for(int i =0;i< argc; i++){
	cout << "argc[" << i << "]" << " = " << argv[i] << endl;
}
cout << endl;
*/
if(argc>7){
	isKeyADD = (strcmp(argv[5],"add")==0)?true:false;
	cout << "Addition out is "<< isKeyADD << endl;
}
    // Initialize pin
    if(PIN_Init(argc, argv));//return Usage();
    PIN_InitSymbols();

    // Initialize the pin lock
    PIN_InitLock(&pinLock);
    
#ifndef PIN_MODE_PROBE

#ifdef THREAD_ADDITION_OUT   
    AddOut = fopen(KnobAddOut.Value().c_str(), "w");

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    INS_AddInstrumentFunction(Instruction, 0);
#endif /*THREAD_ADDITION_OUT*/

    // Register Routine to be called to instrument rtn
    RTN_AddInstrumentFunction(Routine, 0);


    // Register Analysis routines to be called when a thread begins/ends
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
#else /*PIN_MODE_PROBE*/
	
    pinProbeOutFile = fopen(_OutFileNameProbe, "w");

  // only for _OMP_FN
    RTN_AddInstrumentFunction(RoutineProbe, 0);

    // Register ImageLoad to be called when an image is loaded
    IMG_AddInstrumentFunction(ImageLoad, 0 );

    //TRACE_AddInstrumentFunction(InstrumentTrace, 0);

    // Start the program in probe mode, never returns
    PIN_StartProgramProbed();
#endif /*PIN_MODE_PROBE*/
	
    return 0;
}
