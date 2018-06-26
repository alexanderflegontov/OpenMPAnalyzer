/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;


import java.util.HashSet;
import javafx.geometry.Insets;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.scene.layout.TilePane;
import javafx.scene.layout.HBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

/**
 *
 * @author HP
 */

public class Legend extends VBox{
    
        public static final String[] UNIT = {"ms", "s", "m", "h"};
        public static final double[] SCALAR_FACT = {1000.0, 1.0, 1/60.0, 1/3600.0};

        private static final int SQUARE_WH = 10;
        private static final int MAX_NUM_FUNC_IN_LEGEND = MappingFuncNames.NUM_FUNC;//4;
        private static final Color LEGEND_SQUARE_SCOLOR = Color.BLACK;
        private static final Font LEGEND_FUNC_NAME_FONT = Font.font("Arial", FontWeight.BOLD, 14);
        private static final Color FUNC_NAME_LABEL_TEXT_COLOR = Color.BLACK;
        
        private static final int HBOX_SPASING = SQUARE_WH/2;
        private static final Insets HBOX_INSERTS = new Insets(5,5,5,5);
        private static final Insets TILEPANE_INSERTS = new Insets(5,20,5,20);
        
        private HashSet<Integer> functionsLegendHashSet;
        private TilePane legendRoot;
        
        private static final String LEGEND_TILE_PANE_STYLE = "-fx-background-color: rgb(240,240,240);"
                + "-fx-border-color: rgb(0,0,0);"
                + "-fx-border-width: 1;";
            
        public static int GetPrecision(double DoubleNumber){
            int precision = 0; 
            for(; precision <=1; precision++){
                DoubleNumber *= 10;
                if((int)(DoubleNumber) % 10 == 0){
                    break;
                }
            }
            return precision;
        }

        public static String[] GetIntervalTimes(double intervalTime, int countScaleLine){
            //return GetIntervalTimes(intervalTime, countScaleLine, 0);
            if(intervalTime <= 0.0){
                return null;
            }else{
                String[] strRet = new String[countScaleLine+1];

                int k = GetUnit(intervalTime);

                double num = (intervalTime*1)*SCALAR_FACT[k];
                int prec = GetPrecision(num);
                String strFormat = "%."+String.valueOf(prec)+"f";             

                for(int i = 0; i <= countScaleLine; i++){
                    strRet[i] = String.format(strFormat,(intervalTime*i)*SCALAR_FACT[k])+UNIT[k];
                }

                return strRet;   
            }
        }

        public static String[] GetIntervalTimes(double intervalTime, int countScaleLine, double startTime){
            if(intervalTime <= 0.0){
                return null;
            }else{
                String[] strRet = new String[countScaleLine+1];

                int k = GetUnit(intervalTime);

                double num = (intervalTime*1)*SCALAR_FACT[k];
                int prec = GetPrecision(num);
                String strFormat = "%."+String.valueOf(prec)+"f";             

                for(int i = 0; i <= countScaleLine; i++){
                    strRet[i] = String.format(strFormat,(intervalTime*i+startTime)*SCALAR_FACT[k])+UNIT[k];
                }

                return strRet;   
            }
        }
        
        public static String GetFormatTime(double time){
            return GetFormatTime(time, GetUnit(time));
        }
        
        public static String GetFormatTime(double time, int unit){  
            //return GetFormatTimeOnly()+UNIT[unit];
            if(time < 0.0){
                return null;
            }else{
                double num = (time)*SCALAR_FACT[unit];
                int prec = GetPrecision(num);
                final String strFormat = "%."+String.valueOf(prec)+"f";             

                String strRet = String.format(strFormat, num)+UNIT[unit];
                return strRet;   
            }
        }
        
        public static String GetFormatTimeOnly(double time, int unit){  
            if(time < 0.0){
                return null;
            }else{
                double num = (time)*SCALAR_FACT[unit];
                int prec = GetPrecision(num);
                final String strFormat = "%."+String.valueOf(prec)+"f";             

                String strRet = String.format(strFormat, num);
                return strRet;   
            }
        }

        public static String GetFormatTimeOnly(double time, int unit, int precision){  
            if(time < 0.0){
                return null;
            }else{
                double num = (time)*SCALAR_FACT[unit];
                //int prec = GetPrecision(num);
                final String strFormat = "%."+String.valueOf(precision)+"f";             

                String strRet = String.format(strFormat, num);
                return strRet;   
            }
        }
        
        public static int GetUnit(double time){
            int k = 1;
            if(time < 0.0){
                return k;
            }else{
                if (time < 1.0){
                    // mseconds 
                    k = 0;
                }else if (time < 600.0){
                    // seconds if interval of time < 10 min
                    k = 1;
                }else if (time < 3600.0){
                    // minutes if interval of time < 1 hour
                    k = 2;
                }else {//if (intervalTime < 3600.0){
                    // hours if interval of time < 1 hour
                    k = 3;
                }
                return k;   
            }
        }
        
    public Legend(HashSet<Integer> functionsLegend_HashSet){
        legendRoot = new TilePane();
        this.getChildren().add(legendRoot);
        
        legendRoot.setStyle(LEGEND_TILE_PANE_STYLE);
        legendRoot.setPadding(TILEPANE_INSERTS);
        functionsLegendHashSet = functionsLegend_HashSet;       
        ShowLegend();
    }
    
    public void SetHashSet(HashSet functionsLegend_HashSet){
        functionsLegendHashSet = functionsLegend_HashSet;
    }
        
    void ShowLegend(){
        legendRoot.getChildren().clear();
        
        System.out.println("ShowLegend(): MAX_NUM_FUNC_IN_LEGEND = " + String.valueOf(MAX_NUM_FUNC_IN_LEGEND));
        int numFuncInLegend = 0;
        Integer[] arrayFuncIndex = {};
        if(functionsLegendHashSet != null){
            numFuncInLegend = functionsLegendHashSet.size();
            arrayFuncIndex = functionsLegendHashSet.toArray(new Integer[functionsLegendHashSet.size()]);
        }else{
            // else print all
            numFuncInLegend = MappingFuncNames.NUM_FUNC;
            arrayFuncIndex = MappingFuncNames.hashTableThreadName.values().toArray(new Integer[MappingFuncNames.NUM_FUNC]);
        }

        if(numFuncInLegend > MAX_NUM_FUNC_IN_LEGEND){
            System.out.println("ShowLegend(): Error, Stop !!!");
        }

        //if(numFuncInLegend > 7){
        //    legendRoot.setHgap(6);
        //}
        
        for(int i = 0; i < arrayFuncIndex.length; i++){
            int index = arrayFuncIndex[i].intValue();
            Rectangle rect = new Rectangle(0, 0, SQUARE_WH, SQUARE_WH);
            rect.setFill(MappingFuncNames.GetColorForFuncIndex(index));//MappingFuncNames.GetColorForFunc(arrayFunc[i]));//MappingFuncNames.FUNC_RECT_COLOR[key.intValue()]);//i
            rect.setStroke(LEGEND_SQUARE_SCOLOR); 

            Label label = new Label(MappingFuncNames.GetFuncNameIndex(index));//arrayFunc[i]);
            label.setFont(LEGEND_FUNC_NAME_FONT);
            label.setTextFill(FUNC_NAME_LABEL_TEXT_COLOR);

            HBox legElem = new HBox();
            legElem.setSpacing(HBOX_SPASING);
            legElem.setPadding(HBOX_INSERTS);
            legElem.getChildren().addAll(rect, label);
            legendRoot.getChildren().add(legElem);
        }
    }
}
