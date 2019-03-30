/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import javafx.scene.paint.Color;

/**
 *
 * @author hp
 */

// Each function is associated with a special color
public final class MappingFuncNames {
    public static final String START = "_Start";
    public static final String END   = "_End";
    public static final String BARRIER   = "Barrier";
    public static final String GET_WTIME   = "Get_wtime";
    public static final String DELIMITER = "_";            

    // paired functions
    public static final String MAIN = "Main"; 
    public static final String PARALLEL = "Parallel"; 
    public static final String PAYLOADFUNC = "PayloadFunc"; 
    //public static final String BARRIER = "Barrier"; 
    public static final String SINGLE = "Single"; 
    public static final String ATOMIC = "Atomic"; 
    public static final String CRITIICAL = "Critic"; 
    public static final String LOOP_ORDER = "Loop_order"; 
    public static final String ORDERED = "Ordered"; 
    public static final String SECTIONS = "Sections"; 
    public static final String LOCK = "Lock"; 

    public static final Color[] FUNC_RECT_COLOR = {Color.BLUE, Color.RED, Color.GREEN, Color.BLACK, 
                               Color.YELLOW, Color.ORANGE, Color.CYAN, Color.MAGENTA,
                               Color.LIGHTGRAY, Color.DARKGREY, Color.PINK };

    public static final String[] FUNC_NAME = {MAIN, PARALLEL, PAYLOADFUNC, BARRIER, 
                               SINGLE, ATOMIC, CRITIICAL, LOOP_ORDER, 
                               ORDERED, SECTIONS, LOCK};


    public static final int NUM_FUNC = FUNC_NAME.length;

    public static final boolean IS_MAPPING_VALID = FUNC_NAME.length == FUNC_RECT_COLOR.length;


    public static final Map<String,Integer> hashTableThreadName = new HashMap<>();
    static {
        for(int index = 0; index < NUM_FUNC; index++) {
            hashTableThreadName.put(FUNC_NAME[index], (Integer)index);
        }

        System.out.println("IS_MAPPING_VALID = " + String.valueOf(IS_MAPPING_VALID));
        if(IS_MAPPING_VALID == false) {
            System.exit(1);
        }
        if(MappingFuncNames.NUM_FUNC != hashTableThreadName.size()) {
            System.out.println("MAPPING is INVALID!");
            System.exit(1);
        }
    }

    // O(1)
    public static final Integer GetIndex(String funcName) {
        return hashTableThreadName.get(funcName);
    }

    // O(1)
    public static final String GetFuncNameIndex(int FuncIndex) {
        String funcName = null;
        if(FuncIndex < NUM_FUNC){
            funcName = MappingFuncNames.FUNC_NAME[FuncIndex];
        }
        return funcName;
    }

    // O(1)
    public static final Color GetColorForFuncIndex(int FuncIndex) {
        Color c = null;
        if(FuncIndex < MappingFuncNames.FUNC_RECT_COLOR.length) {
            c = MappingFuncNames.FUNC_RECT_COLOR[FuncIndex];
        }
        return c;
    }

    /// It is mapping names and output strings
    public static final String[]  LOG_OPER = {
        "Parallel_End",  // GOMP_parallel_end
        "Parallel_Start",  // GOMP_parallel

        "Atomic_Start",  // GOMP_atomic_start
        "Atomic_End",  // GOMP_atomic_end
        "Barrier",  // GOMP_barrier
        "Critic_Start",  // GOMP_critical_start
        "Critic_End",  // GOMP_critical_end

        "Loop_order_Start",  // GOMP_loop_ordered_static_start
        "Loop_order_Next",  // GOMP_loop_ordered_static_next
        "Loop_End",  // GOMP_loop_end
        "Ordered_Start",  // GOMP_ordered_start
        "Ordered_End",  // GOMP_ordered_end

        "Par_sections_Start",  // GOMP_parallel_sections
        "Sections_Next",  // GOMP_sections_next
        "Sections_nowait_End",  // GOMP_sections_end_nowait

        "Get_wtime",  // omp_get_wtime
        "Init_lock",  // omp_init_lock
        "lock_Start",  // omp_set_lock
        "lock_End",  // omp_unset_lock

        "Single_Start", // GOMP_single_start    "Single_Start_Be    ",
        "Single_End", // GOMP_single_start    "Single_Start_Af    ",
        "Main_Start", // MAIN
        "Main_End", // MAIN
        "PayloadFunc_Start", // _OMP_FN
        "PayloadFunc_End", // _OMP_FN

        "Thread_Start",
        "Thread_End",
    };

