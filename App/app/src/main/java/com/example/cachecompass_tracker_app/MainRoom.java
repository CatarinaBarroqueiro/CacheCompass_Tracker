package com.example.cachecompass_tracker_app;

import androidx.appcompat.app.AppCompatActivity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.widget.ImageView;
import android.widget.TextView;

public class MainRoom extends AppCompatActivity implements SensorEventListener {

    private TextView textView;
    private ImageView imgView;
    private SensorManager sensorManager;
    private Sensor accelerometerSensor,
            magnetometerSensor;

    private float[] lastAccelerometer = new float[3],
            lastMagnetometer = new float[3],
            rotationMatrix = new float[9],
            orientation = new float[3];

    boolean isLastAccelerometerArrayCopied,
            isLastMagnetometerArrayCopied = false;
    long lastUpdatedTime = 0;
    float currentDegree = 0f;




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_room);

        textView = findViewById(R.id.degrees);
        imgView = findViewById(R.id.compass);

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        accelerometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        magnetometerSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);







    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor == accelerometerSensor) {
            System.arraycopy(event.values,0,lastAccelerometer,0,event.values.length);
            isLastAccelerometerArrayCopied = true;

        }else if(event.sensor == magnetometerSensor) {
            System.arraycopy(event.values,0,lastMagnetometer,0,event.values.length);
            isLastMagnetometerArrayCopied = true;
        }

        if (isLastMagnetometerArrayCopied &&
                isLastAccelerometerArrayCopied &&
                System.currentTimeMillis() - lastUpdatedTime > 250){
            SensorManager.getRotationMatrix(rotationMatrix, null, lastAccelerometer, lastMagnetometer);
            SensorManager.getOrientation(rotationMatrix, orientation);

            float azimuthInRadians = orientation[0];
            float azimuthInDegree = (float) Math.toDegrees(azimuthInRadians);

            RotateAnimation rotateAnimation = new RotateAnimation(
                    currentDegree,
                    -azimuthInDegree,
                    Animation.RELATIVE_TO_SELF,
                    0.5f,
                    Animation.RELATIVE_TO_SELF,
                    0.5f
                    );
            rotateAnimation.setDuration(250);
            rotateAnimation.setFillAfter(true);
            imgView.startAnimation(rotateAnimation);

            currentDegree = -azimuthInDegree;
            lastUpdatedTime = System.currentTimeMillis();

            int x = (int) azimuthInDegree;
            textView.setText(x+"°");

        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    // CTRL+O -> procurar resume
    @Override
    protected void onPostResume() {
        super.onPostResume();
        // Registar
        sensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_NORMAL);
        sensorManager.registerListener(this, magnetometerSensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    // CTRL+O -> procurar pause
    @Override
    protected void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this, accelerometerSensor);
        sensorManager.unregisterListener(this, magnetometerSensor);
    }
}