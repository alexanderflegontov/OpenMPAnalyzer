/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author HP
 */


// The class for processing .out text file
public class AnalyzeData{
    List<PinToolMark> listTHMarks;

    public static final String STR_DELIMITER_CONF = "=";
    public static final String STR_NUM_THREADS = "num_threads";
    public static final String STR_NAME_OUT = "name_out";
    
    public static final String DEFAULT_PINTOOL_OUT = "myPinTool.out"; 
    public static final String DEFAULT_PINTOOL_OUT_PATH = "";//"./";//"/home/sasha/Downloads/pin-3.5-97503-gac534ca30-gcc-linux/source/tools/ManualExamples/";
//    public static final String DEFAULT_PINTOOL_OUT_FILE = DEFAULT_PINTOOL_OUT_PATH+DEFAULT_PINTOOL_OUT;
    public static final String DEFAULT_PINTOOL_OUT_CONF = "config.out";

    public AnalyzeData(){
        listTHMarks = new ArrayList<>();
    }

    public static boolean RemovePreviousResultFiles(final String pathToPrevOutFile) {
        //final String pathToPrevOutFile = "./";

        boolean wasSomeoneRemoved = false;
        try{
            File fileOutForAnalysis = new File(pathToPrevOutFile+DEFAULT_PINTOOL_OUT);
            boolean resultRemoveOutFile = Files.deleteIfExists(fileOutForAnalysis.toPath());

            ArrayList<String> namesOutConfFiles = ReadConfigFile(pathToPrevOutFile+DEFAULT_PINTOOL_OUT_CONF);
            if(namesOutConfFiles != null){
                // config file is found and is valid
                for(int i = 0; i <namesOutConfFiles.size(); i++){
                    fileOutForAnalysis = new File(namesOutConfFiles.get(i));
                    Files.deleteIfExists(fileOutForAnalysis.toPath());
                    System.out.println("[deletePreviousResultFiles]: remove " + fileOutForAnalysis.toPath());
                }
            }

            fileOutForAnalysis = new File(pathToPrevOutFile+DEFAULT_PINTOOL_OUT_CONF);
            boolean resultRemoveConfFile = Files.deleteIfExists(fileOutForAnalysis.toPath());
            wasSomeoneRemoved = resultRemoveOutFile || resultRemoveConfFile;
        }catch (IOException ex) {
            Logger.getLogger(AnalyzeData.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        return wasSomeoneRemoved;
    }
    
    public static boolean isConfigFile(String strPinToolOutfile) {
        //File fileOutForAnalysis = new File(strPinToolOutfile);
        //if(fileOutForAnalysis != null && fileOutForAnalysis.isFile() && fileOutForAnalysis.canRead()){
        //if(STR_PINTOOL_OUT_CONF == fileOutForAnalysis.getName()){
        //    return true;
        //}
        
        return DEFAULT_PINTOOL_OUT_CONF.contentEquals(strPinToolOutfile);
    }
    
    public Map Start(String strPinToolOutfile){
        System.out.println("============= AnalyzeData::Start() - try confing file proccessing ========");
        Map TableStatisticaThreads = StartConfingFileProcessing(strPinToolOutfile);
        if(TableStatisticaThreads == null){
            System.out.println("============= AnalyzeData::Start() - config proccessing fails, try one file proccessing ========");
            TableStatisticaThreads = StartOneFileProccessing(strPinToolOutfile);
        }
        return TableStatisticaThreads;
    } 
    
    private boolean Filter(PinToolMark mark){
        switch(mark.funcName.split("_")[0])
        {
            case MappingFuncNames.GET_WTIME:
                return true;
            default:
                return false;
        }
    }
    
    public Map StartOneFileProccessing(String strPinToolOutfile) {
        Map hashTableThreadMarks = null;

        File fileOutForAnalysis = new File(strPinToolOutfile);
        if(fileOutForAnalysis != null && fileOutForAnalysis.isFile() && fileOutForAnalysis.canRead()){

            BufferedReader reader = null;

            try {
                System.out.println("START");

                hashTableThreadMarks = new TreeMap<Integer, List<Mark>>(); 
                reader = new BufferedReader(new FileReader(fileOutForAnalysis));
                
                listTHMarks.clear(); // clear previous data of analysis
                String text;
                while ((text = reader.readLine()) != null) {

                    PinToolMark mark = new PinToolMark();
                    boolean isDecodeOk = mark.ProcessingLineText(text);

                    if(isDecodeOk == true && mark.threadId != null){// && mark.time >= 0){
                        if(Filter(mark)){
                            continue;
                        }
                        
                        Mark mrk = new Mark(mark);
                        List<Mark> getListMarks = (List<Mark>) hashTableThreadMarks.get(mark.threadId);
                        if(getListMarks != null){
                            getListMarks.add(mrk);
                        }else{
                            getListMarks = new ArrayList<>();
                            getListMarks.add(mrk);
                            hashTableThreadMarks.put(mark.threadId, getListMarks);
                        }
                        listTHMarks.add(mark);
                    }
                    else{
                        System.out.println(" ProcessingLineText fails !!!"); 
                    }
                }
                System.out.println("END");

                //TreeMap<Integer, List<Mark>> sorted_map = new TreeMap<Integer, List<Mark>>();
                //sorted_map.putAll(hashTableThreadMarks);
                //System.out.println("unsorted map: " + hashTableThreadMarks);

                ArrayList<Integer> ArrThreadId = new ArrayList<>(hashTableThreadMarks.keySet());
                for(int i = 0; i < ArrThreadId.size(); ++i)// Map.Entry e : sorted_map.entrySet()
                {
                    System.out.println("Thread("+ArrThreadId.get(i)+") is a " + String.valueOf(i));
                }
                System.out.println("AnalyzeData::ThreadCount = " + String.valueOf(ArrThreadId.size()));
  
            } catch (FileNotFoundException e) {
                System.out.println("File .out is not found for analysis!");
                e.printStackTrace();

            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    if (reader != null) {
                        reader.close();
                    }
                } catch (IOException e) {
                }
            }
            System.out.println("This is the end of Start function of AnalyzeData");
        }
        return hashTableThreadMarks;
    }
     
 
    public static Map StartConfingFileProcessing(String strPinToolOutfile){
        ArrayList<String> namesOutConfFiles = ReadConfigFile(strPinToolOutfile);
        if(namesOutConfFiles != null){
            // config file is found and is valid
            System.out.println("config file is found and is valid");
            Map hashTableThreadMarks = new TreeMap<Integer, List<Mark>>();
            AnalyzeData analyze_data = new AnalyzeData();
            for(int i = 0; i <namesOutConfFiles.size(); i++){
                
                System.out.println("outFile = " + (String)namesOutConfFiles.get(i));
                Map hashTable = analyze_data.StartOneFileProccessing(namesOutConfFiles.get(i));
                if(hashTable!= null){
                    System.out.println("putAll");
                    hashTableThreadMarks.putAll(hashTable);
                }else{
                    // fail
                    return null;
                }
            }
            return hashTableThreadMarks;
        }
        return null;
    }
        
    private static ArrayList<String> ReadConfigFile(String strPinToolOutfile){
        System.out.println("IN: "+strPinToolOutfile);

        ArrayList<String> namesOutConfFiles = null;
        File fileOutForAnalysis = new File(strPinToolOutfile);
        if(fileOutForAnalysis != null && fileOutForAnalysis.isFile() && fileOutForAnalysis.canRead()){

            BufferedReader reader = null;
            try{

                final String absolutePath = fileOutForAnalysis.getAbsolutePath();
                final String filePath = absolutePath.
                substring(0,absolutePath.lastIndexOf(File.separator)) + File.separator;
                System.out.println("absolutePath =  " + absolutePath);
                System.out.println("filePath =  " + filePath);

                namesOutConfFiles = new ArrayList<>(); 
                reader = new BufferedReader(new FileReader(fileOutForAnalysis));
                String lineText;
                int num_threads = -1;
                while ((lineText = reader.readLine()) != null) {
                    /*
                    num_threads=6
                    name_out="0.out"
                    name_out="1.out"
                    */
                    StringTokenizer st = new StringTokenizer(lineText.trim(), STR_DELIMITER_CONF);           
                    while(st.hasMoreTokens()) {
                        String key = st.nextToken();
                        String val = st.nextToken();
                        switch(key){
                            case STR_NUM_THREADS:
                                num_threads = Integer.parseInt(val);//Integer.parseUnsignedInt(val);
                                break;
                            case STR_NAME_OUT:
                                // TODO: validation test FOR STRING
                                namesOutConfFiles.add(filePath+val);
                                break;
                            default:
                                System.out.println("unknown format =  " + key + " : " + val);
                                return null;
                                //break;
                        }
                        System.out.println(key + " -> " + val);
                    }
                    //System.out.println(this.toString());
                }
                // validation test, like CRC :)
                if(num_threads != namesOutConfFiles.size()){
                    System.out.println("File .out is not invalid!");
                    return null;
                }
                System.out.println("END");  
            } catch (FileNotFoundException e) {
                System.out.println("File .out is not found for analysis!");
                e.printStackTrace();
                return null;
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            } finally {
                try {
                    if (reader != null) {
                        reader.close();
                    }
                } catch (IOException e) {
                    return null;
                }
            }
           System.out.println("This is the end of FileConfigProcessing function of AnalyzeData");
        }

        return namesOutConfFiles;
    } 
    
    // The class for proccessing one text line in PinToolMark 
    public class PinToolMark extends Object{

    // Abbreviations and auxiliary things: 
        public static final String STR_THREAD    = "TH";
        public static final String STR_OPERATION = "OP";
        public static final String STR_TIME      = "TM";
        public static final String STR_ITER      = "IT";
        public static final String STR_DELIMITER = "= ";
    // Structure of entry:
        public Integer threadId;
        public String funcName;
        public double time;
        public Integer num_iter;

        PinToolMark(){
            threadId = 0;
            funcName = "";
            time = 0.0;
            num_iter = 0;
        }

        @Override
        public String toString(){
                String decodedTextLine = STR_THREAD + STR_DELIMITER + threadId + " ";
                decodedTextLine += STR_OPERATION + STR_DELIMITER + funcName + " ";
                decodedTextLine += STR_TIME + STR_DELIMITER + time;
                if(num_iter != 0){
                    decodedTextLine += STR_ITER + STR_DELIMITER + num_iter + " ";
                }
                return decodedTextLine;
        }

        public boolean ProcessingLineText(String lineText){
            System.out.println("IN: "+lineText);

            //PinToolMark mark = new PinToolMark();
            boolean isOk = true;
            StringTokenizer st = new StringTokenizer(lineText.trim(), STR_DELIMITER);
            while(st.hasMoreTokens()) {
                String key = st.nextToken();
                String val = st.nextToken();
                switch(key){
                    case STR_THREAD:
                        this.threadId = Integer.parseInt(val);//Integer.parseUnsignedInt(val);
                        break;
                    case STR_OPERATION:
                        this.funcName = val;
                        break;
                    case STR_TIME:
                        this.time = Double.parseDouble(val);
                        break;
                    case STR_ITER:
                        this.num_iter = Integer.parseInt(val);
                        break;
                    default:
                        System.out.println("unknown format =  " + key + " : " + val);
                        isOk = false;
                        //this = null;
                        break;
                }
                System.out.println(key + " -> " + val);
            }

            System.out.println(this.toString());

            return isOk;
        }
    };
    
    
    public class Mark{
        public String funcName;
        public double time;
        public int num_iter;
        
        public Mark(PinToolMark mark){
            funcName = mark.funcName;
            time = mark.time;
            num_iter = mark.num_iter;
        }
        
        public Mark(String str, double T, int iter){
            funcName = str;
            time = T;
            num_iter = iter;
        }
    };
    
};

