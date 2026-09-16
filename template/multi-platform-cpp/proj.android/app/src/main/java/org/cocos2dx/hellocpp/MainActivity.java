package org.cocos2dx.hellocpp;

import android.os.Bundle;

import org.cocos2dx.lib.Cocos2dxActivity;

public class MainActivity extends Cocos2dxActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
    }


    static {
        // name "cocos2dcpp" getting from LOCAL_MODULE_FILENAME in Android.mk
        System.loadLibrary("cocos2dcpp");
    }
}