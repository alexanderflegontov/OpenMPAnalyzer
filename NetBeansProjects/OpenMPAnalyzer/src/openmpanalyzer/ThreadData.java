/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

/**
 * @brief The class keeps statistics of its thread.
 * Each property of this class is the value of the duration spent in the corresponding section.
 * @author hp
 */
public class ThreadData {

    private double threadTimeOfWork   = 0.0;
    private double threadTimeInParallel = 0.0;
    private double threadTimeInSequence = 0.0;
    private double threadTimeOfPayload  = 0.0;
    private double threadSpeedUp      = 0.0;
    private double threadTimeInAtomic   = 0.0;

    ThreadData(){
    }

    double GetTimeOfWork(){
        return threadTimeOfWork;
    }

    double GetTimeInParallel(){
        return threadTimeInParallel;
    }

    double GetTimeInSequence(){
        return threadTimeInSequence;
    }

    double GetSpeedUp(){
        return threadSpeedUp;
    }

    double GetTimeOfPayload(){
        return threadTimeOfPayload;
    }

    double GetTimeInAtomic(){
        return threadTimeInAtomic;
    }

    void SetTimeOfWork(double new_val){
        threadTimeOfWork = new_val;
    }

    void SetTimeInParallel(double new_val){
        threadTimeInParallel = new_val;
    }

    void SetTimeInSequence(double new_val){
        threadTimeInSequence = new_val;
    }

    void SetTimeOfPayload(double new_val){
        threadTimeOfPayload = new_val;
    }

    void SetTimeInAtomic(double new_val){
        threadTimeInAtomic = new_val;
    }

    void CalculateTimeInSequence(){
        threadTimeInSequence = threadTimeOfWork - threadTimeInParallel;
    }

    void CalculateSpeedUp(){
        if(threadTimeInSequence != 0.0){
            threadSpeedUp = threadTimeInParallel/threadTimeInSequence;
        }
    }
}
