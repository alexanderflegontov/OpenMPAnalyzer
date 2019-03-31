/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

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
 * @brief The class is a board for representing of circular metric
 * @author hp
 */
public class MetricBoard extends Group {

    public static final int START_DEG = 90;
    public static final int DIRECTION_RUN = -1;
    public static final int BIG_CIRCLE_SIZE = 40;
    public static final int SMALL_CIRCLE_SIZE = (int) (BIG_CIRCLE_SIZE * 0.9);
    public static final int CIRCLE_SROKE_WIDTH = (int) (BIG_CIRCLE_SIZE * 0.3);

    private static final double COEF_ARC_PART_LENGTH = DIRECTION_RUN * (360.0 / 100.0);
    private static final int ARCPART_INIT_LENGTH = 180;
    private static final int LABELPART_INIT_TIMENUMERATOR = 0;
    private static final int LABELPART_INIT_TIMEDENUMERATOR = 100;
    private static final int H_LABEL_INTERVAL = 30;
    private static final double ALIGNMENT_OFFSETX_TIMELABELPART = -H_LABEL_INTERVAL / 1.05;
    private static final double ALIGNMENT_OFFSETY_TIMELABELPART = -H_LABEL_INTERVAL / 2.5;
    private static final double TIMELABELALL_INIT_LENGTH = 0;

    private static final int CIRCLE_FORMAT_PRECISION = 1;
    private static final Color STRING_COLOR = Color.BLACK;
    private static final Font TEXT_FONT = Font.font("Arial", FontWeight.BOLD, 16);
    private static final Font VALUE_FONT_PART = Font.font("Tahoma", FontWeight.BOLD, 18);
    private static final Font VALUE_FONT_ALL = Font.font("Tahoma", FontWeight.BOLD, 18);
    
    private static final int BETWEEN_CI_HBOX_SPASING = 30;
    private static final Insets BETWEEN_CI_HBOX_INSERTS = new Insets(0, 0, 0, 0);
    
    public static final int H = 165;

    private Map hashTableThreadStat;
    private final HBox hb;
    private final CircleInfo circleInfoMain;
    private final ArrayList<CircleInfo> arrayCircleInfoThreads;
    private int scalar_unit = 1;

