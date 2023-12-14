#include <Arduino.h>
#include <bleServer.h>
#include <lora.h>
#include <message.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define NODE_ID 1
#define USER_ID 1

/*
    ##########################################################################
    ############                 Global Variables                 ############
    ##########################################################################
*/
LoRa868 lora(NODE_ID);
TaskHandle_t loraTask;
Message msgClass;
BleServer bleServer(NODE_ID);

/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
String get_time_string(unsigned long timestamp);
void receive_lora(void* parameter);
void setup();
void loop();

/*
    ##########################################################################
    ############                      Code                        ############
    ##########################################################################
*/
String get_time_string(unsigned long timestamp) {
    unsigned long seconds = timestamp % 60;
    unsigned long minutes = (timestamp / 60) % 60;
    unsigned long hours = (timestamp / 3600) % 24;

    // Create a string in HH:MM:SS format
    String timeString = "";
    timeString += (hours < 10 ? "0" : "");
    timeString += hours;
    timeString += ":";
    timeString += (minutes < 10 ? "0" : "");
    timeString += minutes;
    timeString += ":";
    timeString += (seconds < 10 ? "0" : "");
    timeString += seconds;

    return timeString;
}

void receive_lora(void* parameter) {
    Serial.print("LoRa868, Receive task running on core ");
    Serial.println(xPortGetCoreID());
    for (;;) {
        uint8_t buffer[LORA_PAYLOAD];
        uint8_t recSize = lora.receive(buffer);
        if (recSize > 0) {
            Serial.printf("LoRa868, Heard someone talking %d bytes", recSize);
            Serial.println();
        }
        delay(200);
    }
    Serial.println("LoRa868, Receive task ended");
}

bool fetch_authorized_open() {
    if (lora.connected()) {
        // Message to send variables
        size_t sendMsgSize;
        uint8_t* msgToSend = msgClass.open_request(sendMsgSize, NODE_ID,
                                                   lora.get_tx_packet_count(),
                                                   USER_ID, millis());
        // Force wait for duty cycle completion
        while (!lora.send(msgToSend, sendMsgSize))
            delay(200);

        /*Serial.println("Sending message from broker: ");
        print_packet_in_hex(msgToSend, sendMsgSize);
        Serial.println();*/

        // Cleanup allocated message
        msgClass.free_message(msgToSend);

        uint8_t buffer[LORA_PAYLOAD];
        uint8_t recSize = lora.receive(buffer);

        if (msgClass.get_authorized(buffer, recSize))
            Serial.printf(" - Player ID %d, Authorized to open node %d",
                          msgClass.get_user_id(buffer, recSize),
                          msgClass.get_node_id(buffer, recSize));
        else
            Serial.printf(" - Player ID %d, Authorized to open node %d",
                          msgClass.get_user_id(buffer, recSize),
                          msgClass.get_node_id(buffer, recSize));

        return msgClass.get_authorized(buffer, recSize);
    } else {
        Serial.println("GeoCache not connected to LORA receiver");
        return false;
    }

    Serial.println();
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("GeoCache Terminal ready");
    Serial.println();

    bleServer.setup();

    // Setup LoRa868
    while (!lora.configure(VERBOSE))
        delay(3000);

    // Create LoRa Task to receive data
    //xTaskCreate(receive_lora, "receive_lora", 8000, NULL, 1, &loraTask);

    Serial.println();
}

void loop() {

    // When the user tries to connect using BLE
    bool authorized = false;
    if (true)
        authorized = fetch_authorized_open();

    delay(5000);
}
