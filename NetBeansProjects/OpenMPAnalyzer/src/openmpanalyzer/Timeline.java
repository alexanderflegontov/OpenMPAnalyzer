/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package openmpanalyzer;

/**
 * @brief The class is a utility for time and Timeline.
 * @author hp
 */
public final class Timeline {

    public static final String[] UNIT = {"ms", "s", "m", "h"};
    public static final double[] SCALAR_FACT = {1000.0, 1.0, 1/60.0, 1/3600.0};
    
    public static int GetPrecision(double DoubleNumber){
        int precision = 0; 
        for(; precision <=1; ++precision){
            DoubleNumber *= 10;
            if((int)(DoubleNumber) % 10 == 0){
                break;
            }
        }
        return precision;
    }

    public static String[] GetIntervalTimes(double intervalTime, int countScaleLine){
        //return GetIntervalTimes(intervalTime, countScaleLine, 0);
        if(intervalTime <= 0.0){
            return null;
        }else{
            String[] strRet = new String[countScaleLine+1];

            int k = GetUnit(intervalTime);

            double num = (intervalTime*1)*SCALAR_FACT[k];
            int prec = GetPrecision(num);
            String strFormat = "%."+String.valueOf(prec)+"f";             

            for(int i = 0; i <= countScaleLine; i++){
                strRet[i] = String.format(strFormat,(intervalTime*i)*SCALAR_FACT[k])+UNIT[k];
            }

            return strRet;   
        }
    }

    public static String[] GetIntervalTimes(double intervalTime, int countScaleLine, double startTime){
        if(intervalTime <= 0.0){
            return null;
        }else{
            String[] strRet = new String[countScaleLine+1];

            int k = GetUnit(intervalTime);

            double num = (intervalTime*1)*SCALAR_FACT[k];
            int prec = GetPrecision(num);
            String strFormat = "%."+String.valueOf(prec)+"f";             

            for(int i = 0; i <= countScaleLine; i++){
                strRet[i] = String.format(strFormat,(intervalTime*i+startTime)*SCALAR_FACT[k])+UNIT[k];
            }

            return strRet;   
        }
    }

    public static String GetFormatTime(double time){
        return GetFormatTime(time, GetUnit(time));
    }

    public static String GetFormatTime(double time, int unit){  
        //return GetFormatTimeOnly()+UNIT[unit];
        if(time < 0.0){
            return null;
        }else{
            double num = (time)*SCALAR_FACT[unit];
            int prec = GetPrecision(num);
            final String strFormat = "%."+String.valueOf(prec)+"f";             

            String strRet = String.format(strFormat, num)+UNIT[unit];
            return strRet;   
        }
    }

    public static String GetFormatTimeOnly(double time, int unit){  
        if(time < 0.0){
            return null;
        }else{
            double num = (time)*SCALAR_FACT[unit];
            int prec = GetPrecision(num);
            final String strFormat = "%."+String.valueOf(prec)+"f";             

            String strRet = String.format(strFormat, num);
            return strRet;   
        }
    }

    public static String GetFormatTimeOnly(double time, int unit, int precision){  
        if(time < 0.0){
            return null;
        }else{
            double num = (time)*SCALAR_FACT[unit];
            //int prec = GetPrecision(num);
            final String strFormat = "%."+String.valueOf(precision)+"f";             

            String strRet = String.format(strFormat, num);
            return strRet;   
        }
    }

    public static int GetUnit(double time){
        int k = 1;
        if(time < 0.0){
            return k;
        }else{
            if (time < 1.0){
                // mseconds 
                k = 0;
            }else if (time < 600.0){
                // seconds if interval of time < 10 min
                k = 1;
            }else if (time < 3600.0){
                // minutes if interval of time < 1 hour
                k = 2;
            }else {//if (intervalTime < 3600.0){
                // hours if interval of time < 1 hour
                k = 3;
            }
            return k;   
        }
    }
}
