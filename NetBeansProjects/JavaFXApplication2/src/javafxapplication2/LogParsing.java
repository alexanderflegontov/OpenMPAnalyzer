/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.security.InvalidParameterException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.TreeMap;
import javafx.scene.paint.Color;
import javafxapplication2.AnalyzeData.Mark;
import static javafxapplication2.SettingWindow.H_STAT_RECT;
import static javafxapplication2.SettingWindow.W;
import javafxapplication2.StatisticWindow.TimestampLine;

/**
 *
 * @author hp
 */
public class LogParsing {
    //private list<has>
            
    Map<Integer, List<Section>> sectionHierarchicalTable;
    
    LogParsing(){ ///int numOfFiles 
        sectionHierarchicalTable = new TreeMap<>();
    }
    
    Map GetSectionHierarchicalTable(){
        return sectionHierarchicalTable;
    }
            
    //List<Mark> getListMarks = (List<Mark>) TableStatThreads.get(ArrThreadId.get(thID));
    void Parsing(List<Mark> getListMarks, int thID){
        //LinkedList<Mark>

        int parallax = 0;
        int numBetweenSections = 0;
        //List<Mark> betweenSectionList = new ArrayList<>();
            
            for(ListIterator<Mark> elemIter = getListMarks.listIterator(); elemIter.hasNext(); ){
                boolean bFound = false;
                
                Mark elemEnd = elemIter.next();
                if(elemEnd.funcName.contains(MappingFuncNames.START)){
                    ++parallax;
                }else{
                    if(elemEnd.funcName.contains(MappingFuncNames.END)){
                        elemIter.remove(); // remove the end element
                        String[] ArrayStr = elemEnd.funcName.split("_");
                        
                        numBetweenSections = 0;
                        //betweenSectionList.clear();
                        while(elemIter.hasPrevious()){
                            //System.out.println("......    elemEndIter.nextIndex() = "+ elemEndIter.nextIndex());
                            Mark elemStart = elemIter.previous();                           
                            //System.out.println("......    elemStartIter.nextIndex() = "+ elemStartIter.nextIndex());

                            if(elemStart.funcName.contains(ArrayStr[0]) && elemStart.funcName.contains(MappingFuncNames.START)){
                                bFound = true;
                                --parallax;
                                List<Section> getLstParallax = (List<Section>) sectionHierarchicalTable.get(parallax);

                                if(getLstParallax == null){
                                    getLstParallax = new ArrayList<>();
                                    sectionHierarchicalTable.put(parallax, getLstParallax);
                                }
                                
                                final int funcNameInd = MappingFuncNames.GetIndex(ArrayStr[0]);
                                Section rect = CreateStartEndSection(elemStart, elemEnd, thID, funcNameInd, parallax);
                                
                                getLstParallax.add(rect);
                                elemIter.remove(); // remove the start element
                                
                                //if(numBetweenSections != 0){
                                //List<Mark> lstTmp = sectionHierarchicalTable.get(parallax +1);
                                for(; numBetweenSections > 0; --numBetweenSections){
                                    Mark elem1 =  elemIter.next();
                                    Section rectBarrier = CreateAloneSection(elem1, thID, parallax);
                                    if(rectBarrier != null){
                                        
                                        getLstParallax.add(rectBarrier);                                        
                                    }
                                    elemIter.remove(); // remove the current element(elem1)

                                }
                                    //getLstParallax.addAll(betweenSectionList);
                                //}
                                
                                //elemIter.remove(); // remove the end element
                                
                                break;
                            }
                            ++numBetweenSections;
                            //betweenSectionList.add(elemStart);
                        }
                        //System.out.println("END    elemEndIter.nextIndex() = "+ elemEndIter.nextIndex());
                        
                        if(!bFound){
                            System.out.println("Elem end "+"is not found ElemStart ! ");
                            //betweenSectionList.clear();
                        }
                        
                    }                 
                }

            }
        //return hashTableThreadStat;
    }



