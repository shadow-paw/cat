package com.shadowpaw.cat;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity {
    private CatView mView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new CatView(this);
        setContentView(mView);
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        setContentView(new View(this));
        mView.onDestroy();
        mView = null;
    }
    @Override
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }
    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }
}
