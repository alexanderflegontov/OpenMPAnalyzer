/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.security.InvalidParameterException;
import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import static javafxapplication2.SettingWindow.TIME_STAMP_FONT;
import static javafxapplication2.SettingWindow.OFFSET_Y_SECTION_LABEL;
import static javafxapplication2.SettingWindow.OFFSET_Y_SCALE_LINE;
import static javafxapplication2.SettingWindow.OFFSET_Y_SECTION_EDG_TIMESTAMPS;

/**
 *
 * @author HP
 */
public class Section extends Rectangle{
        SectionInfo secInfo;
        
        private boolean isClicked;
        private Label sectionLabel;
        private final boolean isSection;
        //private double prevWidth;

        
        private Label startTimeLabel;
        private Label endTimeLabel;
        private Line startTimeLine;
        private Line endTimeLine;
        

        public static final int ONE_FUNC_WIDTH = 5;
        private static final int BIG_OBJ_THRESHOLD = 2;
        private static final int WIDTH_FOR_SMALL_SECTION = 2;

                
                
        //public final Point2D smallObjScaleValue = new Point2D(5, 1.2);
        private static final int SCALE_ENLARGEMENT_WIDTH = 15;
        private static final int SCALE_ENLARGEMENT_HEIGHT = 15;
        private static final float REDUCE_SCALE_EFFECT_W = 0.5f;
        
        private static final double SECTION_EDGE_STROKE_WIDTH = 2.5;
        private static final Color START_END_TIME_LINE_STROKE_COLOR = Color.BLACK;
        private static final int WIDTH_THRESHOLD_TWO_ONE = 50;

        private static final int  SECTION_LABEL_WIDTH = 200;
        
        private static final Color SECTION_LABEL_TEXT_COLOR = Color.BLACK;
        private static final Font SECTION_LABEL_FONT   = Font.font("BlackArial", FontWeight.BOLD, 24);

        public static void handler(){
            StatTable.ShowParSection(1);
        }
                
        Section(SectionInfo secInfo, boolean isSec){
            super();
            this.secInfo = secInfo;
            sectionLabel = null;
            
            startTimeLabel  = endTimeLabel = null;
            startTimeLine = endTimeLine = null;
            isSection = isSec;
            
            //prevWidth = -1;
            isClicked = false;
            this.setStrokeWidth(0);
        }
        
        Section(SectionInfo secInfo, int X, int Y, int width, int height, boolean isSec){
            super(X, Y, width, height);

            this.secInfo = secInfo;
            sectionLabel = null;


            startTimeLabel  = endTimeLabel = null;
            startTimeLine = endTimeLine = null;
            isSection = isSec;

            //prevWidth = width;
            isClicked = false;
            this.setStrokeWidth(0);
        }

        public double GetStartTime(){
            return secInfo.startTime;
        }        

        public double GetEndTime(){
            return secInfo.endTime;
        }
        
        public String GetFuncName(){
            return secInfo.GetFuncName();//  // MappingFuncNames.GetFuncNameIndex(
        }
        
        public int GetFuncNameId(){
            return secInfo.funcNameId;
        }

        
        public void SetSectionIndex(int sectionInd){
            secInfo.SetSectionIndex(sectionInd);
        }
        
        public int GetSectionIndex(){
            return secInfo.GetSectionIndex();
        }
        
        public int GetNestedLevel(){
            return secInfo.numNestedLevel;
        }
        
        public Label GetSectionLabel(){
            return sectionLabel;
        }
        
        public static final int AntiSmallSections(int widthDeltaSpentTime){
            int returnSectionWidth = widthDeltaSpentTime;
            if(widthDeltaSpentTime <= BIG_OBJ_THRESHOLD){
                returnSectionWidth = WIDTH_FOR_SMALL_SECTION;
            }
            return returnSectionWidth;
        }
                
        private boolean isBigSection(){
            return (this.getWidth() > WIDTH_THRESHOLD_TWO_ONE && isSection == true);
        }
        
