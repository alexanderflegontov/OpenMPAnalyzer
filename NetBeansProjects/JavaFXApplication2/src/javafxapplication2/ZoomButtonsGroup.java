/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import javafx.event.EventHandler;
import javafx.scene.Group;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Shape;


/**
 *
 * @author HP
 */
public class ZoomButtonsGroup extends Group{
        public static final int ZOOM_BUTTON_ORIGIN_X = 110; //OriginX
        public static final int ZOOM_BUTTON_ORIGIN_Y = 80;
        private static final int ZOOM_BUTTON_BETWEEN = 25;
        
        public static final int ZOOM_BUTTON_WIDTH = 30;
        private static final Color ZOOM_BUTTON_FCOLOR_CLICKED = Color.rgb(0, 190, 240);
        private static final Color ZOOM_BUTTON_FCOLOR = Color.rgb(240, 190, 0);
        
        private static final Color ZOOM_BUTTON_SCOLOR = Color.BLACK;

        private static final int ZOOM_BUTTON_ARC = 3;
        private static final int ZOOM_BUTTON_STROKE_W = 3;
        
        private static final Color ZOOM_BUTTON_LINE_SCOLOR = Color.BLACK;
        private static final int ZOOM_BUTTON_LINE_STROKE_W = 3;

        private Rectangle rectBackgroundWindow;
        
        
        
        private Group scalableGroup;
        private static double number = 1;
        
        private Group zoomButtonGroup;
        private ZoomButton zoomButtonPlus;    
        private ZoomButton zoomButtonMinus;    

        ZoomButtonsGroup(){
            super();
            zoomButtonGroup = new Group();
            this.getChildren().add(zoomButtonGroup);

            zoomButtonPlus = new ZoomButton(ZOOM_BUTTON_WIDTH, true);
            zoomButtonPlus.setLayoutY(0);
            zoomButtonGroup.getChildren().add(zoomButtonPlus);
            
            zoomButtonMinus = new ZoomButton(ZOOM_BUTTON_WIDTH, false);
            zoomButtonMinus.setLayoutY(ZOOM_BUTTON_WIDTH + ZOOM_BUTTON_BETWEEN);
            zoomButtonGroup.getChildren().add(zoomButtonMinus);
            
            EventHandler<MouseEvent> mousePressedHandler = new EventHandler<MouseEvent>() {
                                @Override
                                public void handle(MouseEvent event) {
                                    System.out.println("ZoomButtons[event]: mousePressedHandle");
                                    
                                    //ZoomButton zoomButton = (ZoomButton)event.getTarget();
                                    //zoomButton.GetRectButton().setFill(ZOOM_BUTTON_FCOLOR_CLICKED);
                                    //Rectangle rect = (Rectangle)event.getTarget();
                                    
                                    //rect.setFill(ZOOM_BUTTON_FCOLOR_CLICKED);
                                    ZoomButton zoomButton;//(ZoomButton)rect.getParent();
                                    zoomButton = (ZoomButton)(((Shape)event.getTarget()).getParent());
                                    zoomButton.GetRectButton().setFill(ZOOM_BUTTON_FCOLOR_CLICKED);
                                    
                                    if(zoomButton.isPlusButton == true){
                                        number+=0.1;
                                    }else{
                                        if(number > 1){
                                            number-=0.1;
                                        }else{
                                            number = 1;
                                        }
                                    }
                                    ScaleHandler(number); 
                                }
                            };
            
            EventHandler<MouseEvent> mouseReleasedHandler = new EventHandler<MouseEvent>() {
                                @Override
                                public void handle(MouseEvent event) {
                                    System.out.println("ZoomButtons[event]: mouseReleasedHandle");
                                    
                                    //ZoomButton zoomButton = (ZoomButton)event.getTarget();
                                    //zoomButton.GetRectButton().setFill(ZOOM_BUTTON_FCOLOR);
                                    ZoomButton zoomButton;
                                    zoomButton = (ZoomButton)(((Shape)event.getTarget()).getParent());
                                    zoomButton.GetRectButton().setFill(ZOOM_BUTTON_FCOLOR);
                                    //rect.setFill(ZOOM_BUTTON_FCOLOR_CLICKED);
                                }
                            };
            /*
            EventHandler<EnterEvent> mouseFilterHandler = new EventHandler<EnterEvent>() {
                                @Override
                                public void handle(EnterEvent event) {
                                    System.out.println("ZoomButtons[event]: mouseFilterHandler");
                                    if(event.getTarget().getClass() == Line.class)
                                    {
                                        event.consume();
                                    }
                                }
                            };
            zoomButtonPlus.addEventFilter(EventType.ENTERED, mouseFilterHandler);
            //zoomButtonPlus.addEventFilter(EventType.ROOT, mouseFilterHandler);
            */
            
            zoomButtonPlus.setOnMousePressed(mousePressedHandler);
            zoomButtonPlus.setOnMouseReleased(mouseReleasedHandler);
            
            zoomButtonMinus.setOnMousePressed(mousePressedHandler);
            zoomButtonMinus.setOnMouseReleased(mouseReleasedHandler);
            
        }
        
