package com.forensicsupport;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.os.Bundle;
import android.util.Log;

public class GpsLocationReceiver extends BroadcastReceiver implements LocationListener
{        
    @Override
    public void onReceive(Context context, Intent intent) 
    {
    	
		 if (intent.getAction().matches("android.location.PROVIDERS_CHANGED")) 
		 {
			 Log.d("AFSF", "AFSF-location: PROVIDERS_CHANGED");			
		 }		
    }

	@Override
	public void onLocationChanged(Location location) {
		// TODO Auto-generated method stub
		 Log.d("AFSF", "AFSF-location: PROVIDERS_CHANGED");
	}

	@Override
	public void onProviderDisabled(String provider) {
		// TODO Auto-generated method stub
		 Log.d("AFSF", "AFSF-location: PROVIDERS_CHANGED");
	}

	@Override
	public void onProviderEnabled(String provider) {
		// TODO Auto-generated method stub
		 Log.d("AFSF", "AFSF-location: PROVIDERS_CHANGED");
	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		// TODO Auto-generated method stub
		 Log.d("AFSF", "AFSF-location: PROVIDERS_CHANGED");
	}
}