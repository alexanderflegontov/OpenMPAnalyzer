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
#include "pin.H"
#include "MyPinToolDefs.h"

#define HUMAN_STYLE
//#define INDEX_MAPPING
//#define PIN_MAIN_ARGS
#define DEBUG 0
//#define FIND_BY_NAME // else FIND_BY_ADDRESS
//#define ONE_PIN_MTHREAD_OUTFILE

#ifdef ONE_PIN_MTHREAD_OUTFILE
    FILE* pinProbeOutFile;

    #define singleFileProbe "myPinTool.out"
    KNOB<string> KnobOutputFileProbe(KNOB_MODE_WRITEONCE, "pintool",
                                    "o", singleFileProbe, "specify output file name");
#endif

//#define LOG_PINTOOL(fmt, ...)  do { if (DEBUG) printf("[MyPinTool]:%d:%s():"fmt"\n", __LINE__, __func__, __VA_ARGS__); } while (0)
#define LOG_PINTOOL(fmt, ...)  do { if (DEBUG) printf("[MyPinTool]:%d:"fmt"\n", __LINE__, __VA_ARGS__); } while (0)

// not support UINTX and INTX, only standart types.
#define ToString( x ) static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x )).str()
#define RESULT_OUT_DIR  "INFO/"
#define GetFilePathForThreadOut(threadId)  RESULT_OUT_DIR "TH" + ToString(threadId) + ".out"

#if defined(HUMAN_STYLE)
static const char* STR_LOGLINE = "TH=%u OP=%s TM=%f \n";
#elif defined(INDEX_MAPPING)
static const char* STR_LOGLINE = "%d,%f\n";
#else
static const char* STR_LOGLINE = "%s,%f\n";
#endif

 static const char* STR_CFGFILE = "config.out";
 static const char* STR_CFG_NUMTHREADS = "num_threads=%d\n";
 static const char* STR_CFGLINE = "name_out=%s\n";

// accuracy in microseconds.
// return time in seconds
double PortableGetTime()
{
#ifdef __linux__
  struct timeval t;
  memset(&t, 0x00, sizeof(t));
  (void) gettimeofday(&t, 0);
  return (t.tv_sec * 1000000ULL + t.tv_usec) * 1.0e-6;
  //struct timespec spec;
  //(void) clock_gettime(CLOCK_MONOTONIC, &spec );
  //return (spec.tv_sec * 1000000000ULL + spec.tv_nsec) * 1.0e-9;
#elif defined(_WIN32)
  LARGE_INTEGER Frequency;
  QueryPerformanceFrequency(&Frequency);

  LARGE_INTEGER t;
  QueryPerformanceCounter(&t);
  return t.QuadPart / Frequency.QuadPart;
#else
  return time(0);
#endif
}

//unordered_map is only with c++11 
map<OS_THREAD_ID, FILE*> OutTH_map;

FILE* GetThreadFile(OS_THREAD_ID threadId)
{
    FILE* threadFileOut = NULL;
    map<OS_THREAD_ID, FILE*>::iterator it = OutTH_map.find(threadId);
    if (it != OutTH_map.end())
    {
        // found
        threadFileOut = it->second;
    }
    else
    {
        string const& filepath = GetFilePathForThreadOut(threadId);
        threadFileOut = fopen(filepath.c_str(), "w");
        OutTH_map.insert( pair<OS_THREAD_ID, FILE*>(threadId, threadFileOut) );
    }
	return threadFileOut;
}

void closeAllThreadFiles()
{
	FILE* configFile = fopen(STR_CFGFILE, "w");
	fprintf(configFile, STR_CFG_NUMTHREADS, OutTH_map.size());
	fflush(configFile);

    for (map<OS_THREAD_ID, FILE*>::iterator it = OutTH_map.begin(); it != OutTH_map.end(); ++it)
    {
        fclose(it->second);
	    LOG_PINTOOL("close threadFile[%d] : %p", it->first, it->second);

        OS_THREAD_ID threadId = it->first;
        string const& threadFileOut = GetFilePathForThreadOut(threadId);
        fprintf(configFile, STR_CFGLINE, threadFileOut.c_str());
        fflush(configFile);
    }
    //fclose(configFile);
}