        private void ScaleHandler(double scaleValue){
            if(scalableGroup != null){
                //long val = fibonacci(scaleValue);
                scalableGroup.setScaleX(scaleValue);
                scalableGroup.setScaleY(scaleValue);
            }
            if(rectBackgroundWindow != null){
                rectBackgroundWindow.setScaleX(scaleValue);
                rectBackgroundWindow.setScaleY(scaleValue);
            }
        }

        public void SetScalableGroup(Group group){
            scalableGroup = group;
        }
        
        public void SetScalableGroup(Group group, Rectangle rectBackground){
            scalableGroup = group;
            rectBackgroundWindow = rectBackground;
        }

        
        public class ZoomButton extends Group{
            public final boolean isPlusButton;
            private Rectangle rectButton;

            public Rectangle GetRectButton(){
                return rectButton;
            } 
            ZoomButton(){
                super();
                isPlusButton = true;
            }
            ZoomButton(int button_width, boolean isPlus){
                super();
                isPlusButton = isPlus;
                
                rectButton = new Rectangle( 0, 0, button_width, button_width);
                rectButton.setFill(ZOOM_BUTTON_FCOLOR);
                rectButton.setStroke(ZOOM_BUTTON_SCOLOR);
                rectButton.setStrokeWidth(ZOOM_BUTTON_STROKE_W);
                rectButton.setArcWidth(ZOOM_BUTTON_ARC);
                rectButton.setArcHeight(ZOOM_BUTTON_ARC);
                this.getChildren().add(rectButton);

                if(isPlus == true){
                    Line linePlusV = new Line(rectButton.getX() + rectButton.getWidth()/2, rectButton.getY() + ZOOM_BUTTON_STROKE_W*2, 
                            rectButton.getX() + rectButton.getWidth()/2, rectButton.getY() + rectButton.getHeight() - ZOOM_BUTTON_STROKE_W*2);
                    linePlusV.setStroke(ZOOM_BUTTON_LINE_SCOLOR);
                    linePlusV.setStrokeWidth(ZOOM_BUTTON_LINE_STROKE_W);
                    linePlusV.setFill(Color.YELLOW);
                    this.getChildren().add(linePlusV);
                }

                Line linePlusH = new Line(rectButton.getX() + ZOOM_BUTTON_STROKE_W*2, rectButton.getY() + rectButton.getHeight()/2, 
                        rectButton.getX() + rectButton.getWidth() - ZOOM_BUTTON_STROKE_W*2, rectButton.getY() + rectButton.getY() + rectButton.getHeight()/2);
                linePlusH.setStroke(ZOOM_BUTTON_LINE_SCOLOR);
                linePlusH.setStrokeWidth(ZOOM_BUTTON_LINE_STROKE_W);
                linePlusH.setFill(Color.YELLOW);
                this.getChildren().add(linePlusH);
            }
        }
}
