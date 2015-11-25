package com.forensicsupport;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.util.Log;

public class CameraReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO Auto-generated method stub 
      //  abortBroadcast();
        Log.d("AFSF", "AFSF-Photo: New Photo Clicked");
        Cursor cursor = context.getContentResolver().query(intent.getData(), null, null, null, null);
        cursor.moveToFirst();
        String image_path = cursor
            .getString(cursor.getColumnIndex("_data"));
        String data = "AFSF-Photo: New Photo is Saved as : " + image_path;
        Log.d("AFSF", data);
        Log.d("AbFSF", "AbFSF" +  image_path);
        //Add Log
        WriteLog.Append(data, 3);
        //Add to forensic
        String getImageFileName =  image_path.substring(image_path.lastIndexOf("/") + 1); //gets the actual ImageFileName
        
        WriteLog.copyFile(image_path, getImageFileName);
        //Add to cloud
    }



}