/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;


import java.io.File;
import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import javafx.beans.property.SimpleDoubleProperty;

import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.paint.Color;
import javafx.scene.control.ScrollPane;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.ScrollEvent;
import javafx.scene.input.TransferMode;
import javafx.scene.layout.VBox;
import javafx.scene.shape.Line;
import javafxapplication2.DataAnalyzer.Mark;

//import javafxapplication2.Form.MappingFuncNames;
import static javafxapplication2.SettingWindow.*;

/**
 *
 * @author HP
 */

public class StatisticWindow extends Group{
    private InfoBoard infoBoard;
    private Legend legenda;
    private StatTable statTable;
        
    private Map hashTableThreadStat;
    private HashSet<Integer> functionsLegendHashSet;
    private Map TableStatThreads;

    private Group rootGroup;    
    private ScrollPane StatWindowScroller;
    private Group ScrollingGroup;// Explain cascade of group:  this group <- rootGroup <- StatWindowScroller <- ScrollingGroup
//////////////
    private int ThreadCount;
    private ArrayList<Integer> ArrThreadId;
/////////////
    private static final double MAX_ZOOM = 100.0d;
    private static final double MIN_ZOOM = 1.0d;
    private static final double ZOOM_FACTOR = 1.2;    

    //private SimpleDoubleProperty zoom = new SimpleDoubleProperty(1.0);
    private SimpleDoubleProperty posX = new SimpleDoubleProperty(50.0); // 50% in scrollH
    private SimpleDoubleProperty posY = new SimpleDoubleProperty(0.0);
    private NumbersOfThreadsPane numberOfThreadsPane;    
/////////////

    private static final String STAT_WINDOW_SCROLLER_STYLE = "-fx-background-color: rgb(80,80,80);"
                                + "-fx-border-color: rgb(0,0,0);"
                                + "-fx-border-width: 0;";
        
        
    EventHandler<DragEvent> eventDragOverHandler = new EventHandler<DragEvent>(){
        public void handle(DragEvent event){
            Dragboard db = event.getDragboard();
            if(db.hasFiles()){
                event.acceptTransferModes(TransferMode.COPY);
                event.consume();
            }
        }
    };
        
    EventHandler<DragEvent> eventDragDroppedHandler = new EventHandler<DragEvent>(){
        public void handle(DragEvent event){
            Dragboard db = event.getDragboard();
            if(db.hasFiles()){                          
                System.out.println("db.getFiles().size() = " + db.getFiles().size());
                File file = db.getFiles().get(0);
                System.out.println(" file.getAbsolutePath(): " + file.getAbsolutePath() + "");
                dragFile(file);
            }     
            event.setDropCompleted(true);
            event.consume();
        }
    };    
        
    public void clearAll(){
        //rootGroup.getChildren().clear();
        ScrollingGroup.getChildren().clear();
        ScrollingGroup.setScaleX(MIN_ZOOM);
        StatWindowScroller.setHvalue(0);
        StatWindowScroller.setVvalue(0);
    }

    public Map GetHashTableThreadStat(){
        return hashTableThreadStat;
    }
    
    public HashSet GetFunctionsLegendHashSet(){
        return functionsLegendHashSet;
    }

    public void dragFile(File file){
        if(file != null)
        {
            //System.out.println(" File: " + strAppPath + " is found! ");
            System.out.println(" file.getName(): " + file.getName() + "");
            System.out.println(" file.getPath(): " + file.getPath() + "");
            System.out.println(" file.getAbsolutePath(): " + file.getAbsolutePath() + "");

            System.out.println("=================StartTrace==================");
            //StartTrace(file);
            //Trace trace = new Trace(file);
            //trace.Start();
            System.out.println("=================StartAnalyzeData============");
            DataAnalyzer analyzeData = new DataAnalyzer();
            final String strPinToolOutfile = file.getAbsolutePath();
            Map TableStatisticaThreads = analyzeData.Start(strPinToolOutfile);
            System.out.println("=================StartCreateBarChart=========");
            this.Show(TableStatisticaThreads);

            //this.setTitle("Начало построения диаграмм ");
            //StatBoard = new StatisticWindow(root);
            //this.setTitle("Конец построения");
        }
        else
        {
            //System.out.println(" File: " + strAppPath + " is not found! ");
        }
    }

