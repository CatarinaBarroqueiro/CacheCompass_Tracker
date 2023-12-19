#include <Arduino.h>
//#include <task.h>
#include <ESP32Servo.h>
#include <wifiServer.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define PIN_SERVO 13
#define MIN_SERVO_US 1000  // 500 default
#define MAX_SERVO_US 2000  // 2400 default
#define PIN_SWITCH 14

/*
    ##########################################################################
    ############                 Global Variables                 ############
    ##########################################################################
*/
WifiServer wifiServer(SSID_AD_HOC, PASSWORD_AD_HOC);
Servo servo;
// Attention, always connect servo power-line to 5v, not 3v3
// ESP32Servo by Kevin Harrington, John K. Bennett
// https://arduino.stackexchange.com/questions/1321/servo-wont-stop-rotating

bool boxClosed = true;
bool wifiLock = false;
int counter = 0;
/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
void open_box();
void close_box();
void setup();
void loop();

/*
    ##########################################################################
    ############                      Code                        ############
    ##########################################################################
*/

void open_box() {
    Serial.print("Opening box... ");

    servo.write(160);  // max speed clockwise
    vTaskDelay(pdMS_TO_TICKS(120));
    servo.write(90);  // no motion

    Serial.println("Done!");
}

void close_box() {
    Serial.print("Closing box... ");

    servo.write(30);  // max speed counter-clockwise
    vTaskDelay(pdMS_TO_TICKS(100));
    servo.write(90);  // no motion

    Serial.println("Done!");
}

void WifiServer::on_client_connected(void* arg, AsyncClient* client) {
    Serial.println("Client established connection with server");
    client->onData(
        [](void* arg, AsyncClient* client, void* data, size_t len) {
            uint8_t* receivedData = reinterpret_cast<uint8_t*>((char*)(data));
            //printHEXPacket(receivedData, 33); -- debugging
            Serial.printf("Receiving %d bytes from geocache", len);
            Serial.println();

            wifiLock = true;
            counter = 0;
            open_box();

            Serial.println();
        },
        nullptr);
}

void setup() {
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Server Terminal ready");
    Serial.println();

    // Setup servo
    servo.attach(PIN_SERVO);

    // Setup wifi receiver
    wifiServer.setup();

    // Setup Switch
    pinMode(PIN_SWITCH, INPUT);
}

void loop() {

    if (!wifiLock) {
        int switchValue = digitalRead(PIN_SWITCH);
        // switchValue == HIGH -> on
        // switchValue == LOW -> off

        Serial.printf("Switch is %d and box is %d", switchValue, boxClosed);
        Serial.println();

        if (switchValue == HIGH && boxClosed) {
            open_box();
            boxClosed = false;
        } else if (switchValue == LOW && !boxClosed) {
            //close_box();
            boxClosed = true;
        }
    } else {

        if (counter >= 50) {
            close_box();
            boxClosed = true;
            wifiLock = false;
            counter = 0;
        } else {
            Serial.printf("Counter is %d", counter);
            Serial.println();
        }
        counter++;
    }

    vTaskDelay(pdMS_TO_TICKS(100));
}