void ADDLOG(UINT32 logFuncId)
{
    double dt = PortableGetTime();
	OS_THREAD_ID threadId = PIN_GetTid(); // Get system identifier of the current thread

#ifndef INDEX_MAPPING
    const char* NAME_LOG_OPER = LOG_OPER[logFuncId];
#endif

#ifdef ONE_PIN_MTHREAD_OUTFILE
    #ifdef HUMAN_STYLE
    fprintf(pinProbeOutFile, STR_LOGLINE, threadId, NAME_LOG_OPER, dt);
    #elif defined(INDEX_MAPPING)
    fprintf(pinProbeOutFile, STR_LOGLINE, logFuncId, dt);
    #else
    fprintf(pinProbeOutFile, STR_LOGLINE, NAME_LOG_OPER, dt);
    #endif
	fflush(pinProbeOutFile);
#endif

    FILE* fileOutTH = GetThreadFile(threadId);
    #ifdef HUMAN_STYLE
    fprintf(fileOutTH, STR_LOGLINE, threadId, NAME_LOG_OPER, dt);
    #elif defined(INDEX_MAPPING)
    fprintf(fileOutTH, STR_LOGLINE, logFuncId, dt);
    #else
    fprintf(fileOutTH, STR_LOGLINE, NAME_LOG_OPER, dt);
    #endif
    fflush(fileOutTH);
}

// This routine is executed each time REPLACE_FUNC[numFunc] is called.
VOID OMP_Function(UINT32 logFuncId)
{
	ADDLOG(logFuncId);
}

VOID OMP_Function_MAIN_END(UINT32 logFuncId)
{
	ADDLOG(logFuncId);
    closeAllThreadFiles();
}

// Pin calls this function every time a new img is loaded.
// It is best to do probe replacement when the image is loaded,
// because only one thread knows about the image at this time.
VOID ImageLoad(IMG img, VOID *v)
{
    //LOG_PINTOOL("[ImageLoad]: threadId = %d\n", PIN_GetTid());

    string undFuncName = MAIN;
    RTN allocRtn = RTN_FindByName(img, undFuncName.c_str());
    //RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));
    //LOG_PINTOOL("[ImageLoad]: WAY2 -------------- %s\n", undFuncName.c_str());
    if (RTN_Valid(allocRtn))
    {
        if (RTN_IsSafeForProbedReplacement(allocRtn))
        {
            RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)OMP_Function,
                                //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                //IARG_THREAD_ID,
                                IARG_UINT32, OFFSET_MAIN_F,
                                IARG_END);

            PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                                    undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

            RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)OMP_Function_MAIN_END,
                                IARG_PROTOTYPE, proto_malloc,
                                //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                //IARG_THREAD_ID,
                                IARG_UINT32, OFFSET_MAIN_F+1,
                                IARG_END);
            //LOG_PINTOOL("[ImageLoad]: WAY2 ++++++++++++++DONE %s\n", undFuncName.c_str());
        }
        else
        {
            LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
        }
    }
    else
    {
        LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
    }

