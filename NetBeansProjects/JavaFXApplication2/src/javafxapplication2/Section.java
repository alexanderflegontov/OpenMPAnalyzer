/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import java.security.InvalidParameterException;

import javafxapplication2.DataAnalyzer.Mark;
import static javafxapplication2.SettingWindow.H_STAT_RECT;
import static javafxapplication2.SettingWindow.TIME_STAMP_FONT;
import static javafxapplication2.SettingWindow.OFFSET_Y_SECTION_LABEL;
import static javafxapplication2.SettingWindow.OFFSET_Y_SCALE_LINE;
import static javafxapplication2.SettingWindow.OFFSET_Y_SECTION_EDG_TIMESTAMPS;
import static javafxapplication2.SettingWindow.W;

/**
 * Interface for a section
 * @author hp
 */
interface ISection {

    public double GetStartTime();

    public double GetEndTime();

    public String GetFuncName();

    public int GetFuncNameId();

    public void SetSectionIndex(final int sectionInd);

    public int GetSectionIndex();

    public int GetNestedLevel();


    // obsolete?
    public Label GetSectionLabel();
    
    public boolean SetShowAll(final boolean set);

    public void InvertShowAll();
    
    public void CreateLabel(final Group root);
    
    public void CreateEDG(final Group root);
}

/**
 * @brief The class to presentation one section
 * @author HP
 */
public abstract class Section extends Rectangle implements ISection{
    public static final int ATOM_FUNC_WIDTH = 5;
    protected static final int BIG_OBJ_THRESHOLD = 2;
    protected static final int WIDTH_FOR_SMALL_SECTION = 2;
    
    protected SectionData secInfo;

    public static void CheckValidSectionData(int thID, double start, double end, int parallax){
        if((thID < 0 || start < 0 || end < 0 || parallax < 0)){
            throw new InvalidParameterException("INVALID (thID < 0 || start < 0 || end < 0) for thID = "+thID);
        }
        if((end-start) < 0){
            throw new InvalidParameterException("INVALID ((end-start)<0) for thID = "+thID);   
        }
        if(StatisticWindow.TimestampLine.TimeLineEnd < end) {
            throw new InvalidParameterException("INVALID SECTION for thID = "+thID);
        }
    }
        
    public static ISection CreateStartEndSection(
            final Mark mrk1, final Mark mrk2, int thID, int parallax) {
        CheckValidSectionData(thID, mrk1.time, mrk2.time, parallax);

        //final int funcNameInd = elemEnd.funcId;
        //final int funcNameInd = MappingFuncNames.GetIndex(ArrayStr[0]);
        final int funcNameId = MappingFuncNames.PAIR_FUNC_ID_TO_INTERNAL_MAP_FUNC_ID.get(mrk2.funcId);

        final double SpentTime = (mrk1.time - StatisticWindow.TimestampLine.TimeLineStart);
        final int widthSpentTimeBefore = (int)((SpentTime/StatisticWindow.TimestampLine.TimeLineWidth)*W);

        final double deltaSpentTime = mrk2.time-mrk1.time;
        final int widthDeltaSpentTime = (int)((deltaSpentTime/StatisticWindow.TimestampLine.TimeLineWidth)*W);

        final int wid = AntiSmallSection(widthDeltaSpentTime);
        final SectionData secInfo = new SectionData(thID, funcNameId, SpentTime, mrk2.time-StatisticWindow.TimestampLine.TimeLineStart, parallax);
        Section rect = new TouchedViewSection(secInfo, widthSpentTimeBefore, 0, wid, H_STAT_RECT);

        final Color c = MappingFuncNames.GetColorForFuncIndex(funcNameId);
        rect.setFill(c);
        rect.setStroke(Color.BLACK);    

        System.out.printf("[CreateStartEndSection] Thread %d : %s : start=%.6f end=%.6f\n",
                thID, MappingFuncNames.GetFuncNameIndex(funcNameId), mrk1.time, mrk2.time);
        return rect;
    }

