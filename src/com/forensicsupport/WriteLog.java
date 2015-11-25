package com.forensicsupport;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import android.os.Environment;
import android.util.Log;

public class WriteLog 
{
	static String SdCardLoc = Environment.getExternalStorageDirectory().toString();
	static String ForensicLocation = "";//SdCardLoc + "forensic/";
    public static void Append(String data, int flag)
	{
    	ForensicLocation = SdCardLoc + "/forensic/";
        SimpleDateFormat DateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
        Calendar c = Calendar.getInstance();
        String currentDateTimeString = DateFormat.format(c.getTime());
		try 
		{
			String flagStr = "";
			switch(flag)
			{
			case 0:
				flagStr = "-Call";
				break;				
			case 1:
				flagStr = "-GPS";
				break;
			case 2:
				flagStr = "-SMS";
				break;
			case 3:
				flagStr = "-Image";
				break;
			case 4:
				
				break;
			default:
					break;
			}
			
			boolean dirCreated = createDir();
			
			String FileName = "/sdcard/forensic/log" + flagStr + ".txt"; 
			File myFile = new File(FileName);  
			if(!myFile.exists())
			{
			myFile.createNewFile();
			}
			FileOutputStream fOut = new FileOutputStream(myFile,true);  //true for append
			OutputStreamWriter myOutWriter = new OutputStreamWriter(fOut);
			
			myOutWriter.append(currentDateTimeString + " " + data + "\r\n");  
			myOutWriter.close();  
			fOut.close();			
		} 
		catch (FileNotFoundException e1) 
		{
			e1.printStackTrace();
		}  
		catch (IOException e2) 
		{
			e2.printStackTrace();
		}  
	
	}
    public static boolean createDir() {

    	String outputPath = "/sdcard/forensic/";
    	boolean success = false;
        InputStream in = null;
        OutputStream out = null;
        try {

            //create output directory if it doesn't exist
            File dir = new File (outputPath); 
            if (!dir.exists())
            {
                dir.mkdirs();
            }
            success = true;
        }
        catch(Exception ex )
        {
        	
        }
        return success;
    }
    public static void copyFile(String inputPath, String inputFile) {

    	String outputPath = "/sdcard/forensic/";
    	
        InputStream in = null;
        OutputStream out = null;
        try {

            //create output directory if it doesn't exist
            File dir = new File (outputPath); 
            if (!dir.exists())
            {
                dir.mkdirs();
            }


            in = new FileInputStream(inputPath);        
            out = new FileOutputStream(outputPath + inputFile);

            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            
            in.close();
            in = null;

                // write the output file (You have now copied the file)
                out.flush();
            out.close();
            out = null;        

        }  catch (FileNotFoundException fnfe1) {
            Log.e("tag", fnfe1.getMessage());
        }
                catch (Exception e) {
            Log.e("tag", e.getMessage());
        }

    }
}
	 