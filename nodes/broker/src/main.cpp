#include <Arduino.h>
#include <lora.h>
#include <message.h>
#include <webAPI.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define NODE_ID 4

/*
    ##########################################################################
    ############                 Global Variables                 ############
    ##########################################################################
*/
LoRa868 lora(NODE_ID);
TaskHandle_t loraTask;
LoraMessage msgClass;

WebAPI webAPI;
/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
void process_lora_message(uint8_t* message, uint8_t size);
void receive_lora(void* parameter);
void setup();
void loop();

/*
    ##########################################################################
    ############                      Code                        ############
    ##########################################################################
*/

void process_lora_message(uint8_t* message, uint8_t size) {
    // Received message general info
    MESSAGE_TYPE type = msgClass.get_type(message, size);
    uint16_t nodeId = msgClass.get_node_id(message, size);
    uint32_t packetId = msgClass.get_packet_id(message, size);

    // Message to send variables
    size_t sendMsgSize;
    uint8_t* msgToSend;

    Serial.print("LoRa868, Received message from GeoCache: ");
    Serial.println(msgClass.to_string(message, size));

    if (type == HELLO) {
        msgToSend = msgClass.hello_response(sendMsgSize, nodeId, packetId);

    } else if (type == OPENING_REQUEST) {
        uint16_t userId = msgClass.get_user_id(message, size);
        unsigned long timestamp = msgClass.get_timestamp(message, size);

        // call remote service
        Serial.println("Http, Starting requests for user authorization");
        bool authorized = webAPI.request_user_authorized(userId);
        webAPI.post_discovery(nodeId, userId, timestamp, authorized);
        Serial.println("Http, all requests completed");
        Serial.println();

        msgToSend =
            msgClass.open_response(sendMsgSize, nodeId, packetId, authorized);
    } else if (type == OPENING_RESPONSE_ACK) {
        return;
    } else if (type == HELLO_RESPONSE || type == OPENING_RESPONSE) {
        ;  // do nothing
        return;
    } else {
        Serial.println("Error: Unknown message type");
        return;
    }

    /*Serial.println(" - Message to send: ");
    print_packet_in_hex(msgToSend, sendMsgSize);
    Serial.println();*/

    // Send response to GeoCache
    lora.send(msgToSend, sendMsgSize);

    Serial.printf("LoRa868, Sending message to GeoCache: ");
    Serial.println(msgClass.to_string(msgToSend, sendMsgSize));

    // free the created message
    msgClass.free_message(msgToSend);
}

void receive_lora(void* parameter) {
    Serial.print("LoRa868, Receive task running on core ");
    Serial.println(xPortGetCoreID());
    for (;;) {
        //Serial.println("LoRa868, Waiting for data...");
        uint8_t buffer[LORA_PAYLOAD];
        uint8_t recSize = lora.receive(buffer);
        if (recSize > 0) {
            process_lora_message(buffer, recSize);
            Serial.println();
        }
        delay(50);
    }
    Serial.println("LoRa868, Receive task ended");
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Broker Terminal ready");
    Serial.println();

    // Setup webAPI
    webAPI.connect_wifi();

    // Setup LoRa868
    while (!lora.configure(VERBOSE))
        delay(3000);

    // Create LoRa Task to receive data
    //xTaskCreate(receive_lora, "receive_lora", 8000, NULL, 1, &loraTask);

    Serial.println();
}

void loop() {
    uint8_t buffer[LORA_PAYLOAD];
    uint8_t recSize = lora.receive(buffer);
    if (recSize > 0) {
        process_lora_message(buffer, recSize);
        Serial.println();
    }
    delay(200);
}