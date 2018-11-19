/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.util.ArrayList;
import java.util.ListIterator;

/**
 *
 * @author HP
 */
public class StatTableData {

    private ArrayList<TableRowData> ParallelSections;
    private static int LastIter = 0;
    
    StatTableData(){
        ParallelSections = new ArrayList<TableRowData>();
    }
    
    public ArrayList<TableRowData> GetRefParallelSections(){
        return ParallelSections;
    }
    
    public void ClearPrevData(){
        ParallelSections.clear();
    }
    
    public static void ClearLastIterInEachThreadStat(){
        LastIter = 0;
    }
/*    
    private boolean IsNestedParallelSection(Section sec){
        if(ParallelSections.isEmpty()){
            return true;    
        }
        else{
            TableRowData lastSec = (TableRowData)ParallelSections.get(ParallelSections.size()-1);
            return (lastSec.GetStartParallel() < sec.GetStartTime()) && (sec.GetEndTime() < lastSec.GetEndParallel());       
        }
    }
*/
    
    public boolean AddParallel(Section sec){

        double startParTime = sec.GetStartTime();
        double endParTime = sec.GetEndTime();
        TableRowData stc = new TableRowData(startParTime, endParTime);
        ParallelSections.add(stc);
        sec.SetSectionIndex(ParallelSections.size()-1);
        return true;
    }
            
    public boolean AddPayload(int tid, Section sec){
        double startPayTime = sec.GetStartTime();
        double endPayTime = sec.GetEndTime();
        
        ListIterator Iter = ParallelSections.listIterator(LastIter);
        boolean isFound = false;
        TableRowData ParSec = null;
        /*if(tid == 0){
            // begin at the end
            while(Iter.hasPrevious()){
                ParSec = (TableRowData) Iter.previous();
                if(ParSec.startParallelTime <= startPayTime && endPayTime <= ParSec.GetEndParallel()){
                    isFound = true;
                    break;
                }
            }
        }else{*/
            // begin at the start
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
    
    // One parallel section - One instance of the TableRowData class 
    public class TableRowData {
        private double startParallelTime;
        private double endParallelTime;
        private ArrayList startPayloadTime;
        private ArrayList endPayloadTime;
        
        public boolean IsValid(){
            System.out.println("startPayloadTime.size() = " + startPayloadTime.size());
            System.out.println("endPayloadTime.size() = " + endPayloadTime.size());
            return (endPayloadTime.size() == startPayloadTime.size());
        }
        
        public int GetNumThreads(){
            return endPayloadTime.size();
        }
        
        TableRowData(final double startParTime, final double endParTime){
            this.startParallelTime = startParTime;
            this.endParallelTime = endParTime;
            this.startPayloadTime = new ArrayList();
            this.endPayloadTime = new ArrayList();
        }
        
        public void AddPayload(final double startPayTime, final double endPayTime){
            this.startPayloadTime.add(startPayTime);
            this.endPayloadTime.add(endPayTime);
        }
        
        public double GetStartParallel(){
            return this.startParallelTime;
        }

        public double GetEndParallel(){
            return this.endParallelTime;
        }

        public double GetStartPayload(final int i){
            return (double)(this.startPayloadTime.get(i));
        }

        public double GetEndPayload(final int i){
            return (double)(this.endPayloadTime.get(i));
        }

        public double GetParallelDelta(){
            return endParallelTime - startParallelTime;
        }

        public double GetPayloadDelta(final int i){
            return (double)(this.endPayloadTime.get(i)) - (double)(this.startPayloadTime.get(i));
        }
        
        public double GetRate(final int i){
            return GetPayloadDelta(i)/GetParallelDelta();
        }
        
    }
}
