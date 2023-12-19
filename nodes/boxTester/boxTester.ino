// Attention, always connect servo power-line to 5v, not 3v3
// ESP32Servo by Kevin Harrington, John K. Bennett
// https://arduino.stackexchange.com/questions/1321/servo-wont-stop-rotating
#include <ESP32Servo.h>

#define PIN_SERVO 13
#define MIN_SERVO_US 1000 // 500 default
#define MAX_SERVO_US 2000 // 2400 default
#define PIN_SWITCH 14

Servo servo;

bool boxClosed = true;

void open(){
  Serial.print("Opening box... ");

  servo.write(160);  // max speed counter-clockwise
  delay(220);
  servo.write(90);  // no motion

  Serial.println("Done!");
}

void close(){
  Serial.print("Closing box... ");

  servo.write(30);  // max speed clockwise
  delay(200);
  servo.write(90);  // no motion

  Serial.println("Done!");
}

void setup() {

  Serial.begin(115200);
  while (!Serial);
  Serial.println("Box Terminal ready");
  Serial.println();

  // Setup servo
  servo.attach(PIN_SERVO, MIN_SERVO_US, MAX_SERVO_US);

  // Setup Switch
  pinMode(PIN_SWITCH, INPUT);

}

void loop() {
  int switchValue = digitalRead(PIN_SWITCH);
  // switchValue == HIGH -> on
  // switchValue == LOW -> off

  Serial.printf("Switch is %d and box is %d", switchValue, boxClosed);
  Serial.println();

  if(switchValue == HIGH && boxClosed){
    open();
    boxClosed = false;
  }else if(switchValue == LOW && !boxClosed){
    close();
    boxClosed = true;
  }

  delay(100);
} 
