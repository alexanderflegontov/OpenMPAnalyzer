////package zoomableCanvas;

import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.scene.Group;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.transform.Affine;

public abstract class ZoomableCanvas extends Canvas {
    public static final double SCROLL_MAX = 100.0;
    public static final double SCROLL_MIN = 1.0;
    public static final double SCROLL_START = 1.0;

        
    private static final double SCROLL_FACTOR = 400;
    
    private SimpleDoubleProperty zoomX = new SimpleDoubleProperty(1.0);
    private static SimpleDoubleProperty posX = new SimpleDoubleProperty(0.5); // 50% in scrollH
    private SimpleDoubleProperty posY = new SimpleDoubleProperty(0.0);
    
    private static final double POS_START_X = 0.0;
    private static final double POS_START_Y = 0.0;
    
    private double PrevY = posY.get();
    private VBox numThreadGroup;
    private TimeNumber timeNumGroup;
    private double TimeLineWidth;
    private static boolean flagFromZoom;
    private static double mouseCursorX; 
    private static double mouseCursorY; 
    
    public ZoomableCanvas() {
        this(0, 0);
    }

    public ZoomableCanvas(double width, double height) {
        super(width, height);
        this.setOnScroll(zoomHandler);
        //this.zoomProperty().addListener(o -> redraw());
        this.PosXProperty().addListener(o -> ChangePosX());
        this.PosYProperty().addListener(o -> ChangePosY());
        flagFromZoom = false;
    }

    static protected EventHandler<ScrollEvent> zoomHandler = new EventHandler<ScrollEvent>() {
        @Override
        public void handle(ScrollEvent event) {
            mouseCursorX = event.getX();
            mouseCursorY = event.getY();
                    
            ZoomableCanvas zcanvas = (ZoomableCanvas) event.getSource();
            GraphicsContext gc = zcanvas.getGraphicsContext2D();
            Affine affine = gc.getTransform();
            System.out.println("affine = "+affine);
            System.out.println("getDeltaX()"+event.getDeltaX());
            System.out.println("getDeltaY()"+event.getDeltaY());
            System.out.println("affine.getMxx()"+affine.getMxx());
            double zoom = affine.getMxx() + event.getDeltaY() / SCROLL_FACTOR;
            if (zoom <= SCROLL_MIN) {
                zoom = SCROLL_MIN;
            }else if (zoom >= SCROLL_MAX) {
                zoom = SCROLL_MAX;
            }
            zcanvas.setZoom(zoom);

            double w = zcanvas.getWidth();
            double cursorPosPercentX = mouseCursorX/w;
            posX.set(cursorPosPercentX);
            flagFromZoom = true;
            zcanvas.redraw();
        }
    };
    
    void ChangePosX(){
        double posx = posX.get();
        double w = this.getWidth();
        
        if (posx <= 0) {
            posX.set(0);
        }else if (posx >= w) {
            posX.set(w);
        }
        this.redraw();
    }
    
    void ChangePosY(){
        double posx = posY.get();
        double h = this.getHeight()/2;

        if (posx <= 0) {
            posY.set(0);
        }else if (posx >= h) {
            posY.set(h);
        }
        this.redraw();
    }
    
    private void ReplaceNumbersThreadGroup(){
        double deltaPos = getPositionY()-PrevY;
        double prevValue = numThreadGroup.getPadding().getTop();
        numThreadGroup.setPadding(new Insets(prevValue+deltaPos,0,0,0));
    }
    
    private void UpdateTimeLineGroup(){
        double scaleX = getZoom();
        double valueX  =  getPositionX();
        double oldDeltaAllTime = TimeLineWidth;
        //double newDeltaAllTime = oldDeltaAllTime*scale;
        double windowDeltaViewTime = oldDeltaAllTime/scaleX;
        
        double start = (oldDeltaAllTime-windowDeltaViewTime)*valueX;
        double end = start+windowDeltaViewTime;
        timeNumGroup.ShowUpdate(start, end);
    }
    
    public void setNumThreadGroup(VBox gr) {
        numThreadGroup = gr;
    }
    
    public void setTimeLineGroup(TimeNumber tn) {
        timeNumGroup = tn;
    }
    
    public void SetEND_TIME(double time){
        TimeLineWidth = time;
    }
        
    private void setZoom(double value) {
        if (value != getZoom()) {
            this.zoomX.set(value);
            redraw();
        }
    }

    public double getZoom() {
        return zoomX.get();
    }

    public DoubleProperty zoomProperty() {
        return zoomX;
    }
        
    public DoubleProperty PosXProperty() {
        return posX;
    }
    
    public DoubleProperty PosYProperty() {
        return posY;
    }
    
    public double getPositionX() {
        return posX.get();
    }
    
    public double getPositionY() {
        return posY.get();
    }
    
    public void clean() {
        GraphicsContext gc = this.getGraphicsContext2D();
        if (gc == null) return;
        Canvas canvas = gc.getCanvas();
        Affine affine = gc.getTransform();
        gc.setTransform(1, 0, 0, 1, 0, 0);
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        gc.setTransform(affine);
    }

    public void redraw() {
        GraphicsContext gc = this.getGraphicsContext2D();
        if (gc == null) return;

        // Чистим:
        Canvas canvas = gc.getCanvas();
        double w = canvas.getWidth();
        double h = canvas.getHeight();
        gc.setTransform(1, 0, 0, 1, 0, 0);
        gc.clearRect(0, 0, w, h);

        double z = getZoom();
        double xpos = getPositionX();// offset origin in scrollbarH
        double ypos = getPositionY();
        
        // Масштабирование в левый верхний угол:
        //gc.setTransform(z, 0, 0, z, 0, 0);

        // Масштабирование в центр canvas:
        //gc.setTransform(z, 0, 0, z, (w - w * z) / 2.0, (h - h * z) / 2.0);

        // Масштабирование по X:        
        gc.setTransform(z, 0, 0, 1, (w - w * z)*xpos, ypos);
        //gc.setTransform(z, 0, 0, 1, (w - w * z) / 2.0+xpos, ypos);
        this.ReplaceNumbersThreadGroup();
        this.UpdateTimeLineGroup();
        PrevY = ypos;
        mouseCursorX = 0;
        flagFromZoom = false;
        // Рисуем:
        paint(gc);
    }

    public abstract void paint(GraphicsContext gc);

    
    public static final int Hrect = 20;
    public static final int HrectTH = 30;
    public final int Hbeetween = 10;
    
    
    public void ShowStat(GraphicsContext gc){
        gc.setFill(Color.BLUE);
        int withSpendBefore = 0;
        int withSpendTime = this.widthProperty().intValue();
        
        
        for(int i=0; i<10; i++){
            gc.fillRect(withSpendBefore, HrectTH*i, withSpendTime, Hrect);
        }

        withSpendBefore = 100;
        withSpendTime = 250;
        gc.setFill(Color.GREEN);
        for(int i=0; i<10; i+=2){
            gc.fillRect(withSpendBefore, HrectTH*i, withSpendTime, Hrect);
        }
        
        withSpendBefore = 100+250+10;
        withSpendTime = 30;
        gc.setFill(Color.ORANGE);
        gc.fillRect(withSpendBefore, HrectTH*4, withSpendTime, Hrect);
    }
}