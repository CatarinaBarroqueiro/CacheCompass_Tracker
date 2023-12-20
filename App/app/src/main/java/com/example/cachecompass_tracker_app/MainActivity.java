package com.example.cachecompass_tracker_app;

import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.InputType;
import android.view.MotionEvent;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.example.cachecompass_tracker_app.R;
import com.google.android.material.button.MaterialButton;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class MainActivity extends AppCompatActivity {

    private MaterialButton btnSignUp;
    private String userPassword;
    private short userId;
    private EditText passwordEditText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnSignUp = findViewById(R.id.signupbtn);
        passwordEditText = findViewById(R.id.password);

        TextView username = findViewById(R.id.mail);
        TextView password = findViewById(R.id.password);

        MaterialButton loginbtn = findViewById(R.id.loginbtn);

        loginbtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String userEmail = username.getText().toString();
                userPassword = password.getText().toString();

                // Call the method to get user data
                getUserData(userEmail);
            }
        });
        // Eye password
        passwordEditText.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_UP) {
                    // Increase the target area for the eye icon
                    int targetArea = 150; // You can adjust this value

                    if (event.getRawX() >= (passwordEditText.getRight() - targetArea)) {
                        // Toggle between visible and invisible password
                        if (passwordEditText.getInputType() == InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD) {
                            passwordEditText.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
                        } else {
                            passwordEditText.setInputType(InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
                        }
                        passwordEditText.setSelection(passwordEditText.getText().length());
                        return true;
                    }
                }
                return false;
            }
        });
        btnSignUp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openSignUp();
            }
        });

    }

    public void openSignUp() {
        Intent view = new Intent(this, SignUpActivity.class);
        startActivity(view);
    }

    public void openMainRoom(short userId) {
        Intent intent = new Intent(this, MainRoom.class);
        intent.putExtra("userId", userId);
        startActivity(intent);
    }

    private void getUserData(String email) {
        new RequestTask().execute("http://51.20.64.70:3000/user/email?email=" + email);
    }

    private class RequestTask extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... uri) {
            String responseString = null;
            HttpURLConnection urlConnection = null;

            try {
                URL url = new URL(uri[0]);
                urlConnection = (HttpURLConnection) url.openConnection();

                int responseCode = urlConnection.getResponseCode();
                if (responseCode == HttpURLConnection.HTTP_OK) {
                    BufferedReader in = new BufferedReader(new InputStreamReader(urlConnection.getInputStream()));
                    StringBuilder response = new StringBuilder();
                    String inputLine;
                    while ((inputLine = in.readLine()) != null) {
                        response.append(inputLine);
                    }
                    in.close();
                    responseString = response.toString();
                } else {
                    // Handle the error
                    throw new IOException("HTTP error code: " + responseCode);
                }
            } catch (Exception e) {
                e.printStackTrace();
                // Handle exceptions...
            } finally {
                if (urlConnection != null) {
                    urlConnection.disconnect();
                }
            }

            return responseString;
        }

        @Override
        protected void onPostExecute(String result) {
            super.onPostExecute(result);
            if (result != null) {
                String storedPassword = parseJsonResponse(result);
                if (storedPassword != null && userPassword.equals(storedPassword)) {
                    userId = parseUserId(result);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            //Toast.makeText(MainActivity.this, "Login Successful", Toast.LENGTH_SHORT).show();
                            Toast.makeText(MainActivity.this, "Login Successful + " + storedPassword, Toast.LENGTH_SHORT).show();
                            Toast.makeText(MainActivity.this, "ID + " + userId, Toast.LENGTH_SHORT).show();

                            openMainRoom(userId);
                        }
                    });
                } else {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.this, "Login Failed", Toast.LENGTH_SHORT).show();
                        }
                    });
                }
            } else {
                // Handle null response
            }
        }
    }

    private String parseJsonResponse(String jsonResponse) {
        try {
            JSONArray jsonArray = new JSONArray(jsonResponse);
            if (jsonArray.length() > 0) {
                JSONObject userData = jsonArray.getJSONObject(0);
                return userData.getString("password");
            }
        } catch (JSONException e) {
            e.printStackTrace();
            // Handle JSON parsing error
            // ...
        }
        return null;
    }

    private short parseUserId(String jsonResponse) {
        try {
            JSONArray jsonArray = new JSONArray(jsonResponse);
            if (jsonArray.length() > 0) {
                JSONObject userData = jsonArray.getJSONObject(0);
                return (short) userData.getInt("idUser");
            }
        } catch (JSONException e) {
            e.printStackTrace();
            // Handle JSON parsing error
            // ...
        }
        return 0;
    }
}