    MetricBoard(Map hashTable_ThreadStat) {
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

    public void UpdateUnit(double timeWidth) {
        scalar_unit = Timeline.GetUnit(timeWidth);
    }

    ThreadData GetThreadInfo(int tid) {
        ThreadView TMP_threadStat = (ThreadView) hashTableThreadStat.get(tid);
        if (TMP_threadStat != null) {
            if (TMP_threadStat.IsValidDataInThreadInfo()) {
                ThreadData TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                return TMP_threadInfo;
            }
        } else {
            System.out.println("MetricBoard can't find thread statistics for tid " + Integer.toString(tid));
        }
        return null;
    }

    // this function is not used now! see ShowMetrics 
    public void ShowStatForAllThreadWithFullClear() {
        if (hashTableThreadStat == null) {
            System.out.println("HashTable of thread statistics is null");
            return;
        }

        ArrayList<Integer> ArrThreadId = new ArrayList<>(hashTableThreadStat.keySet());

        hb.getChildren().clear();
        hb.getChildren().add(circleInfoMain);

        final int mainThreadID = ArrThreadId.get(0);
        ThreadData mainThreadInfo = GetThreadInfo(mainThreadID);
        if (mainThreadInfo == null) {
            System.out.println("Statistics of main thread is null");
            return;
        }
        final double sharedParallelTime = mainThreadInfo.GetTimeInParallel();
        this.UpdateInfoMainCircle(sharedParallelTime, mainThreadInfo.GetTimeOfWork());

        for (int i = 0; i < ArrThreadId.size(); i++) {
            ThreadView TMP_threadStat = (ThreadView) hashTableThreadStat.get(ArrThreadId.get(i));
            if (TMP_threadStat != null) {
                if (TMP_threadStat.IsValidDataInThreadInfo()) {
                    ThreadData TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                    CircleInfo ciTMP = new CircleInfo(Color.RED, Color.GREEN, "TH" + String.valueOf(TMP_threadStat.threadId));
                    hb.getChildren().add(ciTMP);
                    ciTMP.UpdateCI(TMP_threadInfo.GetTimeOfPayload(), sharedParallelTime);
                }
            } else {
                System.out.println("MetricBoard can't find thread statistics for tid " + Integer.toString(i));
            }
        }
    }

    public void ShowMetrics(Map hashTableThreadStat) {
        if (hashTableThreadStat == null) {
            System.out.println("HashTable of thread statistics is null");
            return;
        }

        ArrayList<Integer> ArrThreadId = new ArrayList<>(hashTableThreadStat.keySet());

        final int mainThreadID = ArrThreadId.get(0);
        ThreadData mainThreadInfo = GetThreadInfo(mainThreadID);
        if (mainThreadInfo == null) {
            System.out.println("Statistics of main thread is null");
            return;
        }
        final double sharedParallelTime = mainThreadInfo.GetTimeInParallel();
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
        System.out.println("  2************************** update items that are there ************************** ");

        for (int i = 0; i < arrayCircleInfoThreads.size(); i++) {
            ThreadView TMP_threadStat = (ThreadView) hashTableThreadStat.get(ArrThreadId.get(i));
            if (TMP_threadStat != null) {
                if (TMP_threadStat.IsValidDataInThreadInfo()) {
                    ThreadData TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                    CircleInfo ci = (CircleInfo) arrayCircleInfoThreads.get(i);

                    ci.UpdateThreadIdLabel("TH" + String.valueOf(TMP_threadStat.threadId));
                    ci.UpdateCI(TMP_threadInfo.GetTimeOfPayload(), sharedParallelTime);//TMP_threadInfo.GetTimeInParallel());
                }
            } else {
                System.out.println("MetricBoard can't find thread statistics for tid " + Integer.toString(i));
            }
        }
        System.out.println("  3************************** create the necessary items if it necessary ************************** ");
        for (int i = arrayCircleInfoThreads.size(); i < ArrThreadId.size(); i++) {

            ThreadView TMP_threadStat = (ThreadView) hashTableThreadStat.get(ArrThreadId.get(i));
            if (TMP_threadStat != null) {
                if (TMP_threadStat.IsValidDataInThreadInfo()) {
                    ThreadData TMP_threadInfo = TMP_threadStat.GetThreadInfo();
                    CircleInfo ci = new CircleInfo(Color.RED, Color.GREEN, "TH" + String.valueOf(TMP_threadStat.threadId));
                    hb.getChildren().add(ci);
                    arrayCircleInfoThreads.add(ci);
                    ci.UpdateCI(TMP_threadInfo.GetTimeOfPayload(), sharedParallelTime);//TMP_threadInfo.GetTimeInParallel());
                }
            } else {
                System.out.println("MetricBoard can't find thread statistics for tid " + Integer.toString(i));
            }
        }
        System.out.println("  4************************** The end of ShowMetrics ************************** ");
    }

    private void UpdateInfoMainCircle(double parallelTime, double overheadTime) {
        circleInfoMain.UpdateCI(parallelTime, overheadTime);
    }

    private String PercentageValue(double numerator, double denominator) {
        final String strFormat = "%." + String.valueOf(CIRCLE_FORMAT_PRECISION) + "f";
        return "" + String.format(strFormat, (numerator / denominator) * 100) + "%";
    }

    /**
     * @brief The class to represent circular metrics
     * @author hp
     */
    public class CircleInfo extends VBox {

        private final Arc arcWhole;
        private final Arc arcPart;
        private final Label timeLabelWhole;
        private final Label timeLabelPart;
        private final Label strThread;

        private String TimeToString(double time){
            return Timeline.GetFormatTime(time, scalar_unit);
        }

        public void UpdateThreadIdLabel(final String StrAboveCircle) {
            strThread.setText(StrAboveCircle);
        }

        public void UpdateCI(double spentTime, double wholeTime) {
            arcPart.setLength(COEF_ARC_PART_LENGTH * ((spentTime / wholeTime) * 100.0));
            timeLabelPart.setText(PercentageValue(spentTime, wholeTime));
            timeLabelWhole.setText(TimeToString(wholeTime));
        }

        CircleInfo(Color c_whole, Color c_part, String StrAboveCircle) {
            super();

            final int coordCenter = BIG_CIRCLE_SIZE + CIRCLE_SROKE_WIDTH;

            strThread = new Label(StrAboveCircle);
            strThread.setFont(TEXT_FONT);
            strThread.setTextFill(STRING_COLOR);

            arcWhole = new Arc(coordCenter, coordCenter, BIG_CIRCLE_SIZE, BIG_CIRCLE_SIZE, START_DEG, DIRECTION_RUN * 360);
            arcWhole.setType(ArcType.OPEN);
            arcWhole.setStrokeWidth(CIRCLE_SROKE_WIDTH);
            arcWhole.setStroke(c_whole);
            arcWhole.setStrokeType(StrokeType.CENTERED);
            arcWhole.setFill(null);

            arcPart = new Arc(coordCenter, coordCenter, SMALL_CIRCLE_SIZE, SMALL_CIRCLE_SIZE, START_DEG, DIRECTION_RUN * ARCPART_INIT_LENGTH); // SettingWindow.WIDTH
            arcPart.setType(ArcType.OPEN);
            arcPart.setStrokeWidth(CIRCLE_SROKE_WIDTH);
            arcPart.setStroke(c_part);
            arcPart.setStrokeType(StrokeType.CENTERED);
            arcPart.setFill(null);

            timeLabelPart = new Label(PercentageValue(LABELPART_INIT_TIMENUMERATOR, LABELPART_INIT_TIMEDENUMERATOR));//+"-"+Legend.GetformatTime(12));
            timeLabelPart.setFont(VALUE_FONT_PART);
            timeLabelPart.setTextFill(STRING_COLOR);
            timeLabelPart.setLayoutX(coordCenter + ALIGNMENT_OFFSETX_TIMELABELPART);
            timeLabelPart.setLayoutY(coordCenter + ALIGNMENT_OFFSETY_TIMELABELPART);

            timeLabelWhole = new Label(Timeline.GetFormatTime(TIMELABELALL_INIT_LENGTH));
            timeLabelWhole.setFont(VALUE_FONT_ALL);
            timeLabelWhole.setTextFill(STRING_COLOR);

            Pane pane = new Pane();
            pane.getChildren().addAll(arcWhole, arcPart, timeLabelPart);

            this.getChildren().addAll(strThread, pane, timeLabelWhole);
            this.setAlignment(Pos.CENTER);
        }
    }
};
