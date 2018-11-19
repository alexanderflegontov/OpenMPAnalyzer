/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.HashMap;
import java.util.Map;
import javafx.scene.paint.Color;

/**
 *
 * @author HP
 */

// Each function is associated with a special color
public final class MappingFuncNames{
        public static final String START = "_Start";
        public static final String END   = "_End";
        public static final String BARRIER   = "Barrier";
        public static final String GET_WTIME   = "Get_wtime";
        public static final String DELIMITER = "_";            

        
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
        static{
            for(int index = 0; index < NUM_FUNC; index++){
                hashTableThreadName.put(FUNC_NAME[index], (Integer)index);
            }

            System.out.println("IS_MAPPING_VALID = " + String.valueOf(IS_MAPPING_VALID));
            if(IS_MAPPING_VALID == false){
                System.exit(1);
            }
            if(MappingFuncNames.NUM_FUNC != hashTableThreadName.size()){
                System.out.println("MAPPING is INVALID!");
                System.exit(1);
            }
        }
        
        // O(1)
        public static final Integer GetIndex(String funcName){
            return hashTableThreadName.get(funcName);
        }
           
        // O(1)
        public static final String GetFuncNameIndex(int FuncIndex){
            String funcName = null;
            if(FuncIndex < NUM_FUNC){
                funcName = MappingFuncNames.FUNC_NAME[FuncIndex];
            }
            return funcName;
        }
        
        // O(1)
        public static final Color GetColorForFuncIndex(int FuncIndex){
            Color c = null;
            if(FuncIndex < MappingFuncNames.FUNC_RECT_COLOR.length){
                c = MappingFuncNames.FUNC_RECT_COLOR[FuncIndex];
            }
            return c;
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
