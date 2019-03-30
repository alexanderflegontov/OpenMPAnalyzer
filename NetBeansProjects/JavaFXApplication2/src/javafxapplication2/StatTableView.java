/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.ArrayList;
import javafx.event.EventHandler;
import javafx.geometry.HPos;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.ScrollPane.ScrollBarPolicy;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Border;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;

/**
 * @brief The class is a representation of all parallel sections in traced program.
 * @author hp
 */
public class StatTableView extends VBox{

    private static final String HEADER_STYLE= "-fx-background-color: rgb(189,150,255);"
                            + "-fx-border-color: rgb(0,0,0);"
                            + "-fx-border-width: 0;"
                            + "-fx-padding: 0,0,0,0;"
                            + "-fx-background-insets: 0;";

    private static final String STAT_TABLE_STYLE= "-fx-background-color: rgb(245,246,247);"
                            + "-fx-border-color: rgb(255,255,255);"
                            + "-fx-border-width: 0;"
                            + "-fx-padding: 0;"
                            + "-fx-background-insets: 0;";

    private static final String[] HEADER_STRINGS = {"section№", "start time", "end time", "delta", "TID","Payload start", "Payload end", "delta", "rate(%)"};
    private static final int[] HEADER_PERCENT = {9, 14, 14, 10 ,6, 14, 14, 10, 10};
    private static final String strDoubleFormat = "%."+String.valueOf(3)+"f";

    //public static final int W = 320;
    public static final int H = 167;
    
    private StatTableData statTableData;
    private ArrayList refParallelSections;
    private GridPane header;
    private int unit = 1;
    static private ScrollPane StatTableScroller;
    static private VBox grScroll;

    ColumnConstraints[] colsSetup;
    ColumnConstraints colsIndentSetup;
    
    public StatTableData GetStatTableData(){
        return statTableData;
    }
    
    StatTableView(double sceneWidth){
        super();

        statTableData = new StatTableData();
        refParallelSections = statTableData.GetRefParallelSections();

        grScroll = new VBox();
        //grScroll.setMaxSize( (int)sceneWidth+1, (H));
        grScroll.setMinSize( (int)sceneWidth+11, (H)); // 11->1 
        grScroll.setBorder(Border.EMPTY);
        grScroll.setStyle(STAT_TABLE_STYLE);
        
        StatTableScroller = new ScrollPane();
        //StatWindowScroller.setMinViewportHeight(300);
        StatTableScroller.setVbarPolicy(ScrollBarPolicy.AS_NEEDED);
        StatTableScroller.setHbarPolicy(ScrollPane.ScrollBarPolicy.NEVER);
        StatTableScroller.setMinSize((int)sceneWidth+13, (H)); // 13->2 0->(2)
        StatTableScroller.setMaxSize((int)sceneWidth+13, (H)); // 13->2 0->(2)
        StatTableScroller.setContent(grScroll);
        StatTableScroller.setBorder(Border.EMPTY);
        StatTableScroller.setStyle(STAT_TABLE_STYLE);

        colsSetup = new ColumnConstraints[HEADER_STRINGS.length];
        for(int i = 0; i < HEADER_STRINGS.length; ++i){
            colsSetup[i] = new ColumnConstraints();
            colsSetup[i].setPercentWidth(HEADER_PERCENT[i]);
            colsSetup[i].setHalignment(HPos.CENTER);
        }
        
        colsIndentSetup = new ColumnConstraints();
        colsIndentSetup.setPercentWidth(colsSetup[0].getPercentWidth() +
                                   colsSetup[1].getPercentWidth() +
                                   colsSetup[2].getPercentWidth() +
                                   colsSetup[3].getPercentWidth());
        colsIndentSetup.setHalignment(HPos.CENTER);

        ShowStatTable();
    }
    
    public void ShowStatTable(){
        this.getChildren().clear();
        grScroll.getChildren().clear();

        CreateHeader();
        this.getChildren().add(StatTableScroller);

        if(refParallelSections != null){
            CreateBody();
        }
    }

    public void UpdateUnit(double timeWidth){
        unit = Timeline.GetUnit(timeWidth);
    }
    
