/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.ArrayList;
import java.util.Map;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Arc;
import javafx.scene.shape.ArcType;
import javafx.scene.shape.StrokeType;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

/**
 *
 * @author HP
 */
public class InfoBoard extends Group {

    public static final int START_DEG = 90;
    public static final int DIRECTION_RUN = -1;
    public static final int BIG_CIRCLE_SIZE = 40;
    public static final int SMALL_CIRCLE_SIZE = (int) (BIG_CIRCLE_SIZE * 0.9);
    public static final int CIRCLE_SROKE_WIDTH = (int) (BIG_CIRCLE_SIZE * 0.3);

    private static final int H_LABEL_INTERVAL = 30;
    private static final int BETWEEN_CI_HBOX_SPASING = 30;
    private static final Insets BETWEEN_CI_HBOX_INSERTS = new Insets(0, 0, 0, 0);

    private static final double COEF_ARC_PART_LENGTH = DIRECTION_RUN * (360.0 / 100.0);
    private static final int ARCPART_INIT_LENGTH = 180;
    private static final int LABELPART_INIT_TIMENUMERATOR = 0;
    private static final int LABELPART_INIT_TIMEDENUMERATOR = 100;
    private static final double ALIGNMENT_OFFSETX_TIMELABELPART = -H_LABEL_INTERVAL / 1.05;
    private static final double ALIGNMENT_OFFSETY_TIMELABELPART = -H_LABEL_INTERVAL / 2.5;
    private static final double TIMELABELALL_INIT_LENGTH = 0;

    private static final int CIRCLE_FORMAT_PRECISION = 1;
    private static final Color STRING_COLOR = Color.BLACK;
    private static final Font TEXT_FONT = Font.font("Arial", FontWeight.BOLD, 16);
    private static final Font VALUE_FONT_PART = Font.font("Tahoma", FontWeight.BOLD, 18);
    private static final Font VALUE_FONT_ALL = Font.font("Tahoma", FontWeight.BOLD, 18);

    //public static final int W = 320;
    public static final int H = 165;

    private Map hashTableThreadStat;
    private HBox hb;
    private CircleInfo circleInfoMain;
    private ArrayList<CircleInfo> arrayCircleInfoThreads;
    private int unit = 1;

    InfoBoard(Map hashTable_ThreadStat) {
        super();
        hashTableThreadStat = hashTable_ThreadStat;

        arrayCircleInfoThreads = new ArrayList<CircleInfo>();
        circleInfoMain = new CircleInfo(Color.BLUE, Color.RED, "Total");

        hb = new HBox();
        hb.setSpacing(BETWEEN_CI_HBOX_SPASING);
        hb.setPadding(BETWEEN_CI_HBOX_INSERTS);
        hb.getChildren().addAll(circleInfoMain);
        this.getChildren().add(hb);
    }

    public void SetHashTable(Map hashTable_ThreadStat) {
        hashTableThreadStat = hashTable_ThreadStat;
    }

    public HBox GetBackground() {
        return hb;
    }

    public void UpdateUnit(double timeWidth) {
        unit = Legend.GetUnit(timeWidth); //(statTableElem.GetParallelDelta());
    }

    ThreadInfo GetThreadInfo(int tid) {
        ThreadStat TMP_threadStat = (ThreadStat) hashTableThreadStat.get(tid);
        if (TMP_threadStat != null) {
            if (TMP_threadStat.IsValidDataInThreadInfo()) {
                ThreadInfo TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                System.out.println("!!!!!! TMP_threadStat.threadId =  " + TMP_threadStat.threadId);
                return TMP_threadInfo;
            }
        } else {
            System.out.println(" TMP_threadStat == null ");
        }
        return null;
    }

    // this is not used now! see ShowMetrics 
    public void ShowStatForAllThreadWithFullClear() { //ShowStatForAllThreadWithFullClear
        if (hashTableThreadStat == null) {
            System.out.println(" hashTableThreadStat == null ");
            return;
        }

        ArrayList<Integer> ArrThreadId = new ArrayList<>(hashTableThreadStat.keySet());

        hb.getChildren().clear();
        hb.getChildren().add(circleInfoMain);

        final int mainThreadID = ArrThreadId.get(0);
        ThreadInfo mainThreadInfo = GetThreadInfo(mainThreadID);
        if (mainThreadInfo == null) {
            System.out.println(" mainThreadInfo == null ");
            return;
        }
        final double sharedParallelTime = mainThreadInfo.GetTimeParallel();
        this.UpdateInfoMainCircle(sharedParallelTime, mainThreadInfo.GetTimeOfWork());

        for (int i = 0; i < ArrThreadId.size(); i++) {
            ThreadStat TMP_threadStat = (ThreadStat) hashTableThreadStat.get(ArrThreadId.get(i));
            if (TMP_threadStat != null) {
                if (TMP_threadStat.IsValidDataInThreadInfo()) {
                    ThreadInfo TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                    System.out.println("!!!!!! TMP_threadStat.threadId =  " + TMP_threadStat.threadId);
                    CircleInfo ciTMP = new CircleInfo(Color.RED, Color.GREEN, "TH" + String.valueOf(TMP_threadStat.threadId));
                    hb.getChildren().add(ciTMP);
                    ciTMP.UpdateCI(TMP_threadInfo.GetTimePayload(), sharedParallelTime);//TMP_threadInfo.GetTimeParallel());
                }
            } else {
                System.out.println(" TMP_threadStat == null ");
            }
        }
    }

