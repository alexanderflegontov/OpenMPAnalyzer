/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author HP
 */
///package example;


import java.util.ArrayList;
import javafx.application.Application;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollBar;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Slider;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.stage.Stage;

public class Main extends Application {

    public static void main(String[] args) {
        launch(args);
    }


    @Override
    public void start(Stage stage) {
        final int CANVAS_W = 500;
        final int CANVAS_H = 300;
        

        ZoomableCanvas canvas = new ZoomableCanvas(CANVAS_W, CANVAS_H) {
            @Override
            public void paint(GraphicsContext gc) {
                // Рисуем:
                /*
                gc.setFill(Color.LIGHTGREEN);
                gc.fillOval(60, 10, 180, 180);
                gc.setFill(Color.WHITE);
                gc.fillOval(100, 50, 100, 100);
                */
                ShowStat(gc);
            }
        };
        
        
        final String BACKGROUND_TH_STYLE= "-fx-background-color: rgb(255,255,255);"
                            + "-fx-border-color: rgb(0,0,0);"
                            + "-fx-border-width: 2;"
                            + "-fx-padding: 0,0,0,0;"
                            + "-fx-background-insets: 0;";
        
        final String BACKGROUND_TEST_STYLE= "-fx-background-color: rgb(240,240,240);"
                    + "-fx-border-color: rgb(0,0,0);"
                    + "-fx-border-width: 2;"
                    + "-fx-padding: 0,0,0,0;"
                    + "-fx-background-insets: 0;";
        //canvas.setStyle(BACKGROUND_TH_STYLE);

        Label zoomLabel = new Label();
        zoomLabel.textProperty().bind(canvas.zoomProperty().asString());
        Slider slider = new Slider(ZoomableCanvas.SCROLL_MIN, ZoomableCanvas.SCROLL_MAX, ZoomableCanvas.SCROLL_START);
        slider.valueProperty().bindBidirectional(canvas.zoomProperty());

        Label posXLabel = new Label();
        posXLabel.textProperty().bind(canvas.PosXProperty().asString());
        //Slider sliderH = new Slider(0.0, 500, ZoomableCanvas.POS_START_X);
        //sliderH.valueProperty().bindBidirectional(canvas.PosXProperty());
        
        Label posYLabel = new Label();
        posYLabel.textProperty().bind(canvas.PosYProperty().asString());
        //Slider sliderV = new Slider(0.0, 500, ZoomableCanvas.POS_START_Y);
        //sliderV.valueProperty().bindBidirectional(canvas.PosYProperty());
        
        
        ScrollBar scrollBarV = new ScrollBar();
        scrollBarV.setOrientation(Orientation.VERTICAL);
        scrollBarV.setMin(0);
        scrollBarV.setMax(1);
        scrollBarV.valueProperty().bindBidirectional(canvas.PosYProperty());
        
        ScrollBar scrollBarH = new ScrollBar();
        scrollBarH.setOrientation(Orientation.HORIZONTAL);
        
        //scrollBarH.setMinHeight(0);
        //scrollBarH.setMaxHeight(canvas.getWidth());
        scrollBarH.valueProperty().bindBidirectional(canvas.PosXProperty());
        scrollBarV.setMin(0);
        scrollBarV.setMax(1);

        Group mainGroup = new Group();
        final int PADDING = 10;
        final int OFFSET_X = PADDING*2;
        //final int OFFSET_Y = PADDING ;
        final int offsetXhbMainGroup = PADDING*10;
        final int offsetYhbMainGroup = PADDING*2;
        
        
        final int paddingCanvas = PADDING*2;

        
        ///// time line //////
        TimeNumber tn = new TimeNumber(CANVAS_W);
        mainGroup.getChildren().add(tn);
        tn.setLayoutX(offsetXhbMainGroup);
        tn.setLayoutY(offsetYhbMainGroup);
        canvas.SetEND_TIME(60.0);
        canvas.setTimeLineGroup(tn);
        
        
        ///// number of threads //////
        VBox numberThreadGroup = new VBox();
        for(int i=0; i<10; i++){
            Label labelTH = new Label("Thread("+String.valueOf(i)+")");
            labelTH.setPrefHeight(ZoomableCanvas.Hrect);
            //labelTH.setLayoutX(5);
            //labelTH.setLayoutY(ZoomableCanvas.HrectTH*i);
            /*
            //private SimpleDoubleProperty scrollLabelY = new SimpleDoubleProperty(0.0);
            labelTH.setOnScroll(new EventHandler<ScrollEvent>() {
                                @Override
                                public void handle(ScrollEvent event) {
                                    
                    System.out.println("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE!!!!!!!!!! " );
                    
                                    boolean state = labelTH.visibleProperty().getValue();
                                    if(labelTH.getLayoutY()>CANVAS_H){
                                        labelTH.setVisible(false);
                                    }else{
                                        labelTH.setVisible(true);
                                    }
                                }
                            });*/
            numberThreadGroup.getChildren().add(labelTH);
        }
        numberThreadGroup.setSpacing(ZoomableCanvas.HrectTH-ZoomableCanvas.Hrect-1); 
        ScrollPane scrollPane = new ScrollPane();
        //StatWindowScroller.setMinViewportHeight(300);
        scrollPane.setVbarPolicy(ScrollPane.ScrollBarPolicy.NEVER);
        scrollPane.setHbarPolicy(ScrollPane.ScrollBarPolicy.NEVER);
        scrollPane.setMinSize(offsetXhbMainGroup-3*PADDING, canvas.getHeight());
        scrollPane.setMaxSize(offsetXhbMainGroup-3*PADDING, canvas.getHeight());
        scrollPane.addEventFilter(ScrollEvent.SCROLL,new EventHandler<ScrollEvent>() {
            @Override
            public void handle(ScrollEvent event) {
                //if (event.getDeltaY() != 0) {
                    event.consume();
                //}
            }
        });
        scrollPane.setStyle("-fx-background-color: rgb(240,3,240);");
        scrollPane.setLayoutX(OFFSET_X);
        scrollPane.setLayoutY(offsetYhbMainGroup);
        scrollPane.setContent(numberThreadGroup);

        canvas.setNumThreadGroup(numberThreadGroup);
        
        
        VBox vb = new VBox(canvas, scrollBarH);//,     slider, zoomLabel,posXLabel, posYLabel);//sliderH, sliderV);
        //vb.setPadding(new Insets(PADDING,PADDING,PADDING,PADDING));
        vb.setStyle(BACKGROUND_TH_STYLE);
        
        HBox hbMainGroup = new HBox();
        hbMainGroup.getChildren().addAll(vb, scrollBarV);
        //hbMainGroup.setSpacing(PADDING);
        hbMainGroup.setPadding(new Insets(offsetYhbMainGroup,0,0,offsetXhbMainGroup));
        mainGroup.getChildren().addAll(scrollPane, hbMainGroup);
        

        Group root = new Group();

        
        
        VBox vbROOT_ONLY_TEST = new VBox(mainGroup);
        //vbROOT_ONLY_TEST.getChildren().addAll(slider, zoomLabel,posXLabel, posYLabel);
        //vbROOT_ONLY_TEST.setStyle(BACKGROUND_TEST_STYLE);
        //vbROOT_ONLY_TEST.setMinHeight(400);
        //vbROOT_ONLY_TEST.setMaxHeight(400);
        //root.getChildren().add(vbROOT_ONLY_TEST);
        root.getChildren().addAll(vbROOT_ONLY_TEST);
        
        Scene scene = new Scene(root);
        stage.setScene(scene);
        stage.show();
        
        stage.setWidth(1000);
        stage.setHeight(600);

        canvas.redraw();
    }
    



    


}