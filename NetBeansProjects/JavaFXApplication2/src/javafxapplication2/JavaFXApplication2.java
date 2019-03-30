/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.io.File;
import java.util.Map;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.stage.Stage;

import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.paint.Color;

import javafx.scene.control.ScrollPane;
import javafx.scene.control.ScrollPane.ScrollBarPolicy;
import javafx.scene.control.TextField;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.TransferMode;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.stage.FileChooser;

/**
 * @brief The main class to run "OpenMP App Analyzer" tool
 * @author HP
 */
public class JavaFXApplication2 extends Application  {
    private static final String STR_PROGRAM_TITLE = "OpenMP App Analyzer";
    private static final int W = 870;
    private static final int H = 820;
    
    private static final Color BACKGROUND_COLOR = Color.rgb(110, 110, 110);
    private static final Color BACKGROUND_STROKE_COLOR = Color.BLACK;
    private static final Color STRING_COLOR = Color.BLACK;

    private static final int WIDTH_BUTTON      = 120;

    private static final Font BUTTON_FONT       = Font.font("Tahoma", 14);
    private static final Font LABEL_FONT        = Font.font("Arial", FontWeight.BOLD, 14);
    private static final Font TEXT_FIELD_FONT   = Font.font("Times New Roman", 14);
    private static final String STYLE_TEXT_FIELD = "-fx-text-inner-color: black;";

    private static final int INPUT_PANEL_HBOX_SPASING = 30;
    private static final Insets INPUT_PANEL_HBOX_INSERTS = new Insets(0,10,0,10);
    private static final int INPUT_PANEL_VBOX_SPASING = 20;
    
    private static final int VBOX_SPASING = 20;
    private static final Insets VBOX_INSERTS = new Insets(10,0,0,0);

    private TextField selectedAppTextField;
    private TextField appParamTextField;
    private Legend legenda;
    private MetricBoard metricBoard;
    private StatisticWindow mainStatWindow;
    private StatTableView statTable;

    private static final String BACKGROUND_STYLE = "-fx-background-color: rgb(110, 2, 110);"
                                + "-fx-border-color: rgb(0,0,0);"
                                + "-fx-border-width: 0;";
        
    private static final String INFO_BOARD_SCROLLER_STYLE= "-fx-background-color: rgb(240,240,240);"
                                + "-fx-border-color: rgb(0,0,0);"
                                + "-fx-border-width: 1;";

    @Override
    public void start(Stage primaryStage){
        Group root = new Group();
        root.setStyle(BACKGROUND_STYLE);
        Scene scene = new Scene(root, W, H-12); // (-12) -> 0
        primaryStage.setTitle(STR_PROGRAM_TITLE);
        primaryStage.setScene(scene);
        primaryStage.show();
        //primaryStage.setMinHeight(H);
        //primaryStage.setMinWidth(W);
        primaryStage.setResizable(false);

        VBox mainVerticalBox = CreateMainBox(scene, primaryStage);

        root.setOnDragOver(mainStatWindow.eventDragOverHandler);
        root.setOnDragDropped(mainStatWindow.eventDragDroppedHandler);
        root.getChildren().add(mainVerticalBox);
    }

    private void Button1ActionPerformed(Stage primaryStage) {
        System.out.println("Button1 is pressed");

        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        File selectedFile = fileChooser.showOpenDialog(primaryStage);
        if (selectedFile != null) {
            final String strPath = selectedFile.getAbsolutePath();
            selectedAppTextField.setText(strPath);
            System.out.println("You have selected this app:" + strPath);
        } else {
            System.out.println("Open command cancelled by user.");
        }
    }
    
