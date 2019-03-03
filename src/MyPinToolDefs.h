#ifndef __MY_PIN_TOOL_DEFS_H__
#define __MY_PIN_TOOL_DEFS_H__
/*
############################
###### MyPinToolDefs.h #####
############################
*/

// The defines of the functions, that we will trace.
#define _OMP_FN                         "._omp_fn."
#define MAIN                            "main"
#define GOMP_single_start               "GOMP_single_start"


#define GOMP_atomic_start               "GOMP_atomic_start"
#define GOMP_atomic_end                 "GOMP_atomic_end"

#define GOMP_barrier                    "GOMP_barrier"
#define GOMP_critical_start             "GOMP_critical_start"
#define GOMP_critical_end               "GOMP_critical_end"

#define GOMP_loop_ordered_static_start  "GOMP_loop_ordered_static_start"
#define GOMP_loop_ordered_static_next   "GOMP_loop_ordered_static_next"
#define GOMP_loop_end                   "GOMP_loop_end"

#define GOMP_ordered_start              "GOMP_ordered_start"
#define GOMP_ordered_end                "GOMP_ordered_end"

#define GOMP_parallel_start             "GOMP_parallel"
#define GOMP_parallel_end               "GOMP_parallel_end"

#define GOMP_parallel_sections          "GOMP_parallel_sections"
#define GOMP_sections_next              "GOMP_sections_next"
#define GOMP_sections_end_nowait        "GOMP_sections_end_nowait"


#define omp_get_wtime                   "omp_get_wtime"
#define omp_init_lock                   "omp_init_lock"
#define omp_set_lock                    "omp_set_lock"
#define omp_unset_lock                  "omp_unset_lock"


const UINT32 NUM_FUNC_REPLACE_ONE = 19;
const UINT32 NUM_FUNC_REPLACE_TWO = 2; // plus _OMP_FN
const UINT32 NUM_FUNC_REPLACE = NUM_FUNC_REPLACE_ONE + NUM_FUNC_REPLACE_TWO * 2;

const UINT32 OFFSET_PARALLEL_END_F = 0; // this offset "Parallel_End  " in LOG_OPER[]
const UINT32 OFFSET_SINGLE_F = NUM_FUNC_REPLACE_ONE; // this offset "Single_Start " in LOG_OPER[]
const UINT32 OFFSET_MAIN_F = NUM_FUNC_REPLACE_ONE + 2; // this offset "Main_Start " in LOG_OPER[]
const UINT32 OFFSET_PAYLOAD_F = NUM_FUNC_REPLACE_ONE + 4; // this offset "PayloadFunc_Start " in LOG_OPER[]

// REPLACE_BEFORE_FUNC
const char* REPLACE_FUNC[] = {
GOMP_parallel_end,
GOMP_parallel_start,

GOMP_atomic_start,
GOMP_atomic_end,
GOMP_barrier,
GOMP_critical_start,
GOMP_critical_end,

GOMP_loop_ordered_static_start,
GOMP_loop_ordered_static_next,
GOMP_loop_end,
GOMP_ordered_start,
GOMP_ordered_end,

GOMP_parallel_sections,
GOMP_sections_next,
GOMP_sections_end_nowait,

omp_get_wtime,
omp_init_lock,
omp_set_lock,
omp_unset_lock,
// NUM_FUNC_REPLACE_ONE
GOMP_single_start,
GOMP_single_start,
MAIN,
MAIN,
// NUM_FUNC_REPLACE
_OMP_FN
_OMP_FN
};

/// It is mapping names and output strings
const char* LOG_OPER[] = {
"Parallel_End       ",  // GOMP_parallel_end
"Parallel_Start     ",  // GOMP_parallel

"Atomic_Start       ",  // GOMP_atomic_start
"Atomic_End         ",  // GOMP_atomic_end
"Barrier            ",  // GOMP_barrier
"Critic_Start       ",  // GOMP_critical_start
"Critic_End         ",  // GOMP_critical_end

"Loop_order_Start   ",  // GOMP_loop_ordered_static_start
"Loop_order_Next    ",  // GOMP_loop_ordered_static_next
"Loop_End           ",  // GOMP_loop_end
"Ordered_Start      ",  // GOMP_ordered_start
"Ordered_End        ",  // GOMP_ordered_end

"Par_sections_Start ",  // GOMP_parallel_sections
"Sections_Next      ",  // GOMP_sections_next
"Sections_nowait_End",  // GOMP_sections_end_nowait

"Get_wtime          ",  // omp_get_wtime
"Init_lock          ",  // omp_init_lock
"lock_Start         ",  // omp_set_lock
"lock_End           ",  // omp_unset_lock

"Single_Start       ", // GOMP_single_start    "Single_Start_Be    ",
"Single_End         ", // GOMP_single_start    "Single_Start_Af    ",
"Main_Start         ", // MAIN
"Main_End           ", // MAIN
"PayloadFunc_Start  ", // _OMP_FN
"PayloadFunc_End    ", // _OMP_FN

"Thread_Start       ",
"Thread_End         ",
};

#endif // __MY_PIN_TOOL_DEFS_H__
