#include <Arduino.h>
#include <ESP32Servo.h>
#include <wifiServer.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define PIN_SERVO 13

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

    servo.write(0);  // max speed clockwise
    delay(100);
    servo.write(90);  // no motion

    Serial.println("Done!");
}

void close_box() {
    Serial.print("Closing box... ");

    servo.write(180);  // max speed counter-clockwise
    delay(100);
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

            open_box();
            delay(15000);
            close_box();

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
}

void loop() {
    delay(1000);
}