    StatisticWindow(Scene scene, InfoBoard pInfoBoard, Legend pLegenda, StatTable pStatTable)
    {
        super();
        rootGroup = new Group();
        rootGroup.setLayoutX(0);
        rootGroup.setLayoutY(0);
        this.getChildren().add(rootGroup);

        infoBoard = pInfoBoard;
        legenda = pLegenda;
        statTable = pStatTable;
        
        if(infoBoard == null || legenda == null || statTable == null){
            throw new UnsupportedOperationException("Not supported yet.");
        }

        hashTableThreadStat = new TreeMap< Integer, ThreadStat>(); 
        functionsLegendHashSet = new HashSet<>();

        infoBoard.SetHashTable(GetHashTableThreadStat());
        legenda.SetHashSet(GetFunctionsLegendHashSet());
        ThreadStat.BindStatTableData(statTable.GetStatTableData());
        
        this.setOnDragOver(eventDragOverHandler);
        this.setOnDragDropped(eventDragDroppedHandler);
        
        ////ShowNumbersOfThreadsPane();
        numberOfThreadsPane = new NumbersOfThreadsPane();
        numberOfThreadsPane.setLayoutX(OFFSET_TEXT);
        numberOfThreadsPane.setLayoutY(TOP_INDENT_Y);                 
        rootGroup.getChildren().add(numberOfThreadsPane);
        
        ScrollingGroup = new Group();
        StatWindowScroller = new ScrollPane();
        rootGroup.getChildren().add(StatWindowScroller);
        //StatWindowScroller.setMinViewportHeight(300);
        StatWindowScroller.setVbarPolicy(ScrollPane.ScrollBarPolicy.ALWAYS);
        StatWindowScroller.setHbarPolicy(ScrollPane.ScrollBarPolicy.AS_NEEDED);
        StatWindowScroller.setMinSize(SettingWindow.W+SCROLL_BAR_WIDTH, SettingWindow.H);
        StatWindowScroller.setMaxSize(SettingWindow.W+SCROLL_BAR_WIDTH, SettingWindow.H);
        StatWindowScroller.setContent(new Group(ScrollingGroup));
        StatWindowScroller.setStyle(STAT_WINDOW_SCROLLER_STYLE);
        StatWindowScroller.setOnDragOver(eventDragOverHandler);
        StatWindowScroller.setOnDragDropped(eventDragDroppedHandler);
        StatWindowScroller.setLayoutX(LEFT_INDENT_X);
        StatWindowScroller.setLayoutY(TOP_INDENT_Y);
        //StatWindowScroller.setPrefViewportWidth(SettingWindow.W);
        //StatWindowScroller.setPrefViewportHeight(SettingWindow.H);
        StatWindowScroller.hvalueProperty().bindBidirectional(posX);
        StatWindowScroller.vvalueProperty().bindBidirectional(posY);
        posX.addListener(o -> UpdateTimestampLine());
        
        StatWindowScroller.addEventHandler(ScrollEvent.ANY, new EventHandler<ScrollEvent>() {
            @Override
            public void handle(ScrollEvent event) {
                System.out.println("^^^^^^^^ StatWindowScroller !!!!!!!!!!!!!!!!!!!!!");
                    double w = StatWindowScroller.getWidth();
                    double h = StatWindowScroller.getHeight();
                    double xpos = event.getX();
                    double ypos = event.getY();

                    // Масштабирование по X:
                    double cursorPosPercentX = xpos/w;
                    System.out.println("^^^^^^^^ cursorPosPercentX = "+ cursorPosPercentX);                    
                    posX.set(cursorPosPercentX);

                    // Get current scale
                    double scale = ScrollingGroup.getScaleX();//.getScale(); // currently we only use Y, same value is used for X
                    // Update current scale
                    if (event.getDeltaY() < 0)
                        scale /= ZOOM_FACTOR;
                    else
                        scale *= ZOOM_FACTOR;
                    // Correct range current scale: [min;max]
                    scale = clamp( scale, MIN_ZOOM, MAX_ZOOM);

                    // Set current scale 
                    ScrollingGroup.setScaleX(scale);

                    event.consume();
                    
            }
        });
/*
        ScrollingGroup.addEventFilter(ScrollEvent.ANY, new EventHandler<ScrollEvent>() {
            @Override
            public void handle(ScrollEvent event) {
                if (event.getDeltaY() > 0) {
                    zoomProperty.set(zoomProperty.get() * 1.1);
                } else if (event.getDeltaY() < 0) {
                    zoomProperty.set(zoomProperty.get() / 1.1);
                }
            }
*/        
        
    }
    