    public static Section CreateStartEndSection(Mark mrk1,Mark mrk2, int thID, final int funcNameId, final int parallax){

        if(TimestampLine.TimeLineEnd < mrk2.time){
            System.out.println("!!!!!!!!!! INVALID SECTION for thID = "+thID+" continue!!!!!!!!!!!!!!!!!!!!!");
            throw new InvalidParameterException("INVALID SECTION for thID = "+thID);
            //return null;//continue;
        }
        

        final double SpentTime = (mrk1.time - TimestampLine.TimeLineStart);
        final int widthSpentTimeBefore = (int)((SpentTime/TimestampLine.TimeLineWidth)*W);

        final double deltaSpentTime = mrk2.time-mrk1.time;
        final int widthDeltaSpentTime = (int)((deltaSpentTime/TimestampLine.TimeLineWidth)*W);

        //g.setColor(Color.RED);
        //g.drawRect((int)(LEFT_INDENT_X + widthSpentTime), Y, widthDeltaSpentTime, H_STAT_RECT);


        final int wid = Section.AntiSmallSections(widthDeltaSpentTime);
        SectionInfo secInfo = new SectionInfo(thID, funcNameId, SpentTime, mrk2.time-TimestampLine.TimeLineStart, parallax);
        Section rect = new Section(secInfo, widthSpentTimeBefore, 0, wid, H_STAT_RECT, true);

        final Color c = MappingFuncNames.GetColorForFuncIndex(funcNameId);
        rect.setFill(c);
        rect.setStroke(Color.BLACK);    
        //rootGroup.getChildren().add(rect);
        //rect.SetTime(thID, SpentTime, mrk2.time-TimestampLine.TimeLineStart);

        /*if(mrk2.funcName.contentEquals(MappingFuncNames.PAYLOADFUNC)){
            rect.SetNumIter(mrk2.num_iter);
        }*/

        //getListRectangles.add(rect);
//        threadStat.AddSection(rect);
//        functionsLegendHashSet.add(funcNameId);
        //rect.setOnMouseClicked(value);
        System.out.printf("!!!!!!!!!! Find  %s !!!s=%.6f e=%.6f \n",mrk1.time, mrk1.time, mrk2.time); //mrk1.funcName
        return rect;

    }

    public static Section CreateAloneSection(Mark mrk1, int thID, final int parallax){
        if(mrk1.funcName.contains(MappingFuncNames.BARRIER)){
            final double SpentTime = (mrk1.time - TimestampLine.TimeLineStart);
            final int widthSpentTimeBefore = (int)((SpentTime/TimestampLine.TimeLineWidth)*W);

            final int funcNameId = MappingFuncNames.GetIndex(MappingFuncNames.BARRIER);
            SectionInfo secInfo = new SectionInfo(thID, funcNameId, SpentTime, SpentTime, parallax);
            Section rect = new Section(secInfo, widthSpentTimeBefore, 0, Section.ONE_FUNC_WIDTH, H_STAT_RECT, false);
            
            final Color c = MappingFuncNames.GetColorForFuncIndex(funcNameId);
            rect.setFill(c);
            rect.setStroke(Color.BLACK);
            //rect.SetTime(thID, SpentTime, SpentTime);
            //rootGroup.getChildren().add(rect);

            /// g.setColor(Color.BLACK);
            /*Label label = new Label(MappingFuncNames.BARRIER);
            label.setFont(TEXT_FONT);
            //label.localToParent(new Point2D(X-15, Y-OFFSET_Y_TITLE_OBJ));
            label.setLayoutX(rect.getX()-15);
            label.setLayoutY(rect.getY()-OFFSET_Y_TITLE_OBJ);
            rootGroup.getChildren().add(label);*/

            //getListRectangles.add(rect);
//            threadStat.AddSection(rect);
//            functionsLegendHashSet.add(funcNameId);
            return rect;
        }else if(mrk1.funcName.contains(MappingFuncNames.GET_WTIME)){
            //
            System.out.println("thread " + String.valueOf(thID) +" : "+ MappingFuncNames.GET_WTIME);
            return null;
        }
        
        throw new InvalidParameterException("INVALID BARRIER SECTION for thID = "+thID);
        //return null;
    }
}


