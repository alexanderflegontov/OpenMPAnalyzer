/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

import java.util.ArrayList;
import java.util.ListIterator;

/**
 * @brief the class keeps data for all parallel sections in an application and provides data to a table for presentation.
 * Filling all the information about all parallel sections is delegated to another class(ThreadStat)
 * StatTableData has data all of TableRowes
 * @author hp
 */
public class StatTableData {

    private ArrayList<TableRowData> ParallelSections;
    private static int LastIter = 0;

    public static void ClearLastIterInEachThreadStat(){
        LastIter = 0;
    }

    StatTableData(){
        ParallelSections = new ArrayList<TableRowData>();
    }

    public ArrayList<TableRowData> GetRefParallelSections(){
        return ParallelSections;
    }

    public void ClearPrevData(){
        ParallelSections.clear();
    }


/*    
    private boolean IsNestedParallelSection(ISection sec){
        if(ParallelSections.isEmpty()){
            return true;    
        }
        else{
            TableRowData lastSec = (TableRowData)ParallelSections.get(ParallelSections.size()-1);
            return (lastSec.GetStartParallel() < sec.GetStartTime()) && (sec.GetEndTime() < lastSec.GetEndParallel());       
        }
    }
*/
    public boolean AddParallel(ISection sec){
        double startParTime = sec.GetStartTime();
        double endParTime = sec.GetEndTime();
        TableRowData stc = new TableRowData(startParTime, endParTime);
        ParallelSections.add(stc);
        sec.SetSectionIndex(ParallelSections.size()-1);
        return true;
    }
   
    public boolean AddPayload(int tid, ISection sec){
        double startPayTime = sec.GetStartTime();
        double endPayTime = sec.GetEndTime();
        
        ListIterator Iter = ParallelSections.listIterator(LastIter);
        boolean isFound = false;
        TableRowData ParSec = null;
        /*if(tid == 0){
            // begin from the end
            while(Iter.hasPrevious()){
                ParSec = (TableRowData) Iter.previous();
                if(ParSec.startParallelTime <= startPayTime && endPayTime <= ParSec.GetEndParallel()){
                    isFound = true;
                    break;
                }
            }
        }else{*/
            // begin from the start
            while(Iter.hasNext()){
                ParSec = (TableRowData) Iter.next();
                if(ParSec.GetStartParallel() <= startPayTime && endPayTime <= ParSec.GetEndParallel()){
                    isFound = true;
                    
                    LastIter = Iter.previousIndex();
                    sec.SetSectionIndex(LastIter); 
                    ParSec.AddPayload(startPayTime, endPayTime);
                    break;
                }
            }
        return true;
    }

    /*
    public boolean checkPayloadValid(double endParTime, double startPayTime){
        return (endParTime < startPayTime);
    }
    */
}
