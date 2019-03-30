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
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.security.InvalidParameterException;

import static javafxapplication2.MappingFuncNames.FUNC_NAME_TO_FUNC_ID;

/**
 * @brief The class to process .out text file
 * @author hp
 */
public class DataAnalyzer{
    private List<PinToolMark> listTHMarks;

    public static final String STR_DELIMITER_CONF = "=";
    public static final String STR_NUM_THREADS = "num_threads";
    public static final String STR_NAME_OUT = "name_out";
    
    public static final String DEFAULT_PINTOOL_OUT_PATH = ""; // "./"
    public static final String DEFAULT_PINTOOL_OUT_CONF = "config.out";
    //public static final String DEFAULT_PINTOOL_OUT = "myPinTool.out"; 

    public DataAnalyzer(){
        listTHMarks = new ArrayList<>();
    }

    public static boolean RemovePreviousResultFiles(final String pathToPrevOutFile){
        System.out.println("[Clear previous results]: ");
        boolean ret_val = false;
        try{
            //File fileOutForAnalysis = new File(pathToPrevOutFile+DEFAULT_PINTOOL_OUT);
            //boolean resultRemoveOutFile = Files.deleteIfExists(fileOutForAnalysis.toPath());
            boolean bNotError = true;
            ArrayList<String> namesOutConfFiles = ReadConfigFile(
                                    pathToPrevOutFile+DEFAULT_PINTOOL_OUT_CONF);
            if(namesOutConfFiles != null){
                // config file is found and is valid
                for(String name: namesOutConfFiles){
                    final Path threadOutFilePath = new File(name).toPath();
                    final boolean bOk = Files.deleteIfExists(threadOutFilePath);
                    if(bOk){
                        System.out.println(threadOutFilePath + "is removed");
                    }
                    else{
                        bNotError = false;
                        System.out.println(threadOutFilePath + "is NOT removed!");
                    }
                }
            }

            final boolean isConfFileRemoved = Files.deleteIfExists(
                    new File(pathToPrevOutFile+DEFAULT_PINTOOL_OUT_CONF).toPath());
            ret_val = isConfFileRemoved && bNotError;
        }catch (IOException ex) {
            Logger.getLogger(DataAnalyzer.class.getName()).log(Level.SEVERE, null, ex);
        }
        return ret_val;
    }

    public static boolean IsConfigFile(final String strPinToolOutfile){
        return DEFAULT_PINTOOL_OUT_CONF.equals(strPinToolOutfile);
    }

    public Map Start(final String strPinToolOutfile){
        System.out.println("============= AnalyzeData::Start() - try config file proccessing ========");
        Map threadStatisticsTable = StartConfingFileProcessing(strPinToolOutfile);
        
        //TreeMap<Integer, List<Mark>> sorted_map = new TreeMap<Integer, List<Mark>>();
        //sorted_map.putAll(hashTableThreadMarks);
        //System.out.println("unsorted map: " + hashTableThreadMarks);
        ArrayList<Integer> ArrThreadId = new ArrayList<>(threadStatisticsTable.keySet());
        System.out.println("AnalyzeData::ThreadCount = " + String.valueOf(ArrThreadId.size()));
        for(int i = 0; i < ArrThreadId.size(); ++i)// Map.Entry e : sorted_map.entrySet()
        {
            System.out.println("Thread("+ArrThreadId.get(i)+") is a " + String.valueOf(i));
        }
        return threadStatisticsTable;
    } 

    public static Map StartConfingFileProcessing(final String strPinToolOutfile) {
        ArrayList<String> namesOutConfFiles = ReadConfigFile(strPinToolOutfile);
        if(namesOutConfFiles == null){
            throw new InvalidParameterException("config file = " + strPinToolOutfile);
        }
        System.out.println("The config file is found and is valid");

        Map retThreadMarksTable = new TreeMap<>();
        DataAnalyzer dataAnalyzer = new DataAnalyzer();
        for(String threadOutFileName : namesOutConfFiles) {
            System.out.println("threadOutFileName = " + threadOutFileName);
            Map hashTable = dataAnalyzer.StartProccessingOfThreadTraceFile(threadOutFileName);
            if(hashTable!= null) {
                System.out.println("Merging the contents of all output files");
                retThreadMarksTable.putAll(hashTable);
            }else{
                // fail
                throw new InvalidParameterException("threadOutFileName = " + threadOutFileName);
            }
        }
        return retThreadMarksTable;
    }

