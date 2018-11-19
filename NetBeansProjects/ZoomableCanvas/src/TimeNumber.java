
import java.util.ArrayList;
import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author HP
 */
    public class TimeNumber extends Group{
    ArrayList<Label> lstNumber;
    static final int PADDING = 20;

    static final int COUNT_SCALE_LINE = 5;
    static final double intervalTime = 500;
    //static final int LEFT_INDENT_X = PADDING*2;
    //static final int TOP_INDENT_Y = PADDING-10;

    static final int OFFSET_Y_SCALE_LINE = 0;
    static final int SCALE_LINE_HALF_H = 5;
    static final int INTERVAL_EDGE_STROKE_WIDTH = 1;
    


        TimeNumber(double width){
            super();

            int INTERVAL = (int) (width/COUNT_SCALE_LINE);
    
            lstNumber = new ArrayList<>();
            
            double[] value = {10.0,20.0,30.0,40.0,50.0,60.0};
            
            String[] intervalTimes = new String[value.length];
            for(int i = 0; i < intervalTimes.length; i++){
                intervalTimes[i] = String.valueOf(value[i]) + "ms";
            }
           //Legend.GetIntervalTimes(intervalTime, COUNT_SCALE_LINE);
            for(int i = 0; i <= COUNT_SCALE_LINE; i++){
                int X = i*INTERVAL;

                Line line = new Line(X, -OFFSET_Y_SCALE_LINE-SCALE_LINE_HALF_H, X, -OFFSET_Y_SCALE_LINE+SCALE_LINE_HALF_H);
                line.setStroke(Color.BLACK);
                line.setStrokeWidth(INTERVAL_EDGE_STROKE_WIDTH);
                this.getChildren().add(line);

                Label label = new Label(intervalTimes[i]);
                //label.setFont(SCALE_VALUE_FONT);
                label.setLayoutX(X );
                label.setLayoutY(line.getStartY()-15);
                this.getChildren().add(label);
                lstNumber.add(label);
            }
        }
        
        public void ShowUpdate(double start, double end){
           double delta = (end-start)/COUNT_SCALE_LINE;
           for(int i = 0; i <= COUNT_SCALE_LINE; i++){
               double curVal = start+i*delta;
               Label label = lstNumber.get(i);
               final String strFormat = "%."+String.valueOf(2)+"f";             
               String strRet = String.format(strFormat, curVal);
               label.setText(strRet+"ms");

           }

        }
    }