    private void Button2ActionPerformed(Stage primaryStage) {
        System.out.println("Button2 is pressed");

        final String strAppPath = selectedAppTextField.getText();
        final File file = new File(strAppPath);
        if(file == null) {
            System.out.println(" File: " + strAppPath + " is not found! ");
            return;
        }
        System.out.println(" File: " + strAppPath + " is found! ");
        System.out.println(" file.getName(): " + file.getName() + "");
        System.out.println(" file.getPath(): " + file.getPath() + "");
        System.out.println(" file.getAbsolutePath(): " + file.getAbsolutePath() + "");
        String strProgName = file.getName();

        String strAppArgs = "";
        if(!appParamTextField.getText().isEmpty()) {
            strAppArgs = appParamTextField.getText();
            System.out.println("Arguments of your program: " + strAppArgs);
        }

        DataAnalyzer.RemovePreviousResultFiles("");
                
        System.out.println("=================StartTrace==================");
        primaryStage.setTitle("Tracing app " + strProgName);
        Trace trace = new Trace(file.getAbsolutePath(), strAppArgs);
        trace.Start();
        primaryStage.setTitle("Tracing " + strProgName + " - is completed!");

        System.out.println("=================StartAnalyzeData============");
        primaryStage.setTitle("Start analyzing the data" + strProgName);

        final String strPinToolOutfile = DataAnalyzer.DEFAULT_PINTOOL_OUT_PATH+DataAnalyzer.DEFAULT_PINTOOL_OUT_CONF;
        DataAnalyzer analyzeData = new DataAnalyzer();
        Map TableStatisticaThreads = analyzeData.Start(strPinToolOutfile);
        primaryStage.setTitle("Analysis " + strProgName + " is completed!");
        if(TableStatisticaThreads == null)
        {
            System.out.println("Analyze of Data is finished with false");
            return;
        }
        
        System.out.println("=================StartCreateBarChart=========");
        primaryStage.setTitle("Start building diagrams");
        mainStatWindow.Show(TableStatisticaThreads);
        primaryStage.setTitle("The end building diagrams");
    }

    private VBox CreateMainBox(Scene scene, Stage primaryStage){
        // All the visual components of the Java application are created here.
        HBox hbAppChoice = CreateAppChoiceBox((int) scene.getWidth(), primaryStage);
        HBox hbAppArgs = CreateAppArgsBox((int) scene.getWidth(), primaryStage);

        legenda = new Legend();
   
        metricBoard = new MetricBoard(null);

        statTable = new StatTableView(W);
     
        ScrollPane metricBoardScroller = new ScrollPane();
        //metricBoardScroller.setMinViewportHeight(300);      
        metricBoardScroller.setVbarPolicy(ScrollBarPolicy.NEVER);
        metricBoardScroller.setHbarPolicy(ScrollPane.ScrollBarPolicy.AS_NEEDED);
        //metricBoardScroller.setMinSize(mainStatWindow.rectCanvas.getWidth(), mainStatWindow.rectCanvas.getHeight());
        metricBoardScroller.setMinSize((int) scene.getWidth(),MetricBoard.H);
        metricBoardScroller.setMaxSize((int) scene.getWidth(),MetricBoard.H);
        metricBoardScroller.setContent(metricBoard);
        metricBoardScroller.setStyle(INFO_BOARD_SCROLLER_STYLE);

        mainStatWindow = new StatisticWindow(scene, legenda, metricBoard, statTable);
        //mainStatWindow.setMinSize((int) scene.getWidth(), SettingWindow.H);
        //mainStatWindow.setMaxSize((int) scene.getWidth(), SettingWindow.H);

        // Add all visual components to VBoxes
        VBox vbInputPanel = new VBox();
        vbInputPanel.setSpacing(INPUT_PANEL_VBOX_SPASING);
        //vbInputPanel.setMaxWidth(SettingWindow.WIDTH);
        vbInputPanel.getChildren().addAll(hbAppChoice, hbAppArgs);

        VBox vbOutputPanel = new VBox();
        //vbOutputPanel.setMaxWidth(SettingWindow.WIDTH);        
        vbOutputPanel.getChildren().addAll(legenda, metricBoardScroller, mainStatWindow, statTable);

        VBox mainVerticalBox = new VBox();
        mainVerticalBox.setSpacing(VBOX_SPASING);
        mainVerticalBox.setPadding(VBOX_INSERTS);
        mainVerticalBox.setMaxWidth(SettingWindow.WIDTH);
        mainVerticalBox.getChildren().addAll(vbInputPanel, vbOutputPanel);
        return mainVerticalBox;
    }
    
