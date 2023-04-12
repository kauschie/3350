// Name: Michael Kausch
// Lab 11
// Software Engineering CMPS 3350
// Date: 4/11/23

package com.example.mkausch;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.view.View;
import android.widget.TextView;
import android.os.Handler;
import android.view.MotionEvent;
import android.app.Activity;
import android.widget.Toast;
import android.content.Context;
import android.util.DisplayMetrics;

public class MainActivity extends AppCompatActivity {

    private boolean isTouch = false;
    public static int startX=0;
    public static int startY=0;
    public static int endX=0;
    public static int endY=0;

//    public bool changed=false;

    DisplayMetrics displayMetrics;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void changeText(View view){
        TextView text=(TextView)findViewById(R.id.myText);
        text.setText("Michael Kausch's app");

        Context context = getApplicationContext();
        CharSequence text_msg = "by Michael Kausch";
        int duration = Toast.LENGTH_SHORT;

        Toast toast = Toast.makeText(context, text_msg, duration);
        toast.show();

    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {


        int eventAction = event.getAction();

        switch (eventAction) {
            case MotionEvent.ACTION_DOWN:
                startX = (int) event.getX();
                startY = (int) event.getY();
//                Toast.makeText(this, "ACTION_DOWN AT COORDS "+"X: "+startX+" Y: "+startY, Toast.LENGTH_SHORT).show();
                isTouch = true;

                break;

            case MotionEvent.ACTION_MOVE:
//                Toast.makeText(this, "MOVE "+"X: "+X+" Y: "+Y, Toast.LENGTH_SHORT).show();
                int newX = (int) event.getX();
                int newY = (int) event.getY();

                if ((newX - startX) > 200 || (newX - startX) < -200) {
                    startX = newX;
                    startY = newY;
//                    Toast.makeText(this, "Changed X by too much, newX "+"X: "+endX+" newY: "+endY, Toast.LENGTH_SHORT).show();
                }

                break;

            case MotionEvent.ACTION_UP:
                endX = (int) event.getX();
                endY = (int) event.getY();

//                Toast.makeText(this, "ACTION_UP "+"X: "+endX+" Y: "+endY, Toast.LENGTH_SHORT).show();
//                Toast.makeText(this, "ACTION_UP originally AT COORDS "+"X: "+startX+" Y: "+startY, Toast.LENGTH_SHORT).show();

                displayMetrics = getResources().getDisplayMetrics();
                int height = displayMetrics.heightPixels;
                int delta = (endY - startY);

//                Toast.makeText(this, "Half-Height is " + (height/2), Toast.LENGTH_SHORT).show();
//                Toast.makeText(this, "Difference is " + delta, Toast.LENGTH_SHORT).show();

                if ((endY - startY) > (height/2.0)) {
//                    Toast.makeText(this, "Swipe was greater than half screen", Toast.LENGTH_SHORT).show();
                    System.exit(0);
                }

                break;
        }
        return true;
    }

}