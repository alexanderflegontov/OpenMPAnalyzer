/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

/**
 * @brief The class is a data-model for section
 * @author HP
 */
public class SectionData {
       
    public final int threadId;
    public final int funcNameId;
    public final double startTime;
    public final double endTime;       
    public final int numNestedLevel;

    private int sectionIndex;

    SectionData(final int tid, final int funcNameId,
            final double startTime, final double endTime, final int nestedLevel){

        this.threadId = tid;
        this.funcNameId = funcNameId;
        this.startTime = startTime;
        this.endTime = endTime;
        this.numNestedLevel = nestedLevel;

        this.sectionIndex = -1;
    }

    public String GetFuncName(){
        return MappingFuncNames.GetFuncNameIndex(funcNameId);
    }

    public void SetSectionIndex(final int sectionInd){
        sectionIndex = sectionInd;
    }

    public int GetSectionIndex(){
        return sectionIndex;
    }
}