    public void ShowMetrics() {
        if (hashTableThreadStat == null) {
            System.out.println(" hashTableThreadStat == null ");
            return;
        }

        ArrayList<Integer> ArrThreadId = new ArrayList<>(hashTableThreadStat.keySet());

        final int mainThreadID = ArrThreadId.get(0);
        ThreadInfo mainThreadInfo = GetThreadInfo(mainThreadID);
        if (mainThreadInfo == null) {
            System.out.println(" mainThreadInfo == null ");
            return;
        }
        final double sharedParallelTime = mainThreadInfo.GetTimeParallel();
        this.UpdateInfoMainCircle(sharedParallelTime, mainThreadInfo.GetTimeOfWork());

        System.out.println("  1************************** remove unnecessary items if they are there ************************** ");
        if (arrayCircleInfoThreads.size() - 1 > ArrThreadId.size()) {
            final int arrayCircleInfoThreads_size = arrayCircleInfoThreads.size();
            for (int i = ArrThreadId.size(); i < arrayCircleInfoThreads_size; i++) {
                arrayCircleInfoThreads.remove(ArrThreadId.size());
                System.out.println(" remove(i) =" + i);
            }
            System.out.println(" hb.getChildren().size() =" + hb.getChildren().size());
            hb.getChildren().remove(1 + ArrThreadId.size(), 1 + arrayCircleInfoThreads_size);
            System.out.println(" hb.getChildren().size() =" + hb.getChildren().size());
        }
        System.out.println("  2************************** update items that are there ************************** size = " + arrayCircleInfoThreads.size());

        for (int i = 0; i < arrayCircleInfoThreads.size(); i++) {
            ThreadStat TMP_threadStat = (ThreadStat) hashTableThreadStat.get(ArrThreadId.get(i));
            if (TMP_threadStat != null) {
                if (TMP_threadStat.IsValidDataInThreadInfo()) {
                    ThreadInfo TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                    System.out.println("!!!!!! TMP_threadStat.threadId =  " + TMP_threadStat.threadId);
                    CircleInfo ci = (CircleInfo) arrayCircleInfoThreads.get(i);
                    System.out.println("ci == null");

                    ci.UpdateThreadIdLabel("TH" + String.valueOf(TMP_threadStat.threadId));
                    ci.UpdateCI(TMP_threadInfo.GetTimePayload(), sharedParallelTime);//TMP_threadInfo.GetTimeParallel());
                }
            } else {
                System.out.println(" TMP_threadStat == null ");
            }
        }
        System.out.println("  3************************** create the necessary items if it necessary **************************  ");
        for (int i = arrayCircleInfoThreads.size(); i < ArrThreadId.size(); i++) {

            ThreadStat TMP_threadStat = (ThreadStat) hashTableThreadStat.get(ArrThreadId.get(i));
            if (TMP_threadStat != null) {
                if (TMP_threadStat.IsValidDataInThreadInfo()) {
                    ThreadInfo TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                    System.out.println("!!!!!! TMP_threadStat.threadId =  " + TMP_threadStat.threadId);
                    System.out.println("ci == null");
                    CircleInfo ci = new CircleInfo(Color.RED, Color.GREEN, "TH" + String.valueOf(TMP_threadStat.threadId));
                    hb.getChildren().add(ci);
                    arrayCircleInfoThreads.add(ci);
                    ci.UpdateCI(TMP_threadInfo.GetTimePayload(), sharedParallelTime);//TMP_threadInfo.GetTimeParallel());
                }
            } else {
                System.out.println(" TMP_threadStat == null ");
            }
        }
        System.out.println("  4************************** The end of ShowMetrics ************************** ");
    }

    private void UpdateInfoMainCircle(double parallelTime, double overheadTime) {
        circleInfoMain.UpdateCI(parallelTime, overheadTime);
    }

