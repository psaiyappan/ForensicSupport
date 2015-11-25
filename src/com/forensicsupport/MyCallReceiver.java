package com.forensicsupport;

import java.io.FileOutputStream;
import java.sql.Date;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.TelephonyManager;
import android.util.Log;

public class MyCallReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {

        if (intent.getStringExtra(TelephonyManager.EXTRA_STATE).equals(TelephonyManager.EXTRA_STATE_RINGING)) {
            // This code will execute when the phone has an incoming call

            // get the phone number 
            String incomingNumber = intent.getStringExtra(TelephonyManager.EXTRA_INCOMING_NUMBER);

            String data = "AFSF-Call: Incoming: "+ incomingNumber;
            Log.d("AFSF", data);
            WriteLog.Append(data, 0);
            
            
        } else if (intent.getStringExtra(TelephonyManager.EXTRA_STATE).equals(
                TelephonyManager.EXTRA_STATE_IDLE)
                || intent.getStringExtra(TelephonyManager.EXTRA_STATE).equals(
                        TelephonyManager.EXTRA_STATE_OFFHOOK)) {
            // This code will execute when the call is disconnected
            //Log.d("AFSF", "AFSF-Call: hangup");
        }
    }

	private java.io.FileOutputStream openFileOutput(String filename, int modePrivate) {
		// TODO Auto-generated method stub
		return null;
	}

	private FileOutputStream FileOutputStream(String string, int i) {
		// TODO Auto-generated method stub
		return null;
	}
}