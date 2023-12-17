// Attention, always connect servo powerline to 5v, not 3v3
// ESP32Servo by Kevin Harrington, John K. Bennett
// https://arduino.stackexchange.com/questions/1321/servo-wont-stop-rotating

#include <Arduino.h>
#include <ESP32Servo.h>

Servo servo;

#define PIN_SERVO 13 // Use the GPIO pin connected to your servo

void setup() {
  Serial.begin(115200);
  
  servo.attach(PIN_SERVO);
  
}
void loop() {

  Serial.println("Ola");
  /*for (int pos = 0; pos <= 180; pos += 20) { // sweep from 0 degrees to 180 degrees
    Serial.println("pos: " + pos);
    servo.write(pos);
    delay(500); // Adjust delay as needed for your servo's speed
  }*/

  servo.write(0); // max speed clockwise

  delay(100);

  servo.write(90); // no motion

  delay(2000);

  servo.write(180); // max speed counter-clockwise

  delay(100);

  servo.write(90); // no motion

  delay(5000);
}