package com.forensicsupport;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;

import android.app.Activity;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	

//	  Button buttonWatch, buttonExit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
    		NativeLib nativeLib = new NativeLib();
    	
      }
    
    public static boolean canRunRootCommands()
    {
       boolean retval = false;
       Process suProcess;

       try
       {
          suProcess = Runtime.getRuntime().exec("su");

          DataOutputStream os = new DataOutputStream(suProcess.getOutputStream());
          DataInputStream osRes = new DataInputStream(suProcess.getInputStream());

          if (null != os && null != osRes)
          {
             // Getting the id of the current user to check if this is root
             os.writeBytes("id\n");
             os.flush();

             String currUid = osRes.readLine();
             boolean exitSu = false;
             if (null == currUid)
             {
                retval = false;
                exitSu = false;
                Log.d("ROOT", "AFSF: Can't get root access or denied by user");
             }
             else if (true == currUid.contains("uid=0"))
             {
                retval = true;
                exitSu = true;
                Log.d("ROOT", "AFSF: Root access granted");
             }
             else
             {
                retval = false;
                exitSu = true;
                Log.d("ROOT", "AFSF: Root access rejected: " + currUid);
             }

             if (exitSu)
             {
                os.writeBytes("exit\n");
                os.flush();
             }
          }
       }
       catch (Exception e)
       {
          // Can't get root !
          // Probably broken pipe exception on trying to write to output stream (os) after su failed, meaning that the device is not rooted

          retval = false;
          Log.d("ROOT", "AFSF: Root access rejected [" + e.getClass().getName() + "] : " + e.getMessage());
       }

       return retval;
    }
    
    }