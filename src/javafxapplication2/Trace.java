/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package javafxapplication2;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 *
 * @author HP
 */
public class Trace{
            public static final String RunExe = "/home/hp/Documents/pintool_trigger.exe"; 
            public static final String StrNamePinTool = "myPinTool.so";
            
            String strPathToAnalyze;
            String strPathPin;
            String strRunPinToolexe;
            String strPathPinTool;
            String strExeParam;
            String strAppArgs;
            
            Trace(String strExecutableFile, String appArgs){
                strPathToAnalyze = strExecutableFile;
                strAppArgs = appArgs;
                
                strPathPin = "/home/hp/Downloads/pin-3.5-97503-gac534ca30-gcc-linux/";
                strRunPinToolexe = strPathPin + "pin";
                strPathPinTool = strPathPin+"source/tools/ManualExamples/obj-intel64/";
                strExeParam = " -t " + strPathPinTool + StrNamePinTool + " --" + strPathToAnalyze;
            }
            
            
        public void Start(){
                            
                ///ProcessBuilder BuilderPinTool = new ProcessBuilder(strRunPinToolexe, strExeParam);
//                List<String> params = java.util.Arrays.asList(RunExe, strRunPinToolexe ,  strPathPinTool + StrNamePinTool, strPathToAnalyze, strAppArgs);//"-arg1", "-arg2");
//                ProcessBuilder BuilderPinTool = new ProcessBuilder(params);
                ProcessBuilder BuilderPinTool = new ProcessBuilder(RunExe, strRunPinToolexe ,  strPathPinTool + StrNamePinTool, strPathToAnalyze, strAppArgs);  
                
                BuilderPinTool.environment().put( "PINTOOL", strPathPin );
                BuilderPinTool.redirectErrorStream( true );

                Process processPinTool = null;
                try {
                    System.out.println("START");
                    processPinTool = BuilderPinTool.start();
                } catch (IOException ex) {
                    //Logger.getLogger(Form.class.getName()).log(Level.SEVERE, null, ex);
                }

////
                if(processPinTool != null){
                    //String StrBegin = "Out of OpenMP app:\n";
                    //jTextField2.setText("XAXA:");
                    //jTextField2.add(StrBegin, this);
                    try ( BufferedReader br = new BufferedReader(new InputStreamReader(processPinTool.getInputStream())) ) {
                        System.out.println("[APP OUT]:");
                        String line;
                        while ((line = br.readLine()) != null) {
                            System.out.println("+ " + line);
                            //jTextField2.add(line, this);
                        }
                    } catch (IOException ex) {
                        //Logger.getLogger(Form.class.getName()).log(Level.SEVERE, null, ex);
                    }

                    try {
                        processPinTool.waitFor();
                        System.out.println("END");
                    } catch (InterruptedException ex) {
                        //Logger.getLogger(Form.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
////                
        }
    
        
        public void Start1() throws IOException{
            List<String> params = java.util.Arrays.asList(strRunPinToolexe, 
                                                    " -t" + strPathPinTool + StrNamePinTool , 
                                                    " --" + strPathToAnalyze);//"-arg1", "-arg2");
            
            List<String> command = new ArrayList<>();
            command.addAll(params);


            ProcessBuilder builder = new ProcessBuilder(command);
            Map<String, String> environ = builder.environment();

            final Process process = builder.start();
            InputStream is = process.getInputStream();
            InputStreamReader isr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(isr);
            String line;
            while ((line = br.readLine()) != null) {
              System.out.println(line);
            }
            System.out.println("Program terminated!");
    
            /*
            ProcessBuilder pb = new ProcessBuilder(params);
                Map<String, String> env = pb.environment();
                env.put("PINTOOL", strPathPin);
                //env.put("OMP_NUM_THREADS", String.valueOf(2));
                //env.remove("OTHERVAR");
                //env.put("VAR2", env.get("VAR1") + "suffix");
                //pb.directory(new File("myDir"));
                File log = new File("log");
                pb.redirectErrorStream(true);
                pb.redirectOutput(Redirect.appendTo(log));
                Process p = pb.start();
                assert pb.redirectInput() == Redirect.PIPE;
                assert pb.redirectOutput().file() == log;
                assert p.getInputStream().read() == -1;
            */
        }
    
};