    //throw new UnsupportedOperationException("Not supported yet.");
    public static double clamp( double value, double min, double max) {

        if( Double.compare(value, min) < 0)
            return min;

        if( Double.compare(value, max) > 0)
            return max;

        return value;
    }
    
    void UpdateTimestampLine(){
        System.out.println("[UpdateTimestampLine]: posX.getValue() = "+ posX.getValue());
        
        double scale = ScrollingGroup.getScaleX();
        double valueX  = StatWindowScroller.getHvalue();
        double oldDeltaAllTime = TimestampLine.TimeLineWidth;
        //double newDeltaAllTime = oldDeltaAllTime*scale;
        double windowDeltaViewTime = oldDeltaAllTime/scale;
        
        double start = (oldDeltaAllTime-windowDeltaViewTime)*valueX;
        double end = start+windowDeltaViewTime;
        
        TimestampLine.TimestampsUpdate(start, end);        

    }
    
    void Show(Map tableStatThreads){
        this.clearAll();
        
        TableStatThreads = tableStatThreads;
        boolean bOk = (TableStatThreads != null);

        if(bOk){
            ThreadCount = TableStatThreads.keySet().size();
            bOk = (ThreadCount > 0);
        }
        
        //rootGroup.setVisible(bOk);
        if(bOk){
            
            ArrThreadId = new ArrayList<Integer>(TableStatThreads.keySet());
            System.out.println("StatisticaBoard::ThreadCount = " + String.valueOf(ThreadCount));
            
            TimestampLine.CalculateGlobalConstants(TableStatThreads, ArrThreadId);

            System.out.println("!!!!!!!!!! ShowTimeLine() !!!!!!!!!!!!!!!!!!!!!");
            TimestampLine.Show(rootGroup);
            //TimestampLine.ShowEdgeLine(rootGroup);

            System.out.println("!!!!!!!!!! ShowNumbersOfThreadsPane() !!!!!!!!!!!!!!!!!!!!!");
            numberOfThreadsPane.updateTextLabels();

            System.out.println("!!!!!!!!!! ShowStatThreads() !!!!!!!!!!!!!!!!!!!!!");
            //Map hashTableThreadStat = ShowStatThreads();
            ShowStatThreads();
            //ArrayList<List<Rectangle>> arraySectionsThreads = AnalysisAllSectionsAllThreads(TableStatThreads);
            
            System.out.println("!!!!!!!!!! Show Legend !!!!!!!!!!!!!!!!!!!!!");
            legenda.ShowLegend();
            
            System.out.println("!!!!!!!!!! Show Metrics !!!!!!!!!!!!!!!!!!!!!");
            infoBoard.ShowMetrics();

            System.out.println("!!!!!!!!!! Show Stat Table !!!!!!!!!!!!!!!!!!!!!");
            statTable.ShowStatTable();
        }else{
            System.out.println("Fail to decode!!!");
            Label label = new Label("Fail to decode!!!");
            label.setFont(NOT_SUCCESSFULL_FONT);
            label.setAlignment(Pos.CENTER);
            label.setLayoutX(SettingWindow.W/2);
            label.setLayoutY(SettingWindow.H/2);
            ScrollingGroup.getChildren().add(label);
        }
    }
    
