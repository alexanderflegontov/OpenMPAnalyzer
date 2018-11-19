/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

/**
 *
 * @author HP
 */
    
    public final class SettingWindow{
////////////// size of general Window //////////////
        ///public static final int globalW = 850;
        ///public static final int globalH = 720;
        
        ///private static final int W = 850;
        ///private static final int H = 720;
        

////////////// internalWindow //////////////
        public static final int W = 690; // Size internal window
        public static final int H = 300;//300
        
        public static final int LEFT_INDENT_X = 110; // left offset internalWindow
        public static final int TOP_INDENT_Y = 300;  // top offset internalWindow

        public static final int RIGHT_INDENT_X = LEFT_INDENT_X/2;   // right offset internalWindow
        public static final int BOTTOM_INDENT_Y = TOP_INDENT_Y/2;   // bottom offset internalWindow
////////////// Size Canvas //////////////
        public static final int WIDTH = W +(LEFT_INDENT_X+RIGHT_INDENT_X);
        public static final int HEIGHT = H +(TOP_INDENT_Y+BOTTOM_INDENT_Y); 
//////////////  Scale line //////////////      
        public static final int OFFSET_Y_SCALE_LINE = 0;//27; // offset Y for Scale line
        public static final int SCALE_LINE_HALF_H = 3;
        public static final int COUNT_SCALE_LINE = 5;
        public static final int INTERVAL = (W/COUNT_SCALE_LINE);
        public static final int OFFSET_Y_NUMERICAL_VALUES = 20;// offset Y (scale time)
        public static final double COEF_FOR_OFFSET_X_TIMESTAMPS = (0.5*10.5);
        
        public static final double INTERVAL_EDGE_STROKE_WIDTH = 2;
        public static final double H_LINE_EDGE_STROKE_WIDTH   = 2;
        public static final double V_LINE_EDGE_STROKE_WIDTH   = 1.5;

        public static final int SCROLL_BAR_WIDTH = 19;        
        public static final int COUNT_SCALE_LINE_BETWEEN = 5;
        public static final int INTERVAL_BETWEEN = INTERVAL/COUNT_SCALE_LINE_BETWEEN;
        public static final int SCALE_LINE_HALF_H_BETWEEN = SCALE_LINE_HALF_H/2;
////////////// for drawing of diagram inside internalWindow //////////////
        public static final int H_STAT_TH   = 40;//50;//Math.max(HEIGHT/10, 40); // it is height of the rects and space above it.
        public static final int H_STAT_RECT = (int)(H_STAT_TH*0.625);//35; //it must be  < H_STAT_TH, it is height of the rects.
        public static final int OFFSET_TEXT = 5; // offsetX thread(0x0942) from edge of the canvas.
////////////// for sections of diagram //////////////
        public static final int OFFSET_Y_SECTION_LABEL = (int)((H_STAT_TH - H_STAT_RECT)*0.3);// offset Y (Section Label)
        public static final int OFFSET_Y_SECTION_EDG_TIMESTAMPS = 20; // offset Y for hidden numerical values of section edge 
////////////// FONTS  //////////////       
        public static final Font THREAD_NUM_FONT      = Font.font("Arial", FontWeight.BOLD, 14);
        public static final Font SCALE_VALUE_FONT     = Font.font("Tahoma", FontWeight.BOLD, 14);
        public static final Font TEXT_FONT            = Font.font("Times New Roman", 14);
        public static final Font NOT_SUCCESSFULL_FONT = Font.font("BlackArial", FontWeight.BOLD, 48);
        public static final Font TIME_STAMP_FONT      = Font.font("Tahoma", FontWeight.BOLD, 18);

    };
    
    