        public void CreateLabel(Group root){
            int Y = (int) (this.getY()-OFFSET_Y_SECTION_LABEL);
            if(!this.isBigSection()){
                Y = (int) (this.getY() +this.getHeight()+OFFSET_Y_SECTION_LABEL);
            }
            
            sectionLabel = new Label(this.GetFuncName());
            sectionLabel.setFont(SECTION_LABEL_FONT);
            sectionLabel.setTextFill(SECTION_LABEL_TEXT_COLOR);
            sectionLabel.setPrefWidth(SECTION_LABEL_WIDTH);
            sectionLabel.setLayoutX(this.getX()+ this.getWidth()/2 - sectionLabel.getPrefWidth()/2); 
            sectionLabel.setLayoutY(Y);
            sectionLabel.setVisible(false);
            root.getChildren().add(sectionLabel);
            
            //CreateLabelIter(root);
        }

  
        public void CreateEDG(Group root){
            if(this.isBigSection()){
                int Y1 = (int) (this.getY()- SCALE_ENLARGEMENT_HEIGHT);
                int Y2 = (int) (this.getY() + this.getHeight()+ SCALE_ENLARGEMENT_HEIGHT/2);
                {
                    int X = (int) (this.getX()-SCALE_ENLARGEMENT_WIDTH/2);
                    startTimeLine = new Line(X, Y1, X, Y2);
                    startTimeLine.setStroke(START_END_TIME_LINE_STROKE_COLOR);
                    startTimeLine.setStrokeWidth(SECTION_EDGE_STROKE_WIDTH);
                    root.getChildren().add(startTimeLine);

                    startTimeLabel = new Label(Legend.GetFormatTime(secInfo.startTime));
                    startTimeLabel.setFont(TIME_STAMP_FONT);
                    startTimeLabel.setLayoutX(startTimeLine.getStartX());
                    startTimeLabel.setLayoutY(startTimeLine.getStartY() - OFFSET_Y_SECTION_EDG_TIMESTAMPS);
                    root.getChildren().add(startTimeLabel);
                }
                {
                    int X = (int) (this.getX()+SCALE_ENLARGEMENT_WIDTH/2+this.getWidth());
                    endTimeLine = new Line(X, Y1, X, Y2);
                    endTimeLine.setStroke(START_END_TIME_LINE_STROKE_COLOR);
                    endTimeLine.setStrokeWidth(SECTION_EDGE_STROKE_WIDTH);
                    root.getChildren().add(endTimeLine);

                    endTimeLabel = new Label(Legend.GetFormatTime(secInfo.endTime));
                    endTimeLabel.setFont(TIME_STAMP_FONT);
                    endTimeLabel.setLayoutX(endTimeLine.getEndX());
                    endTimeLabel.setLayoutY(endTimeLine.getStartY() - OFFSET_Y_SECTION_EDG_TIMESTAMPS);
                    root.getChildren().add(endTimeLabel);
                }
            }else{
                int X = (int) (this.getX()+this.getWidth()/2);
                endTimeLine = startTimeLine = new Line(X, this.getY() - OFFSET_Y_SCALE_LINE, X, this.getY() + OFFSET_Y_SCALE_LINE);
                startTimeLine.setStrokeWidth(SECTION_EDGE_STROKE_WIDTH);
                startTimeLine.setStroke(START_END_TIME_LINE_STROKE_COLOR);
                root.getChildren().add(startTimeLine);

                endTimeLabel = startTimeLabel = new Label(Legend.GetFormatTime(secInfo.startTime));
                startTimeLabel.setFont(TIME_STAMP_FONT);
                startTimeLabel.setLayoutX(startTimeLine.getStartX());
                startTimeLabel.setLayoutY(startTimeLine.getStartY() - OFFSET_Y_SECTION_EDG_TIMESTAMPS);
                root.getChildren().add(startTimeLabel);
            }
            SetShowAll(false);
        }

        private void SetScaleEffect(boolean set){
            if(set){
                if(this.getWidth()> BIG_OBJ_THRESHOLD){
                    this.setScaleX((this.getWidth()+SCALE_ENLARGEMENT_WIDTH)/this.getWidth());
                    this.setScaleY((this.getHeight()+SCALE_ENLARGEMENT_HEIGHT)/this.getHeight());
                }else{
                    //rect.setWidth(2);
                    this.setScaleX((this.getWidth()+SCALE_ENLARGEMENT_WIDTH*REDUCE_SCALE_EFFECT_W)/this.getWidth());
                    this.setScaleY((this.getHeight()+SCALE_ENLARGEMENT_HEIGHT)/this.getHeight());
                }
            }else{
                this.setScaleX(1);
                this.setScaleY(1);
            }
        }

        public boolean SetShowAll(boolean set){
            boolean bRetPrevValue = isClicked;
            isClicked = set;
            sectionLabel.setVisible(set);
            startTimeLabel.setVisible(set);
            endTimeLabel.setVisible(set);
            startTimeLine.setVisible(set);
            endTimeLine.setVisible(set);

            this.SetScaleEffect(set);
            return bRetPrevValue;
        }
        
        public void InvertShowAll(){
            SetShowAll(!isClicked);
        }
}




class SectionInfo {               
        public final int threadId;
        public final int funcNameId;

        public final double startTime;
        public final double endTime;       
        public final int numNestedLevel;

        private int sectionIndex;
        
        SectionInfo(final int tid, final int funcNameId, final double startTime, final double endTime, final int nestedLevel){
            CheckValidSectionData(funcNameId, startTime, endTime);
            
            this.threadId = tid;
            this.funcNameId = funcNameId;
            this.startTime = startTime;
            this.endTime = endTime;
            this.numNestedLevel = nestedLevel;
            
            this.sectionIndex = -1;
        }
        
        public String GetFuncName(){
            return MappingFuncNames.GetFuncNameIndex(funcNameId);
        }
        
        public void SetSectionIndex(final int sectionInd){
            sectionIndex = sectionInd;
        }
        
        public int GetSectionIndex(){
            return sectionIndex;
        }
        
        public static void CheckValidSectionData(int thID, double start, double end){
            if((thID < 0 || start < 0 || end < 0))
                throw new InvalidParameterException("INVALID (thID < 0 || start < 0 || end < 0) for thID = "+thID);
                //return true;

            if((end-start)<0){
                System.out.println("!!!!!!!!!! INVALID ((end-start)<0) for thID = "+thID+" break!!!!!!!!!!!!!!!!!!!!!");
                throw new InvalidParameterException("INVALID ((end-start)<0) for thID = "+thID);   
                //return null;//break;
            }

            //return false;
        }
        
}