    private void CreateHeader(){
        header = new GridPane();
        header.setStyle(HEADER_STYLE);
        header.setBorder(Border.EMPTY);
        header.getColumnConstraints().addAll(colsSetup[0], colsSetup[1], colsSetup[2], colsSetup[3], colsSetup[4], colsSetup[5], colsSetup[6], colsSetup[7], colsSetup[8]);

        final String strUnit = "," + Timeline.UNIT[unit];
        header.add(new Label(HEADER_STRINGS[0]), 0, 0);
        header.add(new Label(HEADER_STRINGS[1]+strUnit), 1, 0);
        header.add(new Label(HEADER_STRINGS[2]+strUnit), 2, 0);
        header.add(new Label(HEADER_STRINGS[3]+strUnit), 3, 0);
        header.add(new Label(HEADER_STRINGS[4]), 4, 0);
        header.add(new Label(HEADER_STRINGS[5]+strUnit), 5, 0);
        header.add(new Label(HEADER_STRINGS[6]+strUnit), 6, 0);
        header.add(new Label(HEADER_STRINGS[7]+strUnit), 7, 0);
        header.add(new Label(HEADER_STRINGS[8]), 8, 0);
        header.setGridLinesVisible(true);
        //header.setPrefWidth(SettingWindow.WIDTH);
        this.getChildren().add(header);
    }
    
    private boolean CreateBody(){
        for(int i = 0; i < refParallelSections.size(); ++i){
            TableRowData statRow = (TableRowData)(refParallelSections.get(i));
            if(!statRow.IsValid()){
                System.out.println("CreateBody: table for parallel section #" + String.valueOf(i) + "is invalid!");
                return false;
            }
            System.out.println("CreateBody: i = " + String.valueOf(i) + " "
                    + statRow.GetStartParallel() + String.valueOf(statRow.GetEndParallel()) + " "
                    + String.valueOf(statRow.GetParallelDelta() + String.valueOf(0)) + " "
                    + String.valueOf(statRow.GetStartPayload(0)) + String.valueOf(statRow.GetEndPayload(0)) + " "
                    + String.valueOf(statRow.GetPayloadDelta(0)) + String.valueOf(statRow.GetRate(0)) + " "
                    );
            
            ParSectionElem parSectionElem = new ParSectionElem(i, statRow);
            grScroll.getChildren().add(parSectionElem);
        }
        return true;
    }
    
    public static void ShowParSection(int index){
        ParSectionElem parSectionElem = (ParSectionElem)grScroll.getChildren().get(index);
        if(parSectionElem == null){
            return;
        }
        parSectionElem.SetAllVisible(true);
        // Alignment
        double Y = parSectionElem.getLocalToParentTransform().getTy();
        double lastY = grScroll.getBoundsInParent().getHeight();
        double windowDeltaView = StatTableScroller.getHeight();
        double valueY = Y/(lastY-windowDeltaView);
        System.out.println("[ShowParSection]: valueY = "+valueY);
        StatTableScroller.setVvalue(valueY);
    }


    /**
     * @brief The class is a representation of one parallel section in statistic table.
     * @author hp
     */
    public final class ParSectionElem extends VBox{

        private static final String SECTION_HEADER_STYLE= "-fx-background-color: rgb(205,205,205);"
                                + "-fx-border-color: rgb(0,0,0);"
                                + "-fx-border-width: 0;"
                                + "-fx-padding: 0;"
                                + "-fx-background-insets: 0;";
        
        private static final String ROW_STYLE = "-fx-background-color: rgb(245,246,247);"
                                + "-fx-border-color: rgb(0,0,0);"
                                + "-fx-border-width: 0;"
                                + "-fx-padding: 0;"
                                + "-fx-background-insets: 0;";
        
        ArrayList<GridPane> lstRows;
        boolean m_isAllVisible;

        private static final int SHOW_PRECISION = 1;
            
        private String TimeToString(double time){
            return Timeline.GetFormatTimeOnly(time, unit, SHOW_PRECISION);
        }
        
        private String DoubleToString(double number){
            return String.format(strDoubleFormat, number);         
        }
        