    Map ShowStatThreads(){
            
        List< Map<Integer, List<Section>> > ParsingThread = new ArrayList<>();
        
        //Map hashTableThreadRects = new TreeMap< Integer, List<Section>>(); 
        functionsLegendHashSet.clear();
        hashTableThreadStat.clear();
        statTable.GetStatTableData().ClearPrevData();
        statTable.UpdateUnit(TimestampLine.intervalTime);
        infoBoard.UpdateUnit(TimestampLine.intervalTime);
                
        
        for(int thID=0; thID<ThreadCount; thID++){
            //List<Section> getListRectangles = new ArrayList<Section>();
            ThreadStat threadStat = new ThreadStat(thID);
            int Y = H_STAT_TH*(thID+1) - H_STAT_RECT;
            threadStat.setLayoutY(Y);
            ScrollingGroup.getChildren().add(threadStat);

            hashTableThreadStat.put(thID, threadStat);

            List<Mark> getListMarks = (List<Mark>) TableStatThreads.get(ArrThreadId.get(thID));           

            LogParsing parser = new LogParsing();
            parser.Parsing(getListMarks, thID);
            ParsingThread.add(parser.GetSectionHierarchicalTable());
        }   
            
        //Map<Integer, List<Mark>> sectionHierarchicalTable = parser.GetSectionHierarchicalTable();
            
        for(int thID=0; thID<ThreadCount; thID++){

            ThreadStat threadStat = (ThreadStat) hashTableThreadStat.get(thID);
            
            Map<Integer, List<Section>> sectionHierarchicalTable = (Map<Integer, List<Section>>) ParsingThread.get(thID);
            for(int parallax=0; parallax < sectionHierarchicalTable.size(); parallax++){
                List<Section> lstMarks = (List<Section>) sectionHierarchicalTable.get(parallax);
                for(int elemNum=0; elemNum < lstMarks.size(); elemNum++){
                    Section rect = lstMarks.get(elemNum);

                    threadStat.AddSection(rect);
                    functionsLegendHashSet.add(rect.GetFuncNameId());
                }
            }

            //SettingMouseClick(sec);
            threadStat.CalculateEnd();
            threadStat.CreateHiddenInfo();
            /*for(Iterator iter = getListRectangles.iterator(); iter.hasNext();){
                Section sec = (Section) iter.next();
                sec.CreateLabel(rootGroup);
                sec.CreateEDG(rootGroup);
            }*/
        }    

        return hashTableThreadStat;
            
    }
    
    
    public static void main(String[] args) {
        //Stage primaryStage = new Stage(); 
        //StatisticWindow sw = new StatisticWindow(primaryStage);
        //launch(args);
    }

    public static class TimestampLine{
        public static double TimeLineStart;
        public static double TimeLineEnd;
        public static double TimeLineWidth;
        public static double intervalTime;
        private static ArrayList<Label> lstNumber = new ArrayList<>();
        
        static void CalculateGlobalConstants(Map tableStatThreads, ArrayList<Integer> arrThreadId){
             if(tableStatThreads != null){

                List<Mark> TMPgetListMarks = (List<Mark>) tableStatThreads.get(arrThreadId.get(0));
                TimeLineStart = TMPgetListMarks.get(0).time;
                TimeLineEnd   = TMPgetListMarks.get(TMPgetListMarks.size()-1).time;
                System.out.println("++++++++++++ TimeLineEnd =  " + String.valueOf(TimeLineEnd));

                TimeLineWidth = TimeLineEnd - TimeLineStart;
                intervalTime  = (TimeLineWidth/COUNT_SCALE_LINE);
                //intervalTime  = (double)(((int)(intervalTime*100))/100.0);
            }           
        }
        