    private HBox CreateAppChoiceBox(int sceneWidth, Stage stageForButtonAction){
        Button browserButton = new Button();
        browserButton.setText("Browse");
        browserButton.setFont(BUTTON_FONT);
        browserButton.setPrefWidth(WIDTH_BUTTON);

        EventHandler<ActionEvent> browserButtonHandler = new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                System.out.println("Browser Button is pressed");
                Button1ActionPerformed(stageForButtonAction);
            }
        };
        browserButton.setOnAction(browserButtonHandler);

        Label textFieldLabel = new Label("Choose OpenMP app for analyze:");
        textFieldLabel.setFont(LABEL_FONT);
        textFieldLabel.setTextFill(STRING_COLOR);

        selectedAppTextField = new TextField();
        selectedAppTextField.setFont(TEXT_FIELD_FONT);
        selectedAppTextField.setStyle(STYLE_TEXT_FIELD);
        selectedAppTextField.setOnDragOver(new EventHandler<DragEvent>(){
             @Override
             public void handle(DragEvent event){
                Dragboard db = event.getDragboard();
                if(db.hasString() || db.hasFiles()){
                    event.acceptTransferModes(TransferMode.COPY);
                    event.consume();
                }
             }
         });
        selectedAppTextField.setOnDragDropped(new EventHandler<DragEvent>(){
             @Override
             public void handle(DragEvent event){
                Dragboard db = event.getDragboard();
                if(db.hasString()){
                    selectedAppTextField.setText(db.getString());
                }else if(db.hasFiles()){                          
                    System.out.println("db.getFiles().size() = " + db.getFiles().size());
                    File file = db.getFiles().get(0);
                    selectedAppTextField.setText(file.getAbsolutePath());  
                }     
                event.setDropCompleted(true);
                event.consume();
             }
         });

        HBox hb = new HBox();
        hb.setSpacing(INPUT_PANEL_HBOX_SPASING);
        HBox.setHgrow(selectedAppTextField, Priority.ALWAYS);
        hb.setMaxWidth(sceneWidth);
        hb.setAlignment(Pos.CENTER);
        hb.setPadding(INPUT_PANEL_HBOX_INSERTS);
        hb.getChildren().addAll(textFieldLabel, selectedAppTextField, browserButton);
        return hb;
    }

    private HBox CreateAppArgsBox(int sceneWidth, Stage stageForButtonAction){
        Button startButton = new Button();
        startButton.setText("Start analyze");
        startButton.setFont(BUTTON_FONT);
        startButton.setPrefWidth(WIDTH_BUTTON);

        EventHandler<ActionEvent> startButtonHandler = new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                System.out.println("Start Button is pressed");
                //event.consume();
                Button2ActionPerformed(stageForButtonAction);
                stageForButtonAction.show();
            }
        };
        startButton.setOnAction(startButtonHandler);

        Label textFieldLabel = new Label("args: ");
        textFieldLabel.setFont(LABEL_FONT);
        textFieldLabel.setTextFill(STRING_COLOR);

        appParamTextField = new TextField();
        appParamTextField.setFont(TEXT_FIELD_FONT);
        appParamTextField.setStyle(STYLE_TEXT_FIELD);

        HBox hb = new HBox();
        hb.setSpacing(INPUT_PANEL_HBOX_SPASING);
        HBox.setHgrow(appParamTextField, Priority.ALWAYS);
        hb.setMaxWidth(sceneWidth);
        hb.setAlignment(Pos.CENTER);
        hb.setPadding(INPUT_PANEL_HBOX_INSERTS);
        hb.getChildren().addAll(textFieldLabel, appParamTextField, startButton);
        return hb;
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        launch(args);
    }
}