#ifdef FIND_BY_NAME
    undFuncName = GOMP_parallel_end;
    allocRtn = RTN_FindByName(img, undFuncName.c_str());
    if (RTN_Valid(allocRtn))
    {
        if (RTN_IsSafeForProbedReplacement(allocRtn))
        {
            PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void*), CALLINGSTD_DEFAULT,
                                    undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

            RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)OMP_Function,
                                IARG_PROTOTYPE, proto_malloc,
                                //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                //IARG_THREAD_ID,
                                IARG_UINT32, OFFSET_PARALLEL_END_F,
                                IARG_END);
        }
        else
        {
            LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
        }
    }
    else
    {
        LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
    }

    for (UINT32 iter = 1; iter < NUM_FUNC_REPLACE_ONE; ++iter)
    {
        undFuncName = REPLACE_FUNC[iter];
        allocRtn = RTN_FindByName(img, undFuncName.c_str());
        if (RTN_Valid(allocRtn))
        {
            if (RTN_IsSafeForProbedReplacement(allocRtn))
            {
                //  Find the GOMP_Function function.
                RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)OMP_Function,
                                    //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                    //IARG_THREAD_ID,
                                    IARG_UINT32, iter,
                                    IARG_END);
            }
            else
            {
                LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
            }
        }
        else
        {
            LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
        }
    }

    undFuncName = GOMP_single_start;
    allocRtn = RTN_FindByName(img, undFuncName.c_str());
    //LOG_PINTOOL("[ImageLoad]: WAY2 -------------- %s\n", undFuncName.c_str());
    if (RTN_Valid(allocRtn))
    {
        if (RTN_IsSafeForProbedReplacement(allocRtn))
        {
            RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)OMP_Function,
                                //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                //IARG_THREAD_ID,
                                IARG_UINT32, OFFSET_SINGLE_F,
                                IARG_END);

            PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                                    undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

            RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)OMP_Function,
                                IARG_PROTOTYPE, proto_malloc,
                                //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                //IARG_THREAD_ID,
                                IARG_UINT32, OFFSET_SINGLE_F+1,
                                IARG_END);
            //LOG_PINTOOL("[ImageLoad]: WAY2 ++++++++++++++DONE %s\n", undFuncName.c_str());
        }
        else
        {
            LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
        }
    }
    else
    {
        LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
    }
#else // FIND_BY_ADDRESS

    // Walk through the symbols in the symbol table.
    for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym))
    {	
        string const& undFuncName = PIN_UndecorateSymbolName(SYM_Name(sym), UNDECORATION_NAME_ONLY);

        //LOG_PINTOOL("[DETECTED]: %s\n", undFuncName.c_str());

        if (strcmp(undFuncName.c_str(), GOMP_parallel_end) == 0)
        {
            //RTN allocRtn = RTN_FindByName(img, undFuncName.c_str());
            RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));		    
            if (RTN_Valid(allocRtn))
            {
                if (RTN_IsSafeForProbedReplacement(allocRtn))
                {
                    PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void*), CALLINGSTD_DEFAULT,
                                            undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

                    RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)OMP_Function,
                                        IARG_PROTOTYPE, proto_malloc,
                                        //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                        //IARG_THREAD_ID,
                                        IARG_UINT32, OFFSET_PARALLEL_END_F,
                                        IARG_END);
                }
                else
                {
                    LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
                }
            }
            else
            {
                LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
            }
            continue;
        }

        for (UINT32 iter = 1; iter < NUM_FUNC_REPLACE_ONE; ++iter)
        {
            if (strcmp(undFuncName.c_str(), REPLACE_FUNC[iter]) == 0)
            {
                //RTN allocRtn = RTN_FindByName(img, undFuncName.c_str());
                RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));		    
                if (RTN_Valid(allocRtn))
                {
                    if (RTN_IsSafeForProbedReplacement(allocRtn))
                    {
                        //  Find the GOMP_Function function.
                        RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)OMP_Function,
                                            //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                            //IARG_THREAD_ID,
                                            IARG_UINT32, iter,
                                            IARG_END);
                    }
                    else
                    {
                        LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
                    }
                }
                else
                {
                    LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
                }
                //iter = NUM_FUNC_REPLACE;
                goto continue_for_symbols;
            }
        }

        if (strcmp(undFuncName.c_str(), GOMP_single_start) == 0)
        {
            //RTN allocRtn = RTN_FindByName(img, undFuncName.c_str());
            RTN allocRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));
            //LOG_PINTOOL("[ImageLoad]: WAY2 -------------- %s\n", undFuncName.c_str());
            if (RTN_Valid(allocRtn))
            {
                if (RTN_IsSafeForProbedReplacement(allocRtn))
                {
                    RTN_InsertCallProbed(allocRtn, IPOINT_BEFORE, (AFUNPTR)OMP_Function,
                                        //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                        //IARG_THREAD_ID,
                                        IARG_UINT32, OFFSET_SINGLE_F,
                                        IARG_END);

                    PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                                            undFuncName.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

                    RTN_InsertCallProbed(allocRtn, IPOINT_AFTER, (AFUNPTR)OMP_Function,
                                        IARG_PROTOTYPE, proto_malloc,
                                        //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                        //IARG_THREAD_ID,
                                        IARG_UINT32, OFFSET_SINGLE_F+1,
                                        IARG_END);
                    //LOG_PINTOOL("[ImageLoad]: WAY2 ++++++++++++++DONE %s\n", undFuncName.c_str());
                }
                else
                {
                    LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is not safe.\n", undFuncName.c_str(), IMG_Name(img).c_str());
                }
            }
            else
            {
                LOG_PINTOOL("[ImageLoad]: Skip replacing %s in %s since it is invalid.\n", undFuncName.c_str(), IMG_Name(img).c_str());
            }
            continue;
        }
        continue_for_symbols:;
    } // for in symbol table
