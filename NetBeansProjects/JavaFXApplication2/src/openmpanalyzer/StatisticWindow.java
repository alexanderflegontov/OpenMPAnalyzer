/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

import java.io.File;
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
import openmpanalyzer.DataAnalyzer.Mark;
import static openmpanalyzer.SettingWindow.*;

/**
 * @brief class is responsible for showing all the results.
 * It parses the result and creates legend, metrics, charts and table by them.
 * @author hp
 */
public class StatisticWindow extends Group{
    // Explanation for cascade of group:
    // this group <- rootGroup <- StatWindowScroller <- ScrollingGroup
    private Group rootGroup;
    private ScrollPane StatWindowScroller;
    private Group ScrollingGroup;
//////////////
    private Legend legenda;
    private MetricBoard metricBoard;
    private StatTableView statTable;

    private Map hashTableThreadStat;
    private HashSet<Integer> functionsLegendHashSet;
    private Map TableStatThreads;

    private int numThreads;
    private ArrayList<Integer> ArrThreadId;
//////////////
    private static final double MAX_ZOOM = 100.0d;
    private static final double MIN_ZOOM = 1.0d;
    private static final double ZOOM_FACTOR = 1.2;
    private SimpleDoubleProperty posX = new SimpleDoubleProperty(50.0); // hvalue = 50%
    private SimpleDoubleProperty posY = new SimpleDoubleProperty(0.0); // vvalue = 0%
    private NumbersOfThreadsPane numberOfThreadsPane;
//////////////
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

