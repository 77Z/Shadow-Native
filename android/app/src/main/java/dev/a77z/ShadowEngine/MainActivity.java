package dev.a77z.ShadowEngine;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.TextView;

import dev.a77z.ShadowEngine.databinding.ActivityMainBinding;

public class MainActivity extends android.app.NativeActivity {

    // Used to load the 'ShadowEngine' library on application startup.
    static {
        //System.loadLibrary("ShadowEngine");
        System.loadLibrary("c++_shared");
        System.loadLibrary("examplesRelease");
    }

    private static final int readExternalStoragePermission = 1;

    //private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED)
        {
            // We absolutely need this permission to run some of the examples, so ask it again even if it was refused before.
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, readExternalStoragePermission);
        }

        //binding = ActivityMainBinding.inflate(getLayoutInflater());
        //setContentView(binding.getRoot());

        // Example of a call to a native method
        //TextView tv = binding.sampleText;
        //tv.setText(stringFromJNI());
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults)
    {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode)
        {
            case readExternalStoragePermission:
            {
                if (grantResults.length > 0 && grantResults[0] != PackageManager.PERMISSION_GRANTED)
                {
                    // Permission refused, show a warning!
                    AlertDialog alertDialog = new AlertDialog.Builder(this).create();
                    alertDialog.setTitle("Warning");
                    alertDialog.setMessage("Some examples needs to load data from the sdcard, those will crash if data can't be loaded!");
                    alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK", new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int which)
                        {
                            dialog.dismiss();
                        }
                    });
                    alertDialog.show();
                }
            }
        }
    }

    /**
     * A native method that is implemented by the 'ShadowEngine' native library,
     * which is packaged with this application.
     */
    //public native String stringFromJNI();
}