    private String PercentageValue(double numerator, double denominator) {
        final String strFormat = "%." + String.valueOf(CIRCLE_FORMAT_PRECISION) + "f";
        String str = "" + String.format(strFormat, (numerator / denominator) * 100) + "%";
        return str;
    }

    public class CircleInfo extends VBox {

        private Arc arcAll;
        private Arc arcPart;
        private Label timeLabelAll;
        private Label timeLabelPart;
        private Label strThread;

        //private static final int SHOW_PRECISION = 1;
        private String TimeToString(double time) {
            return Legend.GetFormatTime(time, unit);//, SHOW_PRECISION);
        }

        public void UpdateThreadIdLabel(String StrAboveCircle) {
            strThread.setText(StrAboveCircle);
        }

        public void UpdateCI(double spentTime, double allTime) {
            //System.out.println("parallelTime="+parallelTime);
            //System.out.println("overheadTime="+overheadTime);
            arcPart.setLength(COEF_ARC_PART_LENGTH * ((spentTime / allTime) * 100.0));
            timeLabelPart.setText(PercentageValue(spentTime, allTime));
            timeLabelAll.setText(TimeToString(allTime)); ///
            //timeLabelAll.setText(Legend.GetFormatTime(overheadTime)); ///
        }

        CircleInfo(Color c_all, Color c_part, String StrAboveCircle) {
            super();

            int coordCenter = BIG_CIRCLE_SIZE + CIRCLE_SROKE_WIDTH;

            strThread = new Label(StrAboveCircle);
            strThread.setFont(TEXT_FONT);
            strThread.setTextFill(STRING_COLOR);
            //strThread.setLayoutX(coordCenter-H_LABEL_INTERVAL/1.05);
            //strThread.setLayoutY(coordCenter -BIG_CIRCLE_SIZE/2 - 2*H_LABEL_INTERVAL);
            //this.getChildren().add(strThread);

            arcAll = new Arc(coordCenter, coordCenter, BIG_CIRCLE_SIZE, BIG_CIRCLE_SIZE, START_DEG, DIRECTION_RUN * 360); // SettingWindow.WIDTH
            arcAll.setType(ArcType.OPEN);
            arcAll.setStrokeWidth(CIRCLE_SROKE_WIDTH);
            arcAll.setStroke(c_all);//CORAL
            arcAll.setStrokeType(StrokeType.CENTERED);
            arcAll.setFill(null);
            //this.getChildren().add(arcAll);

            arcPart = new Arc(coordCenter, coordCenter, SMALL_CIRCLE_SIZE, SMALL_CIRCLE_SIZE, START_DEG, DIRECTION_RUN * ARCPART_INIT_LENGTH); // SettingWindow.WIDTH
            arcPart.setType(ArcType.OPEN);
            arcPart.setStrokeWidth(CIRCLE_SROKE_WIDTH);
            arcPart.setStroke(c_part);
            arcPart.setStrokeType(StrokeType.CENTERED);
            arcPart.setFill(null);
            //this.getChildren().add(arcPart);

            timeLabelPart = new Label(PercentageValue(LABELPART_INIT_TIMENUMERATOR, LABELPART_INIT_TIMEDENUMERATOR));//+"-"+Legend.GetformatTime(12));
            timeLabelPart.setFont(VALUE_FONT_PART);
            timeLabelPart.setTextFill(STRING_COLOR);
            timeLabelPart.setLayoutX(coordCenter + ALIGNMENT_OFFSETX_TIMELABELPART);
            timeLabelPart.setLayoutY(coordCenter + ALIGNMENT_OFFSETY_TIMELABELPART);
            //this.getChildren().add(timeLabelPart);
            //arc1.setLayoutX(10);
            //arc1.setLayoutY(10);

            timeLabelAll = new Label(Legend.GetFormatTime(TIMELABELALL_INIT_LENGTH));
            timeLabelAll.setFont(VALUE_FONT_ALL);
            timeLabelAll.setTextFill(STRING_COLOR);
            //timeLabelAll.setLayoutX(coordCenter-BIG_CIRCLE_SIZE);
            //timeLabelAll.setLayoutY(coordCenter +BIG_CIRCLE_SIZE/2 + H_LABEL_INTERVAL);
            //this.getChildren().add(timeLabelAll);
            //arc.setLayoutX(10);
            //arc.setLayoutY(10);

            Pane pane = new Pane();
            pane.getChildren().addAll(arcAll, arcPart, timeLabelPart);

            //VBox vb = new VBox();
            //vb.setSpacing(VBOX_SPASING);
            //vb.setPadding(VBOX_INSERTS);
            this.getChildren().addAll(strThread, pane, timeLabelAll);
            this.setAlignment(Pos.CENTER);
            //this.getChildren().add(vb);
        }
    }

};