    private void clearAll(){
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
            System.out.println(" file.getName(): " + file.getName() + "");
            System.out.println(" file.getPath(): " + file.getPath() + "");
            System.out.println(" file.getAbsolutePath(): " + file.getAbsolutePath() + "");
            System.out.println("=================StartAnalyzeData============");
            DataAnalyzer analyzeData = new DataAnalyzer();
            final String strPinToolOutfile = file.getAbsolutePath();
            Map TableStatisticaThreads = analyzeData.Start(strPinToolOutfile);
            System.out.println("=================StartCreateBarChart=========");
            this.Show(TableStatisticaThreads);
        }
    }

    StatisticWindow(Scene scene, Legend pLegenda, MetricBoard pMetricBoard, StatTableView pStatTable){
        super();

        legenda = pLegenda;
        metricBoard = pMetricBoard;
        statTable = pStatTable;

        if(legenda == null || metricBoard == null || statTable == null){
            throw new UnsupportedOperationException(
                    "One or more: legenda, metricBoard, statTable - is null");
        }

        rootGroup = new Group();
        rootGroup.setLayoutX(0);
        rootGroup.setLayoutY(0);
        this.getChildren().add(rootGroup);
        this.setOnDragOver(eventDragOverHandler);
        this.setOnDragDropped(eventDragDroppedHandler);

        hashTableThreadStat = new TreeMap<Integer, ThreadView>(); 
        functionsLegendHashSet = new HashSet<>();
        //legenda.SetHashSet(GetFunctionsLegendHashSet());
        metricBoard.SetHashTable(GetHashTableThreadStat());
        ThreadView.BindStatTableData(statTable.GetStatTableData());

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

        
        StatWindowScroller.addEventFilter(ScrollEvent.SCROLL, new EventHandler<ScrollEvent>() {
            @Override
            public void handle(ScrollEvent event) { // Scale by X
                // Update X position
                double w = StatWindowScroller.getWidth();
                double xpos = event.getX();
                double cursorPosPercentX = xpos/w;
                posX.set(cursorPosPercentX);

                // Get and update the current scale
                double scale = ScrollingGroup.getScaleX();
                if (event.getDeltaY() < 0)
                    scale /= ZOOM_FACTOR;
                else
                    scale *= ZOOM_FACTOR;

                // Correct range of the current scale: [min;max]
                scale = clamp(scale, MIN_ZOOM, MAX_ZOOM);

                // Set the current scale
                ScrollingGroup.setScaleX(scale);
                event.consume();
            }
       });
    }

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
            numThreads = TableStatThreads.keySet().size();
            bOk = (numThreads > 0);
        }
        //rootGroup.setVisible(bOk);
        if(bOk){
            System.out.println("Number of threads: " + String.valueOf(numThreads));
            ArrThreadId = new ArrayList<Integer>(TableStatThreads.keySet());
            TimestampLine.CalculateTimestampLine(TableStatThreads, ArrThreadId);

            System.out.println("!!!!!!!!!! ShowTimeLine() !!!!!!!!!!!!!!!!!!!!!");
            TimestampLine.Show(rootGroup);

            System.out.println("!!!!!!!!!! ShowNumbersOfThreadsPane() !!!!!!!!!!!!!!!!!!!!!");
            numberOfThreadsPane.updateTextLabels();

            System.out.println("!!!!!!!!!! ShowStatThreads() !!!!!!!!!!!!!!!!!!!!!");
            ShowStatThreads();

            System.out.println("!!!!!!!!!! Show Legend !!!!!!!!!!!!!!!!!!!!!");
            legenda.ShowLegend(GetFunctionsLegendHashSet());

            System.out.println("!!!!!!!!!! Show Metrics !!!!!!!!!!!!!!!!!!!!!");
            metricBoard.ShowMetrics(GetHashTableThreadStat());

            System.out.println("!!!!!!!!!! Show Stat Table !!!!!!!!!!!!!!!!!!!!!");
            statTable.ShowStatTable();
        }else{
            System.out.println("Failed to decode!");
            Label label = new Label("Failed to decode!");
            label.setFont(NOT_SUCCESSFULL_FONT);
            label.setAlignment(Pos.CENTER);
            label.setLayoutX(SettingWindow.W/2);
            label.setLayoutY(SettingWindow.H/2);
            ScrollingGroup.getChildren().add(label);
        }
    }

    Map ShowStatThreads(){
        List< Map<Integer, List<Section>> > ParsingThread = new ArrayList<>();
        functionsLegendHashSet.clear();
        hashTableThreadStat.clear();
        statTable.GetStatTableData().ClearPrevData();
        statTable.UpdateUnit(TimestampLine.intervalTime);
        metricBoard.UpdateUnit(TimestampLine.intervalTime);

        for(int thID = 0; thID < numThreads; ++thID){
            ThreadView threadStat = new ThreadView(thID);
            int Y = H_STAT_TH*(thID+1) - H_STAT_RECT;
            threadStat.setLayoutY(Y);
            ScrollingGroup.getChildren().add(threadStat);
            hashTableThreadStat.put(thID, threadStat);

            List<Mark> getListMarks = (List<Mark>) TableStatThreads.get(ArrThreadId.get(thID));           
            LogParsing parser = new LogParsing();
            parser.Parsing(getListMarks, thID);
            ParsingThread.add(parser.GetSectionHierarchicalTable());
        }

        for(int thID=0; thID < numThreads; ++thID){
            ThreadView threadStat = (ThreadView) hashTableThreadStat.get(thID);
            Map<Integer, List<Section>> sectionHierarchicalTable = (Map<Integer, List<Section>>) ParsingThread.get(thID);
            for(int parallax = 0; parallax < sectionHierarchicalTable.size(); ++parallax){
                List<Section> lstMarks = (List<Section>) sectionHierarchicalTable.get(parallax);
                for(int elemNum = 0; elemNum < lstMarks.size(); ++elemNum) {
                    Section rect = lstMarks.get(elemNum);
                    threadStat.AddSection(rect);
                    functionsLegendHashSet.add(rect.GetFuncNameId());
                }
            }
            threadStat.CalculateEnd();
            threadStat.CreateHiddenInfo();
        }
        return hashTableThreadStat;
    }

    public static void main(String[] args) {
        //Stage primaryStage = new Stage(); 
        //StatisticWindow sw = new StatisticWindow(primaryStage);
        //launch(args);
    }


    /**
     * @brief the class is responsible for showing the time line with timestamp and scale in statistic window.
     * @author hp
     */
    public static class TimestampLine{
        public static double TimeLineStart;
        public static double TimeLineEnd;
        public static double TimeLineWidth;
        public static double intervalTime;
        private static ArrayList<Label> timestampsLabelList = new ArrayList<>();

        public static void CalculateTimestampLine(Map tableStatThreads, ArrayList<Integer> arrThreadId){
            List<Mark> mainThreadMarksList = (List<Mark>) tableStatThreads.get(arrThreadId.get(0));
            TimeLineStart = mainThreadMarksList.get(0).time;
            TimeLineEnd   = mainThreadMarksList.get(mainThreadMarksList.size()-1).time;
            TimeLineWidth = (TimeLineEnd - TimeLineStart);
            intervalTime  = (TimeLineWidth/COUNT_SCALE_LINE);
        }

        public static void Show(Group root){
            root.getChildren().removeAll(timestampsLabelList);
            timestampsLabelList.clear();

            String[] intervalTimes = Timeline.GetIntervalTimes(intervalTime, COUNT_SCALE_LINE);
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
                timestampsLabelList.add(label);
            }

            for(int i = 0; i < COUNT_SCALE_LINE; i++){
                int X = LEFT_INDENT_X+i*INTERVAL;
                ShowScale(root, X);
            }
            
            //ShowEdgeLine(root);
        }

        public static void ShowScale(Group root, final int gX){
            for(int i = 1; i < COUNT_SCALE_LINE_BETWEEN; ++i){
                int X = gX + (i * INTERVAL_BETWEEN);
                Line line = new Line(X, TOP_INDENT_Y-OFFSET_Y_SCALE_LINE-SCALE_LINE_HALF_H_BETWEEN, X, TOP_INDENT_Y-OFFSET_Y_SCALE_LINE+SCALE_LINE_HALF_H_BETWEEN);
                line.setStroke(Color.BLACK);
                line.setStrokeWidth(INTERVAL_EDGE_STROKE_WIDTH);
                root.getChildren().add(line);
            }
        }

        public static void TimestampsUpdate(double start, double end){
            double delta = (end-start)/COUNT_SCALE_LINE;
            String[] intervalTimes = Timeline.GetIntervalTimes(delta, COUNT_SCALE_LINE, start);
            for(int i = 0; i <= COUNT_SCALE_LINE; ++i){
                Label label = timestampsLabelList.get(i);
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


    /**
     * @brief the class is responsible for showing the sequence number of threads in statistic window.
     * @author hp
     */
    public class NumbersOfThreadsPane extends ScrollPane{
        
        private static final String NUMBERS_OF_THREADS_STYLE = "-fx-background-color: rgb(255,255,255);"
                        + "-fx-border-color: rgb(0,0,0);"
                        + "-fx-border-width: 0;";
        private static final String STR_THREAD = "Thread";
        public VBox numberThreadGroup;

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
            numberThreadGroup.setSpacing(H_STAT_TH-H_STAT_RECT);
            for(int i = 0; i < numThreads; ++i){
                Label label = new Label(STR_THREAD+"("+Integer.toString(i)+")");
                label.setFont(THREAD_NUM_FONT);
                label.setPrefHeight(H_STAT_RECT);
                numberThreadGroup.getChildren().add(label);
            }
            this.setContent(numberThreadGroup);
        }
        
        public void updateTextLabels(){
            int prevNumberOfThreads = numberThreadGroup.getChildren().size();
            if(numThreads < prevNumberOfThreads){
                numberThreadGroup.getChildren().remove(numThreads, prevNumberOfThreads);
            }

            prevNumberOfThreads = numberThreadGroup.getChildren().size();
            for(int i = 0; i < prevNumberOfThreads; ++i){
                Label label = (Label)numberThreadGroup.getChildren().get(i);
                label.setText(STR_THREAD+"("+Integer.toString(i)+")");
            }
            
            if(prevNumberOfThreads < numThreads){
                for(int i = prevNumberOfThreads; i < numThreads; ++i){
                    Label label = new Label(STR_THREAD+"("+Integer.toString(i)+")");
                    label.setFont(THREAD_NUM_FONT);
                    label.setPrefHeight(H_STAT_RECT);
                    numberThreadGroup.getChildren().add(label);
                }
            }
        }
    }

}
