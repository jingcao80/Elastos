package com.xxg.browser;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.webkit.URLUtil;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class BrowserActivity extends Activity {
	
	WebView wv;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().requestFeature(Window.FEATURE_PROGRESS);
        setContentView(R.layout.main);
        
       wv=(WebView)findViewById(R.id.wv);
       wv.setWebChromeClient(new WebChromeClient(){
    	   @Override
    	   public void onProgressChanged(WebView view,int newProgress){
    		   BrowserActivity.this.setProgress(newProgress*100);
    	   }
    	   
       });
       wv.setWebViewClient(new WebViewClient(){
    	   @Override
    	   public void onReceivedError(WebView view, int errorCode, String description, String failingUrl){
    		   Toast.makeText(BrowserActivity.this,"sorry"+description,Toast.LENGTH_SHORT).show();
    		   
    	   }
    	   
       });
       
       Button btn1 = (Button)findViewById(R.id.btn1);
       Button btn2 = (Button)findViewById(R.id.btn2);
       Button btn3 = (Button)findViewById(R.id.btn3);
       
       btn3.setOnClickListener(new View.OnClickListener() {
		
		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			EditText et=(EditText)findViewById(R.id.et); 
			String url=et.getText().toString().trim();
			if(URLUtil.isNetworkUrl(url)){
				wv.loadUrl(url);
			}else{
				Toast.makeText(BrowserActivity.this, "sorry,网址错误", Toast.LENGTH_SHORT).show();
				et.requestFocus();
			}
			
		}
	});
       btn2.setOnClickListener(new View.OnClickListener() {
		
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
		
			if(wv.canGoForward()){
				wv.goForward();
			}else{
				Toast.makeText(BrowserActivity.this, "sorry,不能前进", Toast.LENGTH_SHORT).show();
			}
		}
	});
       btn1.setOnClickListener(new View.OnClickListener() {
   		
   		@Override
   		public void onClick(View v) {
   			// TODO Auto-generated method stub
   		
   			if(wv.canGoBack()){
   				wv.goBack();
   			}else{
   				Toast.makeText(BrowserActivity.this, "sorry,不能返回", Toast.LENGTH_SHORT).show();
   			}
   		}
   	});
    }
}