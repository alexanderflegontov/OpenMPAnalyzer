/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;
import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;

import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import static javafxapplication2.SettingWindow.H_STAT_RECT;
import static javafxapplication2.SettingWindow.W;

/**
 * @brief the class keeps statistics of its thread.
 * It store data of statistics thread, view for each section,
 * This class is responsible for filling, storing, providing all statistical data for the thread.
 * It handles а mouse click event on а section that is included in the thread.
 * Fills statistics data for а table(another class).
 * @author hp
 */
class ThreadView extends Group{
    public final int threadId;

    private List<ISection> listRectangles;
    private ISection PrevShowSection;

    private ThreadData threadInfo;
    private static boolean oneMain;
    private boolean isValidThreadInfo;

    private static StatTableData statTableData;
    private int numPayloadParallax;
    private int numParallelParallax;

    private static final String BACKGROUND_TH_STYLE= "-fx-background-color: rgb(255,255,255);"
                    + "-fx-border-color: rgb(0,0,0);"
                    //+ "-fx-stroke-width: 2, 2, 2, 2;"
                    + "-fx-stroke-color: black;"
                    + "-fx-padding: 0,0,0,0;"
                    + "-fx-background-insets: 0;";
    
    public void MouseClickEventHandle(MouseEvent event) {
        System.out.println("+++++ Show statistics for tid = " + String.valueOf(this.threadId));
        MouseButton button = event.getButton();

        ISection rect = this.FindTouchRect(event.getX(), event.getY());
        if(rect == null){
            return;
        }
        System.out.println(rect.GetFuncName() + " is clicked");
        /*boolean isNeedOnlyHide = this.HideExtraInfo(rect);
        if(isNeedOnlyHide){
            return;
        }*/ 
        if(button == null){
            return;
        }

        switch (button) {
            case PRIMARY:
                System.out.println("PRIMARY button is clicked");
                //rect.InvertShowAll();
                if(rect.GetSectionIndex() != -1){
                    StatTableView.ShowParSection(rect.GetSectionIndex());
                }   break;
            case SECONDARY:
                System.out.println("SECONDARY button is clicked");
                break;
            case MIDDLE:
                System.out.println("MIDDLE button is clicked");
                break;
            default:
                break;
        }
    }

    public boolean HideExtraInfo(ISection sec){
        boolean isPrevStateShow = false;
        if(PrevShowSection != null){
            isPrevStateShow = PrevShowSection.SetShowAll(false);
        }
        // if we clicked on the same section, we only need to hide this section 
        boolean isNeedOnlyHide =  (PrevShowSection == sec) && isPrevStateShow;
        PrevShowSection = sec;
        return isNeedOnlyHide;
    }

    // [Strategy]The start of search is the end List!
    public ISection FindTouchRect(double X, double Y){
        List<ISection> TMP_threadStat = this.GetListRectangles();
        for(ListIterator li = TMP_threadStat.listIterator(TMP_threadStat.size()); li.hasPrevious(); ){
            Section rect = (Section) li.previous();
            if(rect.contains(X, Y)){
                return rect;
            }
        }
        return null;
    }

    private void BackgroundTHStat(){
        Line lineBordRectTop = new Line(0,0,W,0);
        lineBordRectTop.setStrokeWidth(1);
        this.getChildren().add(lineBordRectTop);
        lineBordRectTop.disableProperty();

        Rectangle rect = new Rectangle(0, 0, W, H_STAT_RECT);
        rect.setFill(Color.WHITE);
        rect.setStroke(Color.BLACK);
        rect.setStrokeWidth(0);
        rect.setStyle(BACKGROUND_TH_STYLE);
        this.getChildren().add(rect);
    }