    public static ISection CreateAtomSection(Mark mrk1, int thID, final int parallax) {

        if(mrk1.funcId.equals(MappingFuncNames.BARRIER_ID)) {
            final double SpentTime = (mrk1.time - StatisticWindow.TimestampLine.TimeLineStart);
            final int widthSpentTimeBefore = (int)((SpentTime/StatisticWindow.TimestampLine.TimeLineWidth)*W);

            final int funcNameId = MappingFuncNames.GetIndex(MappingFuncNames.BARRIER);
            final SectionData secInfo = new SectionData(thID, funcNameId, SpentTime, SpentTime, parallax);
            Section rect = new TouchedViewSection(secInfo, widthSpentTimeBefore, 0, Section.ATOM_FUNC_WIDTH, H_STAT_RECT);

            final Color c = MappingFuncNames.GetColorForFuncIndex(funcNameId);
            rect.setFill(c);
            rect.setStroke(Color.BLACK);

            return rect;
        } else if(mrk1.funcId.equals(MappingFuncNames.GET_WTIME_ID)) {
            System.out.println("[CreateAloneSection] Thread " +
                    String.valueOf(thID)+ " : "+ MappingFuncNames.GET_WTIME);
            return null;
        }

        throw new InvalidParameterException("INVALID BARRIER SECTION for thID = "+thID);
    }


    private static int AntiSmallSection(int widthDeltaSpentTime){
        int returnSectionWidth = widthDeltaSpentTime;
        if(widthDeltaSpentTime <= BIG_OBJ_THRESHOLD){
            returnSectionWidth = WIDTH_FOR_SMALL_SECTION;
        }
        return returnSectionWidth;
    }

    Section(final SectionData secInfo, int X, int Y, int width, int height){
        super(X, Y, width, height);
        this.secInfo = secInfo;
        this.setStrokeWidth(0);
    }

    @Override
    public double GetStartTime(){
        return secInfo.startTime;
    }        

    @Override
    public double GetEndTime(){
        return secInfo.endTime;
    }

    @Override
    public String GetFuncName(){
        return secInfo.GetFuncName();
    }

    @Override
    public int GetFuncNameId(){
        return secInfo.funcNameId;
    }

    @Override
    public void SetSectionIndex(int sectionInd){
        secInfo.SetSectionIndex(sectionInd);
    }

    @Override
    public int GetSectionIndex(){
        return secInfo.GetSectionIndex();
    }

    @Override
    public int GetNestedLevel(){
        return secInfo.numNestedLevel;
    }
}

class TouchedViewSection extends Section {
    
    private static final int SCALE_ENLARGEMENT_WIDTH = 15;
    private static final int SCALE_ENLARGEMENT_HEIGHT = 15;
    private static final float REDUCE_SCALE_EFFECT_W = 0.5f;

    private static final double SECTION_EDGE_STROKE_WIDTH = 2.5;
    private static final Color START_END_TIME_LINE_STROKE_COLOR = Color.BLACK;
    private static final int WIDTH_THRESHOLD_TWO_ONE = 50;

    private static final int  SECTION_LABEL_WIDTH = 200;

    private static final Color SECTION_LABEL_TEXT_COLOR = Color.BLACK;
    private static final Font SECTION_LABEL_FONT   = Font.font("BlackArial", FontWeight.BOLD, 24);
    
    private boolean isClicked;
    private Label sectionLabel;
    //private double prevWidth;
    private Label startTimeLabel;
    private Label endTimeLabel;
    private Line startTimeLine;
    private Line endTimeLine;
    
    TouchedViewSection(final SectionData secInfo, int X, int Y, int width, int height){
        super(secInfo, X, Y, width, height);
        sectionLabel = null;
        startTimeLabel  = endTimeLabel = null;
        startTimeLine = endTimeLine = null;
        //prevWidth = width;
        isClicked = false;      
    }

    @Override
    public Label GetSectionLabel(){
        return sectionLabel;
    }

    @Override
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
    
    @Override
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

                startTimeLabel = new Label(Timeline.GetFormatTime(secInfo.startTime));
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

                endTimeLabel = new Label(Timeline.GetFormatTime(secInfo.endTime));
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

            endTimeLabel = startTimeLabel = new Label(Timeline.GetFormatTime(secInfo.startTime));
            startTimeLabel.setFont(TIME_STAMP_FONT);
            startTimeLabel.setLayoutX(startTimeLine.getStartX());
            startTimeLabel.setLayoutY(startTimeLine.getStartY() - OFFSET_Y_SECTION_EDG_TIMESTAMPS);
            root.getChildren().add(startTimeLabel);
        }
        SetShowAll(false);
    }

    @Override
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

    @Override
    public void InvertShowAll(){
        SetShowAll(!isClicked);
    }

    private boolean isBigSection(){
        return (this.getWidth() > WIDTH_THRESHOLD_TWO_ONE);
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
}