    public static final Set<Integer> SET_FUNC_START = new HashSet<>();
    public static final Set<Integer> SET_FUNC_END = new HashSet<>();
    public static final Map<Integer,Integer> FUNC_TO_FUNC = new HashMap<>();
    public static final Map<String,Integer> FUNC_NAME_TO_FUNC_ID = new HashMap<>();
    public static final Map<Integer,Integer> PAIR_FUNC_ID_TO_INTERNAL_MAP_FUNC_ID = new HashMap<>();

    static {
        for(int index = 0; index < LOG_OPER.length; ++index) {
            final String curLogOper = LOG_OPER[index];
            if(curLogOper.contains(START)) {
                SET_FUNC_START.add(index);
            }else if(curLogOper.contains(END)) {
                SET_FUNC_END.add(index);
            }
            String[] ArrayStr = curLogOper.split("_");
            int ind2 = 0;
            for(; ind2 < LOG_OPER.length; ++ind2) {
                if(!(LOG_OPER[ind2].equals(curLogOper)) &&
                    LOG_OPER[ind2].contains(ArrayStr[0])) {
                    FUNC_TO_FUNC.put(index, ind2);

                    ////START//// PAIR_FUNC_ID_TO_INTERNAL_MAP_FUNC_ID
                    final String funcName = curLogOper.split("_")[0];
                    int internal_index = 0;
                    for(; internal_index < FUNC_NAME.length; ++internal_index) {
                        if(funcName.equals(FUNC_NAME[internal_index])) {
                            PAIR_FUNC_ID_TO_INTERNAL_MAP_FUNC_ID.put(index, internal_index);
                            break;
                        }
                    }
                    if(internal_index == FUNC_NAME.length) {
                        // not found in FUNC_NAME
                        //PAIR_FUNC_ID_TO_INTERNAL_MAP_FUNC_ID.put(index, internal_index);
                        // Usage in Parsing only when it is pair!
                    }
                    ////END//// PAIR_FUNC_ID_TO_INTERNAL_MAP_FUNC_ID
                    break;
                }
            }
            if(ind2 == LOG_OPER.length) {
                // pair not found
                FUNC_TO_FUNC.put(index, index);
            }

            FUNC_NAME_TO_FUNC_ID.put(curLogOper, index);
        }


        System.out.println("SET_FUNC_START = ");
        SET_FUNC_START.forEach((elem) -> {
                System.out.println(elem + " => " + LOG_OPER[elem]);
                });

        System.out.println("SET_FUNC_END = ");
        SET_FUNC_END.forEach((elem) -> {
                System.out.println(elem + " => " + LOG_OPER[elem]);
                });

        System.out.println("FUNC_TO_FUNC = ");
        for(int index = 0; index < LOG_OPER.length; ++index){
            System.out.println(index + " => " + FUNC_TO_FUNC.get(index));
        }
    }

    public static final Integer BARRIER_ID = FUNC_NAME_TO_FUNC_ID.get(BARRIER);
    public static final Integer GET_WTIME_ID = FUNC_NAME_TO_FUNC_ID.get(GET_WTIME);

    // O(1)
    public static final boolean IsSameFunction(final Integer funcId1, final Integer funcId2) {
        return FUNC_TO_FUNC.get(funcId1).equals(funcId2);
    }

    /*
    static final int GetIndex(String funcName){
        if(FUNC_NAME.length !=  FUNC_RECT_COLOR.length){
            return -1;
        }

        for(int index = 0; index < FUNC_NAME.length; index++){
            if(funcName.equals(FUNC_NAME[index])){
                return index;
            }
        }
        return -1;
    }

    public static final Color GetColorForFunc(String funcName){
        Color c;
        switch(funcName){
            case "Main": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[0];
                    break;
            case "Parallel": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[1];
                    break;
            case "PayloadFunc": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[2];
                    break;
            //case "Barrier": 
            //        c = FUNC_RECT_COLOR[3];
            //        break;
            case "Single": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[4];
                    break;
            case "Atomic": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[5];
                    break;
            case "Critic": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[6];
                    break;
            case "Loop_order": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[7];
                    break;
            case "Ordered": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[8];
                    break;
            case "Sections": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[9];
                    break;
            case "Lock": 
                    c = MappingFuncNames.FUNC_RECT_COLOR[10];
                    break;                                    
            default:
                c = Color.CYAN;
                    break;            
        }
        return c;
    }
    */
};
