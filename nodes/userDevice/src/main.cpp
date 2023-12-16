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
}

void loop() {

    delay(10000);

    // Message to send variables
    size_t sendMsgSize;
    uint8_t* msgToSend = bleMsgClass.open_request(
        sendMsgSize, bleClient.get_packet_count(), USER_ID);

    bleClient.send(msgToSend, sendMsgSize);

    Serial.printf("Sending message to Geocache: %s",
                  bleMsgClass.to_string(msgToSend, sendMsgSize));
    Serial.println();
    Serial.println();

    // Cleanup allocated message
    bleMsgClass.free_message(msgToSend);
}