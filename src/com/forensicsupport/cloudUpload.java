package com.forensicsupport;

import java.io.File;
import java.io.FileInputStream;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.impl.client.DefaultHttpClient;

import android.os.Environment;

public class cloudUpload {

	public static boolean uploadFunction(String filepath, String fileName)
	{
		String url = "http://truefriends.comuf.com/cloud/image-save.php";
		File file = new File(filepath, fileName);
		try 
		{
		    HttpClient httpclient = new DefaultHttpClient();
		    HttpPost httppost = new HttpPost(url);
		    InputStreamEntity reqEntity = new InputStreamEntity(new FileInputStream(file), -1);
		    reqEntity.setContentType("binary/octet-stream");
		    reqEntity.setChunked(true); // Send in multiple parts if needed
		    httppost.setEntity(reqEntity);
		    HttpResponse response = httpclient.execute(httppost);
		    //Do something with response...
		    return true;

		} 
		catch (Exception e) 
		{
		    // show error
			return false;
			
		}
		
	}
}
