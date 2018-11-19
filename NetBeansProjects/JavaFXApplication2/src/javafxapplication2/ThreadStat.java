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
 *
 * @author HP
 */
    //HashSet<String> allfunctiosHashSet;
    class ThreadStat extends Group{
        public final int threadId;

        private List<Section> listRectangles;
        private Section PrevShowSection;

        private ThreadInfo threadInfo;
        private static boolean oneMain;
        private boolean isValidThreadInfo;
        //static HashSet<String> allfunctiosHashSet  = new HashSet<>();
        
        private static StatTableData statTableData;
        private int numPayloadParallax;
        private int numParallelParallax;

        public void MouseClickEventHandle(MouseEvent event) {
            System.out.println("+++++ threadStat "+String.valueOf(this.threadId)+" +++++");
            MouseButton button = event.getButton();


            //if(infoBoard != null){
                //infoBoard.ShowStatForThread(threadStat.threadId);
            //}

            Section rect = this.FindTouchRect(event.getX(), event.getY());
            //threadStat.HideExtraInfo(rect);
            if(rect == null){
                return;
            }
            /*
            boolean isNeedOnlyHide = this.HideExtraInfo(rect);
            if(isNeedOnlyHide){
                return;
            }*/

            if(button == MouseButton.PRIMARY){
                System.out.println("PRIMARY button clicked");

                if(rect != null){
                    //rect.InvertShowAll();
                    if(rect.GetSectionIndex() != -1){
                        StatTable.ShowParSection(rect.GetSectionIndex());
                    }
                }
                //label.setText("PRIMARY button clicked");
            }else if(button==MouseButton.SECONDARY){
                System.out.println("SECONDARY button clicked");
                //rect.setWidth(rect.prevWidth);
                //label.setText("SECONDARY button clicked");
                //rect.setWidth(2);

                //rect.setScaleX(1);
                //rect.setScaleY(1);
                //rect.HideLabel();

            }else if(button==MouseButton.MIDDLE){
                System.out.println("MIDDLE button clicked");
                //label.setText("MIDDLE button clicked");
            }
            System.out.println("Click to " + rect.GetFuncName());
        }
        
        ThreadStat(int thId){
            super();
            BackgroundTHStat();
            threadId = thId;
            listRectangles = new ArrayList<>();
            PrevShowSection = null;

            threadInfo = new ThreadInfo();
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
        
        private static final String BACKGROUND_TH_STYLE= "-fx-background-color: rgb(255,255,255);"
                            + "-fx-border-color: rgb(0,0,0);"
                            //+ "-fx-stroke-width: 2, 2, 2, 2;"
                            + "-fx-stroke-color: black;"
                            + "-fx-padding: 0,0,0,0;"
                            + "-fx-background-insets: 0;";
        
        public void BackgroundTHStat(){
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

            //Line lineBordRectBottom = new Line(0,H_STAT_RECT,W,H_STAT_RECT);
            //lineBordRectBottom.setStrokeWidth(1);
            //this.getChildren().add(lineBordRectBottom);
        }

        public List<Section> GetListRectangles(){
            return listRectangles;
        }
        
        public static void BindStatTableData(StatTableData statisticTableData){
            statTableData = statisticTableData;
        }
                
        public ThreadInfo GetThreadInfo(){
            return threadInfo;
        }
        
        public boolean IsValidDataInThreadInfo(){
            return isValidThreadInfo;
        }

        private boolean AddMain(Section sec){
            if(oneMain == false && threadId == 0){
                oneMain = true;
                System.out.println("AddMain(): AddMain");

                threadInfo.SetTimeOfWork(sec.GetEndTime() - sec.GetStartTime());   
            }else{
                //Here can be only one MAIN!!!! 
                System.out.println("AddMain(): Here can be only one MAIN!!!");
                isValidThreadInfo = false;
                return false;
            }
            return true;
        }
 
        
        private boolean AddParallel(Section sec){
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
            
            // The time of several parallel sections is accumulated
            threadInfo.SetTimeParallel(threadInfo.GetTimeParallel() + (sec.GetEndTime() - sec.GetStartTime()));
            return statTableData.AddParallel(sec);
        }
                
        private boolean AddPayload(Section sec){
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
            // The time of several Payload sections is accumulated
            threadInfo.SetTimePayload(threadInfo.GetTimePayload() + (sec.GetEndTime() - sec.GetStartTime()));
            return statTableData.AddPayload(threadId, sec);
        }

        private void AddAtomic(Section sec){
            System.out.println("AddAtomic(): AddPayload");
            // The time of several Atomic sections is accumulated
            threadInfo.SetTimeAtomic(threadInfo.GetTimeAtomic() + (sec.GetEndTime() - sec.GetStartTime()));   
        }

        
        public void AddSection(Section sec){
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
            //this.SettingMouseClick(sec);
        }
        
        // it is not used
        private void SettingMouseClick(Section rect){
            rect.setOnMouseClicked(new EventHandler<MouseEvent>() {
                                @Override
                                public void handle(MouseEvent event) {
                                    MouseButton button = event.getButton();

                                    boolean isNeedOnlyHide = HideExtraInfo(rect);
                                    if(isNeedOnlyHide){
                                        return;
                                    }
                                    
                                    if(null != button)switch (button) {
                                        case PRIMARY:
                                            System.out.println("PRIMARY button clicked");
                                            //rect.SetShowAll(true);
                                            rect.InvertShowAll();
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
                                    System.out.println("Click to " + rect.GetFuncName());
                                }
                            });
        }
        
        public void CalculateEnd(){
            threadInfo.CalculateTimeSequence();
            threadInfo.CalculateSpeedUp();
        }
        
        public void CreateHiddenInfo(){
            for (Section sec : listRectangles) {
                sec.CreateLabel(this);
                sec.CreateEDG(this);
            }
        }
        
        
        public boolean HideExtraInfo(Section sec){
            /*for(Iterator iter = listRectangles.iterator(); iter.hasNext();){
                Section sec = (Section) iter.next();
                sec.HideAll();
                sec.SetScaleEffect(false);
            }*/
            boolean isPrevStateShow = false;
            if(PrevShowSection != null){
                isPrevStateShow = PrevShowSection.SetShowAll(false);
            }
            // if we clicked on the same section and it was showed before(we need ONLY HIDE this section) 
            boolean isNeedOnlyHide =  (PrevShowSection == sec) && isPrevStateShow;
            PrevShowSection = sec;
            return isNeedOnlyHide;
        }
        
        // The start of search is the end List![Strategy]
        public Section FindTouchRect(double X, double Y){
            List<Section> TMP_threadStat = this.GetListRectangles();
            //boolean isFound = false;
            Section rect = null;
            //for(int i = TMP_threadStat.size()-1; i >=0; i--){
            //    rect = TMP_threadStat.get(i);
            for(ListIterator li = TMP_threadStat.listIterator(TMP_threadStat.size()); li.hasPrevious(); ){
                rect = (Section) li.previous();
                if(rect.contains(X, Y)){
                    //isFound = true;
                    return rect;
                    //break;
                }
            }
            return null;
        }
    }


    class ThreadInfo{
        private double threadTimeOfWork   = 0.0;
        private double threadTimeParallel = 0.0;
        private double threadTimeSequence = 0.0;
        private double threadTimePayload  = 0.0;
        private double threadSpeedUp      = 0.0;
        private double threadTimeAtomic   = 0.0;
        
        
        ThreadInfo(){
        }
        
        
        double GetTimeOfWork(){
            return threadTimeOfWork;
        }
        
        double GetTimeParallel(){
            return threadTimeParallel;
        }
        
        double GetTimeSequence(){
            return threadTimeSequence;
        }
        
        double GetSpeedUp(){
            return threadSpeedUp;
        }

        double GetTimePayload(){
            return threadTimePayload;
        }
        
        double GetTimeAtomic(){
            return threadTimeAtomic;
        }
        
        void SetTimeOfWork(double tmp){
            threadTimeOfWork = tmp;
        }
        
        void SetTimeParallel(double tmp){
            threadTimeParallel = tmp;
        }
        
        void SetTimeSequence(double tmp){
            threadTimeSequence = tmp;
        }

        void SetTimePayload(double tmp){
            threadTimePayload = tmp;
        }
        
        void SetTimeAtomic(double tmp){
            threadTimeAtomic = tmp;
        }
        
        void CalculateTimeSequence(){
            threadTimeSequence = threadTimeOfWork - threadTimeParallel;
        }
        
        void CalculateSpeedUp(){
            if(threadTimeSequence != 0.0){
                threadSpeedUp = threadTimeParallel/threadTimeSequence;
            }
        }
    }
    