///
/*
                
                
                
                Mark mrk1 = getListMarks.get(j);

                if(mrk1.funcName.contains(MappingFuncNames.START)){
                    String[] ArrayStr = mrk1.funcName.split("_");

                    int k = getListMarks.size()-1;
                    for(; k > j; --k){
                        String strFindEndmrk = getListMarks.get(k).funcName;
                        if(strFindEndmrk.contains(ArrayStr[0]) && strFindEndmrk.contains(MappingFuncNames.END)){
                            break;
                        }
                    }                    
                    

                    //if(k != getListMarks.size()){
                    if(k != j){
                        Mark mrk2 = getListMarks.get(k);
                        if((mrk2.time-mrk1.time)<0){
                            System.out.println("!!!!!!!!!! INVALID SECTION for thID = "+thID+" break!!!!!!!!!!!!!!!!!!!!!");
                            //throw new InvalidParameterException("INVALID SECTION for thID = "+thID);   
                            break;
                        }
                        if(TimestampLine.TimeLineEnd < mrk2.time){
                            System.out.println("!!!!!!!!!! INVALID SECTION for thID = "+thID+" continue!!!!!!!!!!!!!!!!!!!!!");
                            continue;
                        }
                        System.out.printf("!!!!!!!!!! Find  %s !!!s=%.6f e=%.6f \n",ArrayStr[0], mrk1.time, mrk2.time); //mrk1.funcName

                        double SpentTime = (mrk1.time - TimestampLine.TimeLineStart);
                        int widthSpentTimeBefore = (int)((SpentTime/TimestampLine.TimeLineWidth)*W);

                        double deltaSpentTime = mrk2.time-mrk1.time;
                        int widthDeltaSpentTime = (int)((deltaSpentTime/TimestampLine.TimeLineWidth)*W);
                        
                        //g.setColor(Color.RED);
                        //g.drawRect((int)(LEFT_INDENT_X + widthSpentTime), Y, widthDeltaSpentTime, H_STAT_RECT);
                        int funcNameId = MappingFuncNames.GetIndex(ArrayStr[0]);
                        Color c = MappingFuncNames.GetColorForFuncIndex(funcNameId);
                        
                        int wid = Section.AntiSmallSections(widthDeltaSpentTime);
                        
                        Section rect = new Section(funcNameId, widthSpentTimeBefore, 0, wid, H_STAT_RECT, true);
                        rect.setFill(c);
                        rect.setStroke(Color.BLACK);    
                        //rootGroup.getChildren().add(rect);
                        rect.SetTime(thID, SpentTime, mrk2.time-TimestampLine.TimeLineStart);


                        //getListRectangles.add(rect);
                        threadStat.AddSection(rect);
                        functionsLegendHashSet.add(funcNameId);
                        //rect.setOnMouseClicked(value);
                    }
                }else if(mrk1.funcName.contains(MappingFuncNames.BARRIER)){
                    double SpentTime = (mrk1.time - TimestampLine.TimeLineStart);
                    int widthSpentTimeBefore = (int)((SpentTime/TimestampLine.TimeLineWidth)*W);

                    int funcNameId = MappingFuncNames.GetIndex(MappingFuncNames.BARRIER);
                    Color c = MappingFuncNames.GetColorForFuncIndex(funcNameId);

                    Section rect = new Section(funcNameId, widthSpentTimeBefore, 0, Section.ONE_FUNC_WIDTH, H_STAT_RECT, false);
                    rect.setFill(c);
                    rect.setStroke(Color.BLACK);
                    rect.SetTime(thID, SpentTime, SpentTime);
                    //rootGroup.getChildren().add(rect);

                    /// g.setColor(Color.BLACK);


                    //getListRectangles.add(rect);
                    threadStat.AddSection(rect);
                    functionsLegendHashSet.add(funcNameId);

                }else if(mrk1.funcName.contains(MappingFuncNames.GET_WTIME)){
                    //
                    System.out.println("thread " + String.valueOf(thID) +" : "+ MappingFuncNames.GET_WTIME);
                }

*/
///