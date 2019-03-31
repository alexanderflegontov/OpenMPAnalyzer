/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

import java.util.ArrayList;

/**
 * @brief the class keeps data for one row in the table
 * Filling all information about parallel section is delegated to another class(see above: StatTableData)
 * One parallel section - One instance of the TableRowData class
 * @author hp
 */
public class TableRowData {
    private final double startParallelTime;
    private final double endParallelTime;
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
        return GetPayloadDelta(i) / GetParallelDelta();
    }
}