#endif
}

// Pin calls this function every time a new rtn is executed
VOID RoutineProbe(RTN rtn, VOID *v)
{
    //LOG_PINTOOL("[RoutineProbe]: threadId = %d\n", PIN_GetTid());
    std::string const& nameRTN = RTN_Name(rtn);

    //RTN_Open(rtn);
    if(nameRTN.find(_OMP_FN) != std::string::npos)
    {
        //LOG_PINTOOL("[RoutineProbe]: RTN name = %s\n", nameRTN.c_str());
        //LOG_PINTOOL("[RoutineProbe]: SEC name = %s\n", SEC_Name(nameRTN.c_str()));
        //LOG_PINTOOL("[RoutineProbe]: IMG name = %s\n", IMG_Name(SEC_Img(nameRTN.c_str())));
        //LOG_PINTOOL("[RoutineProbe]: ####### Func is Detected: %s\n", nameRTN.c_str());
        //LOG_PINTOOL("[RoutineProbe]: Before_OMP_FN -------------- %s\n", nameRTN.c_str());
		if (RTN_Valid(rtn))
		{
			if(RTN_IsSafeForProbedReplacement(rtn))
			{
                RTN_InsertCallProbed(rtn, IPOINT_BEFORE, (AFUNPTR)OMP_Function,
                                    //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                    //IARG_THREAD_ID,
                                    IARG_UINT32, OFFSET_PAYLOAD_F, IARG_END);

                PROTO proto_malloc = PROTO_Allocate(PIN_PARG(void *), CALLINGSTD_DEFAULT,
                                        nameRTN.c_str(), /*PIN_PARG(int),*/ PIN_PARG_END());

                RTN_InsertCallProbed(rtn, IPOINT_AFTER, (AFUNPTR)OMP_Function,
                                    IARG_PROTOTYPE, proto_malloc,
                                    //IARG_FUNCARG_ENTRYPOINT_VALUE,
                                    //IARG_THREAD_ID,
                                    IARG_UINT32, OFFSET_PAYLOAD_F+1, IARG_END);
                //LOG_PINTOOL("[RoutineProbe]: Before_OMP_FN ++++++++++++++ DONE %s\n", nameRTN.c_str());
			}
			else
			{
                LOG_PINTOOL("[RoutineProbe]: Skip replacing %s in %s  since it is invalid.\n", nameRTN.c_str(), IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
			}
		}
		else
		{
            LOG_PINTOOL("[RoutineProbe]: Skip replacing %s in %s since it is not safe.\n", nameRTN.c_str(), IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
		}
	}
    //RTN_Close(rtn);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of OpenMP calls in the guest application\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
int main(INT32 argc, CHAR **argv)
{
#ifdef PIN_MAIN_ARGS
    printf("argc = %d\n", argc);
    for(INT32 i = 0; i < argc; ++i)
    {
        printf("argc[%d]=%s\n", i, argv[i]);
    }
    printf("\n");
#endif

    // Initialize pin
    if(PIN_Init(argc, argv)) return Usage();

    PIN_InitSymbols();

#ifdef ONE_PIN_MTHREAD_OUTFILE
    pinProbeOutFile = fopen(singleFileProbe, "w");
#endif

    // only for _OMP_FN
    RTN_AddInstrumentFunction(RoutineProbe, 0);

    // Register ImageLoad to be called when an image is loaded
    IMG_AddInstrumentFunction(ImageLoad, 0);

    // Start the program in probe mode, never returns
    PIN_StartProgramProbed();

    return 0;
}
