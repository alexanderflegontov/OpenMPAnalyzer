/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * @brief The class to run pintool and wait for it
 * @author HP
 */
public class Trace{
    public static final String RunExe = "/home/hp/Documents/pintool_trigger.exe"; 

    public static final String strPathPin = "/home/hp/Documents/pin-3.5-97503-gac534ca30-gcc-linux/";
    public static final String strRunPinToolexe = strPathPin + "pin";

    public static final String strPathPinTool= strPathPin+"source/tools/ManualExamples/obj-intel64/";
    public static final String StrNamePinTool = "myPinTool.so";

    private final String strPathToAnalyze;
    private final String strAppArgs;
    //private final String strExeParam;

    public Trace(String strExecutableFile, String appArgs){
        strPathToAnalyze = strExecutableFile;
        strAppArgs = appArgs;
        //strExeParam = " -t " + strPathPinTool + StrNamePinTool + " -- " + strPathToAnalyze;
    }

    public void Start() {
        //List<String> params = java.util.Arrays.asList(strRunPinToolexe, 
        //                                " -t" + strPathPinTool + StrNamePinTool , 
        //                                " --" + strPathToAnalyze);
        
        //ProcessBuilder BuilderPinTool = new ProcessBuilder(strRunPinToolexe, strExeParam);
        // List<String> params = java.util.Arrays.asList(RunExe, strRunPinToolexe ,  strPathPinTool + StrNamePinTool, strPathToAnalyze, strAppArgs);
        // ProcessBuilder BuilderPinTool = new ProcessBuilder(params);
        
        ProcessBuilder BuilderPinTool = new ProcessBuilder(RunExe, strRunPinToolexe ,  strPathPinTool + StrNamePinTool, strPathToAnalyze, strAppArgs);
        BuilderPinTool.environment().put( "PINTOOL", strPathPin );
        BuilderPinTool.redirectErrorStream(true);

        Process processPinTool = null;
        try {
            System.out.println("START");
            processPinTool = BuilderPinTool.start();
        } catch (IOException ex) {
            //Logger.getLogger(Form.class.getName()).log(Level.SEVERE, null, ex);
        }

        if(processPinTool != null){

            try ( BufferedReader br = new BufferedReader(new InputStreamReader(processPinTool.getInputStream())) ) {
                System.out.println("[APP OUT]:");
                String line;
                while ((line = br.readLine()) != null) {
                    System.out.println("+ " + line);
                    //jTextField2.add(line, this);
                }
                processPinTool.waitFor();
                System.out.println("END");
            } catch (IOException | InterruptedException ex) {
                //Logger.getLogger(Form.class.getName()).log(Level.SEVERE, null, ex);
            }
        }         
    }
};