        ParSectionElem(int sectionNumber, TableRowData statTableElem){
            super();
            lstRows = new ArrayList<>();
            
            GridPane rowHeader = new GridPane();
            rowHeader.setStyle(SECTION_HEADER_STYLE);
            rowHeader.setGridLinesVisible(true);
            rowHeader.getColumnConstraints().addAll(colsSetup[0], colsSetup[1], colsSetup[2], colsSetup[3], colsSetup[4], colsSetup[5], colsSetup[6], colsSetup[7], colsSetup[8]);
            
            rowHeader.add(new Label(String.valueOf(sectionNumber)), 0, 0);
            rowHeader.add(new Label(TimeToString(statTableElem.GetStartParallel())), 1, 0);
            rowHeader.add(new Label(TimeToString(statTableElem.GetEndParallel())), 2, 0);
            rowHeader.add(new Label(TimeToString(statTableElem.GetParallelDelta())), 3, 0);
            rowHeader.add(new Label(String.valueOf(0)), 4, 0);
            rowHeader.add(new Label(TimeToString(statTableElem.GetStartPayload(0))), 5, 0);
            rowHeader.add(new Label(TimeToString(statTableElem.GetEndPayload(0))), 6, 0);
            rowHeader.add(new Label(TimeToString(statTableElem.GetPayloadDelta(0))), 7, 0);
            rowHeader.add(new Label(DoubleToString(statTableElem.GetRate(0))), 8, 0);
            
            EventHandler<MouseEvent> threadStatEventHandlerMouseClick = new EventHandler<MouseEvent>() {
                @Override
                public void handle(MouseEvent event){
                    System.out.println("mouse click detected! ");
                    /*
                    // This is useful information, please never remove it !!!
                    System.out.println("getSource = " + event.getSource());
                    System.out.println("getTarget = " + event.getTarget());
                    System.out.println("event.getXY() = " + event.getX() +" "+ event.getY());
                    System.out.println("event.getScreenXY() = " + event.getScreenX() +" "+ event.getScreenY());
                    System.out.println("event.getSceneXY() = " + event.getSceneX()+" "+ event.getSceneY());

                    System.out.println("row.contains(getXY) = " + row.contains(event.getX(), event.getY()));
                    System.out.println("row.contains(getScreenXY) = " + row.contains(event.getScreenX(), event.getScreenY()));
                    System.out.println("row.contains(getSceneXY) = " + row.contains(event.getSceneX(), event.getSceneY()));
                    */
                    MouseClickEventHandle(event);
                    event.consume();
                }
            };
            
            rowHeader.setOnMouseClicked(threadStatEventHandlerMouseClick);

            lstRows.add(rowHeader);
            this.getChildren().add(rowHeader);
            
            for(int tid = 1; tid < statTableElem.GetNumThreads(); ++tid){
                GridPane addRow = new GridPane();
                addRow.setStyle(ROW_STYLE);
                addRow.setGridLinesVisible(true);

                addRow.add(new Label(String.valueOf(tid)), 1, tid);
                addRow.add(new Label(TimeToString(statTableElem.GetStartPayload(tid))), 2, tid);
                addRow.add(new Label(TimeToString(statTableElem.GetEndPayload(tid))), 3, tid);
                addRow.add(new Label(TimeToString(statTableElem.GetPayloadDelta(tid))), 4, tid);
                addRow.add(new Label(DoubleToString(statTableElem.GetRate(tid))), 5, tid);

                addRow.getColumnConstraints().addAll(colsIndentSetup, colsSetup[4], colsSetup[5], colsSetup[6], colsSetup[7], colsSetup[8]);

                lstRows.add(addRow);
            }
            this.getChildren().addAll(lstRows.subList(1, lstRows.size()));
            SetAllVisible(false);
        }

        public boolean IsAllVisible(){
            return m_isAllVisible;
        }
        
        public void SetAllVisible(boolean isAllVisible){
            m_isAllVisible = isAllVisible;
            if(m_isAllVisible){
                this.getChildren().forEach((elem) -> { elem.setVisible(true);});
            }else{
                this.getChildren().forEach((elem) -> { elem.setVisible(false);});
                this.getChildren().get(0).setVisible(true);
            }
        }
        
        public void MouseClickEventHandle(MouseEvent event) {
            MouseButton button = event.getButton();
            if(null != button)switch (button) {
                case PRIMARY:
                    System.out.println("PRIMARY button clicked");
                    this.SetAllVisible(!this.IsAllVisible());
                    break;
                case SECONDARY:
                    System.out.println("SECONDARY button clicked");
                    break;
                case MIDDLE:
                    System.out.println("MIDDLE button clicked");
                    break;
                default:
                    break;
            }
        }
    }
}