        public static void Show(Group root){
            root.getChildren().removeAll(lstNumber);
            lstNumber.clear();

            String[] intervalTimes = Legend.GetIntervalTimes(intervalTime, COUNT_SCALE_LINE);
            for(int i = 0; i <= COUNT_SCALE_LINE; i++){
                int X = LEFT_INDENT_X+i*INTERVAL;

                Line line = new Line(X, TOP_INDENT_Y-OFFSET_Y_SCALE_LINE-SCALE_LINE_HALF_H, X, TOP_INDENT_Y-OFFSET_Y_SCALE_LINE+SCALE_LINE_HALF_H);
                line.setStroke(Color.BLACK);
                line.setStrokeWidth(INTERVAL_EDGE_STROKE_WIDTH);
                root.getChildren().add(line);

                Label label = new Label(intervalTimes[i]);
                label.setFont(SCALE_VALUE_FONT);
                label.setLayoutX(X - intervalTimes[i].length()*COEF_FOR_OFFSET_X_TIMESTAMPS);
                label.setLayoutY(line.getStartY()-OFFSET_Y_NUMERICAL_VALUES);
                root.getChildren().add(label);
                lstNumber.add(label);
            }
            
            for(int i = 0; i < COUNT_SCALE_LINE; i++){
                int X = LEFT_INDENT_X+i*INTERVAL;
                ShowBetween(root, X);
            }
        }
        
        public static void ShowBetween(Group root, final int gX){
            //String[] intervalTimes = Legend.GetIntervalTimes(intervalTime, COUNT_SCALE_LINE);
            for(int i = 1; i < COUNT_SCALE_LINE_BETWEEN; i++){
                int X = gX+i*INTERVAL_BETWEEN;

                Line line = new Line(X, TOP_INDENT_Y-OFFSET_Y_SCALE_LINE-SCALE_LINE_HALF_H_BETWEEN, X, TOP_INDENT_Y-OFFSET_Y_SCALE_LINE+SCALE_LINE_HALF_H_BETWEEN);
                line.setStroke(Color.BLACK);
                line.setStrokeWidth(INTERVAL_EDGE_STROKE_WIDTH);
                root.getChildren().add(line);
            }
        }
        
        public static void TimestampsUpdate(double start, double end){
            double delta = (end-start)/COUNT_SCALE_LINE;
            String[] intervalTimes = Legend.GetIntervalTimes(delta, COUNT_SCALE_LINE, start);
            //System.out.println("interablTimes:" + Arrays.toString(intervalTimes));
            for(int i = 0; i <= COUNT_SCALE_LINE; i++){
                Label label = lstNumber.get(i);
                label.setText(intervalTimes[i]);
            }
        }
        
            // not used
        public static void ShowEdgeLine(Group root){
            final int Y = TOP_INDENT_Y-OFFSET_Y_SCALE_LINE;

            Line line = new Line(LEFT_INDENT_X, Y, LEFT_INDENT_X+W, Y);
            line.setStroke(Color.BLACK);
            line.setStrokeWidth(H_LINE_EDGE_STROKE_WIDTH);
            root.getChildren().add(line);


            line = new Line(LEFT_INDENT_X, Y, LEFT_INDENT_X, Y);
            line.setStroke(Color.BLACK);
            line.setStrokeWidth(V_LINE_EDGE_STROKE_WIDTH);
            root.getChildren().add(line);

            line = new Line(LEFT_INDENT_X+W, Y, LEFT_INDENT_X+W, Y);
            line.setStroke(Color.BLACK);
            line.setStrokeWidth(V_LINE_EDGE_STROKE_WIDTH);
            root.getChildren().add(line);
        }
    }
    
    public class NumbersOfThreadsPane extends ScrollPane{
        
