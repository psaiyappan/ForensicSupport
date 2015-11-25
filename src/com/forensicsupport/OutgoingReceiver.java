package com.forensicsupport;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class OutgoingReceiver extends BroadcastReceiver {
    public OutgoingReceiver() {
    	//Log.d("AFSF", "AFSF-Call: Outgoing: ");
    	
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String number = intent.getStringExtra(Intent.EXTRA_PHONE_NUMBER);
        //Log.d("AFSF", "AFSF-Call: Outgoing: "+ number);
        String data = "AFSF-Call: Outgoing: "+ number;
        Log.d("AFSF", data);
        WriteLog.Append(data, 0);
    }

}