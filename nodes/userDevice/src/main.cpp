#include <Arduino.h>
#include <bleClient.h>
#include <message.h>
/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define USER_ID 1

/*
    ##########################################################################
    ############                 Global Variables                 ############
    ##########################################################################
*/
BleClient bleClient;
BleMessage bleMsgClass;

/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
void setup();
void loop();

/*
    ##########################################################################
    ############                      Code                        ############
    ##########################################################################
*/
void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Device Terminal ready");
    Serial.println();

    bleClient.setup();

    Serial.println();
}

void loop() {

    delay(10000);

    // Message to send variables
    size_t sendMsgSize;
    uint8_t* msgToSend = bleMsgClass.open_request(
        sendMsgSize, bleClient.get_packet_count(), USER_ID);

    bleClient.send(msgToSend, sendMsgSize);

    Serial.print("BLE, Sending message to Geocache: ");
    Serial.println(bleMsgClass.to_string(msgToSend, sendMsgSize));
    Serial.println();

    // Cleanup allocated message
    bleMsgClass.free_message(msgToSend);
}