        private static final String NUMBERS_OF_THREADS_STYLE = "-fx-background-color: rgb(255,255,255);"
                        + "-fx-border-color: rgb(0,0,0);"
                        + "-fx-border-width: 0;";
        
        public VBox numberThreadGroup;
        private static final String STR_THREAD = "Thread";
        
        NumbersOfThreadsPane(){
            super();
            this.setStyle(NUMBERS_OF_THREADS_STYLE);
            this.setVbarPolicy(ScrollPane.ScrollBarPolicy.NEVER);
            this.setHbarPolicy(ScrollPane.ScrollBarPolicy.NEVER);
            //this.setMinSize(LEFT_INDENT_X, SettingWindow.H-SCROLL_BAR_WIDTH);
            this.setMaxSize(LEFT_INDENT_X, SettingWindow.H-SCROLL_BAR_WIDTH); // 17
            //this.setMaxHeight(SettingWindow.H-SCROLL_BAR_WIDTH);
            
            this.addEventFilter(ScrollEvent.ANY, (Event event) -> {
                event.consume();
            });
            
            this.vvalueProperty().bindBidirectional(posY);
            
            numberThreadGroup = new VBox();
            numberThreadGroup.setStyle(NUMBERS_OF_THREADS_STYLE);
            //Group newGroup = new Group();
            //numberThreadGroup.getChildren().add(newGroup);
            
            for(int i = 0; i < ThreadCount; i++){
                Label label = new Label(STR_THREAD+"("+Integer.toString(i)+")");
                label.setFont(THREAD_NUM_FONT);
                label.setPrefHeight(H_STAT_RECT);
                numberThreadGroup.getChildren().add(label);
                
                //int Y = H_STAT_TH*(i+1) - H_STAT_RECT;
                ////label.setLayoutX(OFFSET_TEXT);
                //label.setLayoutY((Y));
                //newGroup.getChildren().add(label);
            }     

            numberThreadGroup.setSpacing(H_STAT_TH-H_STAT_RECT);
            this.setContent(numberThreadGroup);
        }
        
        public void updateTextLabels(){
            if(ThreadCount < numberThreadGroup.getChildren().size()){
                numberThreadGroup.getChildren().remove(ThreadCount, numberThreadGroup.getChildren().size());
            }
            
            for(int i = 0; i < numberThreadGroup.getChildren().size(); i++){
                Label label = (Label)numberThreadGroup.getChildren().get(i);
                label.setText(STR_THREAD+"("+Integer.toString(i)+")");
            }
            
            if(numberThreadGroup.getChildren().size() < ThreadCount){
                for(int i = numberThreadGroup.getChildren().size(); i < ThreadCount; i++){
                    Label label = new Label(STR_THREAD+"("+Integer.toString(i)+")");
                    label.setFont(THREAD_NUM_FONT);
                    label.setPrefHeight(H_STAT_RECT);
                    numberThreadGroup.getChildren().add(label);

                    //int Y = H_STAT_TH*(i+1) - H_STAT_RECT;
                    ////label.setLayoutX(OFFSET_TEXT);
                    //label.setLayoutY((Y));
                    //newGroup.getChildren().add(label);
                }
            }
        }
    }

  
} 

