/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.HashSet;
import javafx.geometry.Insets;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.scene.layout.TilePane;
import javafx.scene.layout.HBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

/**
 * @brief The class is a presentation of Legend
 * @author hp
 */
public class Legend extends VBox {
    private final TilePane legendRoot;

    private static final int MAX_NUM_FUNC_IN_LEGEND = MappingFuncNames.NUM_FUNC;
    private static final Color LEGEND_SQUARE_SCOLOR = Color.BLACK;
    private static final Font LEGEND_FUNC_NAME_FONT = Font.font("Arial", FontWeight.BOLD, 14);
    private static final Color FUNC_NAME_LABEL_TEXT_COLOR = Color.BLACK;
    private static final String LEGEND_TILE_PANE_STYLE = "-fx-background-color: rgb(240,240,240);"
            + "-fx-border-color: rgb(0,0,0);"
            + "-fx-border-width: 1;";

    private static final int SQUARE_WH = 10;
    private static final int HBOX_SPASING = SQUARE_WH/2;
    private static final Insets HBOX_INSERTS = new Insets(5,5,5,5);
    private static final Insets TILEPANE_INSERTS = new Insets(5,20,5,20);

    public Legend(){
        legendRoot = new TilePane();
        legendRoot.setStyle(LEGEND_TILE_PANE_STYLE);
        legendRoot.setPadding(TILEPANE_INSERTS);
        this.getChildren().add(legendRoot);
        ShowLegend(null);
    }

    void ShowLegend(HashSet<Integer> functionsLegendHashSet){
        legendRoot.getChildren().clear();
        
        System.out.println("ShowLegend(): MAX_NUM_FUNC_IN_LEGEND = " + String.valueOf(MAX_NUM_FUNC_IN_LEGEND));
        int numFuncInLegend = 0;
        Integer[] arrayFuncIndex = {};
        if(functionsLegendHashSet != null){
            numFuncInLegend = functionsLegendHashSet.size();
            arrayFuncIndex = functionsLegendHashSet.toArray(new Integer[functionsLegendHashSet.size()]);
        }else{
            // else print all
            numFuncInLegend = MappingFuncNames.NUM_FUNC;
            arrayFuncIndex = MappingFuncNames.hashTableThreadName.values().toArray(new Integer[MappingFuncNames.NUM_FUNC]);
        }

        if(numFuncInLegend > MAX_NUM_FUNC_IN_LEGEND){
            System.out.println("ShowLegend(): Error, Stop !!!");
        }

        for(int i = 0; i < arrayFuncIndex.length; i++){
            final int index = arrayFuncIndex[i].intValue();
            Rectangle rect = new Rectangle(0, 0, SQUARE_WH, SQUARE_WH);
            rect.setFill(MappingFuncNames.GetColorForFuncIndex(index));//MappingFuncNames.GetColorForFunc(arrayFunc[i]));//MappingFuncNames.FUNC_RECT_COLOR[key.intValue()]);//i
            rect.setStroke(LEGEND_SQUARE_SCOLOR); 

            Label label = new Label(MappingFuncNames.GetFuncNameIndex(index));//arrayFunc[i]);
            label.setFont(LEGEND_FUNC_NAME_FONT);
            label.setTextFill(FUNC_NAME_LABEL_TEXT_COLOR);

            HBox legElem = new HBox();
            legElem.setSpacing(HBOX_SPASING);
            legElem.setPadding(HBOX_INSERTS);
            legElem.getChildren().addAll(rect, label);
            legendRoot.getChildren().add(legElem);
        }
    }
}