    private static ArrayList<String> ReadConfigFile(final String strPinToolOutfile){
        System.out.println("IN: "+strPinToolOutfile);
        ArrayList<String> retOutputThreadFileNames = null;
        File fileOutForAnalysis = new File(strPinToolOutfile);

        if(fileOutForAnalysis.isFile() && fileOutForAnalysis.canRead()) {

            final String configPath = fileOutForAnalysis.getAbsolutePath();
            final String configDirPath = configPath.
                    substring(0,configPath.lastIndexOf(File.separator)) + File.separator;
            System.out.println("absolute PathToConfig = " + configPath);
            System.out.println("Dir path to config =  " + configDirPath);
            
            BufferedReader reader = null;
            try {
                retOutputThreadFileNames = new ArrayList<>(); 
                reader = new BufferedReader(new FileReader(fileOutForAnalysis));
                int num_threads = -1;
                String lineText;
                while ((lineText = reader.readLine()) != null) {
                    /*
                    The format:
                    num_threads=6
                    name_out="INFO/0.out"
                    name_out="INFO/1.out"
                    */
                    StringTokenizer st = new StringTokenizer(lineText.trim(), STR_DELIMITER_CONF);           
                    while(st.hasMoreTokens()) {
                        final String key = st.nextToken();
                        final String val = st.nextToken();
                        switch(key) {
                            case STR_NAME_OUT:
                                // val is "INFO/0.out"
                                retOutputThreadFileNames.add(configDirPath+val);
                                break;
                            case STR_NUM_THREADS:
                                // val is 6
                                num_threads = Integer.parseUnsignedInt(val);
                                break;
                            default:
                                System.out.println("Unknown format =  " + key + " : " + val);
                                return null;
                        }
                        System.out.println(key + " -> " + val);
                    }
                }

                // Validation test, like CRC :)
                if(num_threads != retOutputThreadFileNames.size()) {
                    System.out.println("File .out is not invalid!");
                    return null;
                }
                System.out.println("END");
            } catch (FileNotFoundException e) {
                System.out.println("File .out is not found for analysis!");
                return null;
            } catch (IOException e) {
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
        return retOutputThreadFileNames;
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
    
    public Map StartOneFileProccessing(final String outputTH_filePath){
        Map retThreadMarksTable = null;

        File outputTHFile = new File(outputTH_filePath);
        if(outputTHFile.isFile() && outputTHFile.canRead()){
            System.out.println("StartOneFileProccessing");

            BufferedReader reader = null;
            try {

                retThreadMarksTable = new TreeMap<Integer, List<Mark>>(); 
                reader = new BufferedReader(new FileReader(outputTHFile));
                
                listTHMarks.clear(); // clear previous data of analysis
                String text;
                while ((text = reader.readLine()) != null){

                    PinToolMark mark = new PinToolMark();
                    boolean isDecodeOk = mark.ProcessingHumanFormat(text);

                    if(isDecodeOk == true && mark.threadId != null){// && mark.time >= 0){
                        if(Filter(mark)){
                            continue;
                        }
                        
                        Mark mrk = new Mark(mark);
                        List<Mark> getListMarks = (List<Mark>) retThreadMarksTable.get(mark.threadId);
                        if(getListMarks != null){
                            getListMarks.add(mrk);
                        }else{
                            getListMarks = new ArrayList<>();
                            getListMarks.add(mrk);
                            retThreadMarksTable.put(mark.threadId, getListMarks);
                        }
                        listTHMarks.add(mark);
                    }
                    else{
                        System.out.println(" ProcessingHumanFormat fails !!!"); 
                    }
                }

                //TreeMap<Integer, List<Mark>> sorted_map = new TreeMap<Integer, List<Mark>>();
                //sorted_map.putAll(hashTableThreadMarks);
                //System.out.println("unsorted map: " + hashTableThreadMarks);
                ArrayList<Integer> ArrThreadId = new ArrayList<>(retThreadMarksTable.keySet());
                System.out.println("AnalyzeData::ThreadCount = " + String.valueOf(ArrThreadId.size()));
                for(int i = 0; i < ArrThreadId.size(); ++i)// Map.Entry e : sorted_map.entrySet()
                {
                    System.out.println("Thread("+ArrThreadId.get(i)+") is a " + String.valueOf(i));
                }
            } catch (FileNotFoundException e) {
                System.out.println("File .out is not found for analysis!");
            } catch (IOException e) {
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
        return retThreadMarksTable;
    }

    /**
     * @brief The class to process one text line in PinToolMark
     */
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
        public Integer funcId;
        public double time;

        PinToolMark(){
            threadId = -1;
            funcName = "";
            funcId = -1;
            time = 0.0;
        }

        @Override
        public String toString(){
                String decodedTextLine = STR_THREAD + STR_DELIMITER + threadId + " ";
                decodedTextLine += STR_OPERATION + STR_DELIMITER + funcName + " ";
                decodedTextLine += STR_TIME + STR_DELIMITER + time;
                return decodedTextLine;
        }

        public boolean ProcessingHumanFormat(String lineText) {
            System.out.println("IN: "+lineText);

            //PinToolMark mark = new PinToolMark();
            boolean bOk = true;
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
                        this.funcId = FUNC_NAME_TO_FUNC_ID.get(this.funcName);
                        break;
                    case STR_TIME:
                        this.time = Double.parseDouble(val);
                        break;
                    default:
                        System.out.println("unknown format =  " + key + " : " + val);
                        bOk = false;
                        //this = null;
                        break;
                }
                System.out.println(key + " -> " + val);
            }

            System.out.println(this.toString());
            return bOk;
        }
        
        public boolean ProcessingLineText(final String lineText) {
            System.out.println("IN: "+lineText);
            try {
                String[] tokens = lineText.trim().split(",");
                Integer opId = Integer.parseInt(tokens[0]);
                this.funcId = opId;
                this.funcName = MappingFuncNames.LOG_OPER[opId];
                this.time = Double.parseDouble(tokens[1]);
                System.out.println(this.toString());
            }
            catch(NumberFormatException e){
                System.out.println(" ProcessingLineText fails !!!"); 
                return false;
            }
            return true;   
        }
    };
    
    public class Mark{
        public Integer funcId;
        public double time;

        public Mark(PinToolMark mark){
            funcId = mark.funcId;
            time = mark.time;
        }

        public Mark(String funcName, double T){
            funcId = FUNC_NAME_TO_FUNC_ID.get(funcName);
            time = T;
        }
    };
    
    public Map StartProccessingOfThreadTraceFile(final String outputTH_filePath){
        Map retThreadMarksTable = null;

        File outputTHFile = new File(outputTH_filePath);
        if(outputTHFile.isFile() && outputTHFile.canRead()){
            System.out.println("StartOneFileProccessing");
            
            final String threadTraceFileName = outputTHFile.getName();
            
            final String ttFileName_without_extension = threadTraceFileName.
                                substring("TH".length(),threadTraceFileName.lastIndexOf("."));
            System.out.println("threadTraceFileName = " + threadTraceFileName);
            System.out.println("ttFileName_without_extension =  " + ttFileName_without_extension);
            Integer tid;
            try {
                tid = Integer.parseUnsignedInt(ttFileName_without_extension);
            }
            catch(NumberFormatException e){
                // legacy
                return StartOneFileProccessing(outputTH_filePath);
            }
            
            BufferedReader reader = null;
            try {
                retThreadMarksTable = new TreeMap<Integer, LinkedList<Mark>>(); 
                reader = new BufferedReader(new FileReader(outputTHFile));
                
                listTHMarks.clear(); // clear previous data of analysis
                String text;
                while ((text = reader.readLine()) != null){

                    PinToolMark mark = new PinToolMark();
                    if(mark.ProcessingLineText(text)) {
                        if(Filter(mark)) {
                            continue;
                        }
                        
                        Mark mrk = new Mark(mark);
                        List<Mark> getListMarks = (LinkedList<Mark>) retThreadMarksTable.get(tid);
                        if(getListMarks != null){
                            getListMarks.add(mrk);
                        }else{
                            getListMarks = new LinkedList<>();
                            getListMarks.add(mrk);
                            retThreadMarksTable.put(tid, getListMarks);
                        }
                        listTHMarks.add(mark);
                    }
                }

            } catch (FileNotFoundException e) {
                System.out.println("File .out is not found for analysis!");
            } catch (IOException e) {
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
        return retThreadMarksTable;
    }
};