/*
    
    ArrayList<List<Rectangle>> AnalysisAllSectionsAllThreads(Map tableStatThreads){
        int threadCount = tableStatThreads.keySet().size();
        ArrayList<List<Rectangle>> arraySectionsThreads = new ArrayList<List<Rectangle>>(threadCount);

        List<Rectangle> lstThreadSections;
        for(int i=0; i<threadCount; i++){
            System.out.println("^^^^^^^^ Thread " + String.valueOf(i));

            List<Mark> getListMarks = (List<Mark>) tableStatThreads.get(ArrThreadId.get(i));
            int Y_Str = TOP_INDENT_Y + H_STAT_TH*(i+1);
            int Y = Y_Str - H_STAT_RECT;
            
            lstThreadSections = AnalysisAllSections( getListMarks, Y);
            //arraySectionsThreads.put(i, lstThreadSections);
            //arraySectionsThreads.add(lstThreadSections);
        }
        
        return arraySectionsThreads;
    }
    
    List<Rectangle> AnalysisAllSections(List<Mark> getListMarks, int Y){
        
        List<Rectangle> lstThreadSections = new ArrayList<Rectangle>();
        
        List<Mark> lstSectionStart = new ArrayList<Mark>();
        boolean emptyReturnData = true;
        for(int i=0; i<getListMarks.size(); i++){
            Mark mrk1 = getListMarks.get(i);

            if(mrk1.funcName.contains(MappingFuncNames.START)){
                lstSectionStart.add(mrk1);
            }else if(mrk1.funcName.contains(MappingFuncNames.END)){
                boolean found = false;
                for(int j = lstSectionStart.size()-1; j>=0; j--){
                    Mark mrk2 = lstSectionStart.get(j);
                    String[] ArrayStr = mrk2.funcName.split("_");
                    if(mrk1.funcName.contains(ArrayStr[0])){
                        
                        
                        double SpentTime = (mrk1.time - TimeLineStart);
                        int widthSpentTime = (int)((SpentTime/TimeLineWidth)*W);

                        double deltaSpentTime = mrk2.time-mrk1.time;
                        int widthDeltaSpentTime = (int)((deltaSpentTime/TimeLineWidth)*W);
                        
                        //g.setColor(Color.RED);
                        //g.drawRect((int)(LEFT_INDENT_X + widthSpentTime), Y, widthDeltaSpentTime, HRect);                        

                        Color c = MappingFuncNames.GetColorForFuncIndex(MappingFuncNames.GetIndex(ArrayStr[0]));

                        Rectangle rect = new Rectangle((int)(LEFT_INDENT_X + widthSpentTime), Y, widthDeltaSpentTime, H_STAT_RECT);
                        rect.setFill(c);
                        rect.setStroke(Color.BLACK);    
                        rootGroup.getChildren().add(rect);
                        
                        lstSectionStart.remove(mrk2);
                        //lstThreadSections.add(rect);
                        found = true;
                        emptyReturnData = false;
                        System.out.println("^^^^^^^^ Section: " + ArrayStr[0] + " - is found");
                        break;
                    }
                    if(found == false){
                        System.out.println("^^^^^^^^^^^^ Pair Element: " + mrk1.funcName + " - is not found !!!!!!!!!!!!!!!!!!!!!");
                    }
                }
            }else if(mrk1.funcName.contains(MappingFuncNames.BARRIER)){
                System.out.println("^^^^^^^^ Operation: " + MappingFuncNames.BARRIER + " - is found");
                emptyReturnData = false;

                double SpentTime = (mrk1.time - TimeLineStart);
                int widthSpentTime = (int)((SpentTime/TimeLineWidth)*W);

                int X = (int)(LEFT_INDENT_X + widthSpentTime);

                Rectangle rect = new Rectangle(X, Y, 2, H_STAT_RECT);
                //rect.setFill(Color.BLACK);
                rect.setStroke(Color.BLACK);    
                rootGroup.getChildren().add(rect);

                // g.setColor(Color.BLACK);
                Label label = new Label(MappingFuncNames.BARRIER);
                label.setFont(TEXT_FONT);
                //label.localToParent(new Point2D(X-15, Y-OFFSET_Y_NUMERICAL_VALUES));
                label.setLayoutX(rect.getX()-15);
                label.setLayoutY(rect.getY()-OFFSET_Y_NUMERICAL_VALUES);
                rootGroup.getChildren().add(label);

                //lstThreadSections.add(rect);
            }else{
                System.out.println("^^^^^^^^ What is this? !!!!!!!!!!!!!!!!!!!!!");
            }
        }
        
        if (emptyReturnData == true){
            return null;
        }

        return lstThreadSections;
    }
    */