    ThreadView(int thId){
        super();
        BackgroundTHStat();
        threadId = thId;
        listRectangles = new ArrayList<>();
        PrevShowSection = null;

        threadInfo = new ThreadData();
        oneMain = false;
        isValidThreadInfo = true;
        numPayloadParallax = -1;
        numParallelParallax = -1;

        EventHandler<MouseEvent> threadStatEventHandlerMouseClick = new EventHandler<MouseEvent>() {
                            @Override
                            public void handle(MouseEvent event) {
                                MouseClickEventHandle(event);
                                event.consume();
                            }
                        };

        this.setOnMouseClicked(threadStatEventHandlerMouseClick);
        StatTableData.ClearLastIterInEachThreadStat();
    }

    public List<ISection> GetListRectangles(){
        return listRectangles;
    }

    public static void BindStatTableData(StatTableData statisticTableData){
        statTableData = statisticTableData;
    }

    public ThreadData GetThreadInfo(){
        return threadInfo;
    }

    public boolean IsValidDataInThreadInfo(){
        return isValidThreadInfo;
    }

    private boolean AddMain(final ISection sec){
        if(oneMain == false && threadId == 0){
            System.out.println("AddMain(): AddMain");
            oneMain = true;
            threadInfo.SetTimeOfWork(sec.GetEndTime() - sec.GetStartTime());   
        }else{
            //Here can be only one MAIN! 
            System.out.println("AddMain(): Here can be only one MAIN!");
            isValidThreadInfo = false;
            return false;
        }
        return true;
    }

    private boolean AddParallel(final ISection sec){
        System.out.println("AddParallel(): AddParallel");
        if(threadId != 0){
            return true;
        }

        if(numParallelParallax == -1){
            numParallelParallax = sec.GetNestedLevel();
        }else{
            if(numParallelParallax != sec.GetNestedLevel()){
                return false;
                //sec.setOpacity(0.3);
                //return true;
            }
        }

        // The time of several parallel sections is accumulated here
        threadInfo.SetTimeInParallel(threadInfo.GetTimeInParallel() + (sec.GetEndTime() - sec.GetStartTime()));
        return statTableData.AddParallel((Section)sec);
    }

    private boolean AddPayload(final ISection sec){
        if(numPayloadParallax == -1){
            numPayloadParallax = sec.GetNestedLevel();
        }else{
            if(numPayloadParallax != sec.GetNestedLevel()){
                return false;
                //sec.setOpacity(0.5);
                //return true;
            }
        }

        System.out.println("AddPayload(): AddPayload");
        // The time of several Payload sections is accumulated  here
        threadInfo.SetTimeOfPayload(threadInfo.GetTimeOfPayload() + (sec.GetEndTime() - sec.GetStartTime()));
        return statTableData.AddPayload(threadId, (Section)sec);
    }

    private void AddAtomic(final ISection sec){
        System.out.println("AddAtomic(): AddPayload");
        // The time of several Atomic sections is accumulated here
        threadInfo.SetTimeInAtomic(threadInfo.GetTimeInAtomic() + (sec.GetEndTime() - sec.GetStartTime()));   
    }

    public void AddSection(final Section sec){
        //we think that section has valid time, otherwise the section is not exist!
        boolean isAddOk = true;
        switch(sec.GetFuncName()){
            case "Main": 
                    isAddOk = AddMain(sec);
                    break;
            case "Parallel": 
                    isAddOk = AddParallel(sec);
                    break;
            case "PayloadFunc": 
                    isAddOk = AddPayload(sec);
                    break;
            //case "Barrier": 
            //        c = FUNC_RECT_COLOR[3];
            //        break;
            case "Single": 
                    break;
            case "Atomic":
                    AddAtomic(sec);
                    break;
            case "Critic": 
                    break;
            case "Loop_order": 
                    break;
            case "Ordered": 
                    break;
            case "Sections": 
                    break;
            case "Lock": 
                    break;                                    
            default:
                    break;            
        }
        if(isAddOk == false){
            return;
        }
        listRectangles.add(sec);
        this.getChildren().add(sec);
    }

    public void CalculateEnd(){
        threadInfo.CalculateTimeInSequence();
        threadInfo.CalculateSpeedUp();
    }

    public void CreateHiddenInfo(){
        for (ISection sec : listRectangles) {
            sec.CreateLabel(this);
            sec.CreateEDG(this);
        }
    }
}
