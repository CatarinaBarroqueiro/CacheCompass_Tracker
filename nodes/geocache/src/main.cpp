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
LoraMessage loraMsgClass;

BleServer bleServer(NODE_ID);
TaskHandle_t bleTask;
BleMessage bleMsgClass;
/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
String get_time_string(unsigned long timestamp);
void receive_lora(void* parameter);
void receive_ble(void* parameter);
void process_ble_message(uint8_t* data, uint8_t len);
bool fetch_authorized_open(int userId);
void open_box();
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

void receive_ble(void* parameter) {
    Serial.print("BLE Task running on core ");
    Serial.println(xPortGetCoreID());
    for (;;) {
        uint8_t len = 0;
        uint8_t* data = bleServer.read(&len);
        if (data != nullptr) {
            //Serial.printf("Receiving %d bytes from client...", len);
            //Serial.println();
            process_ble_message(data, len - BLE_INTERNAL_HEADER);
        }
        delay(50);
    }
    Serial.println("BLE Task ended");
}

void process_ble_message(uint8_t* data, uint8_t len) {
    Serial.printf("BLE, user device sent: %s",
                  bleMsgClass.to_string(data, len));
    Serial.println();

    if (loraMsgClass.get_type(data, len) == OPENING_REQUEST) {
        int userId = bleMsgClass.get_user_id(data, len);
        bool authorized = fetch_authorized_open(userId);

        if (authorized)
            open_box();

        // work on sending a response back to the user
    }
}

bool fetch_authorized_open(int userId) {
    if (lora.connected()) {
        // Message to send variables
        size_t sendMsgSize;
        uint8_t* msgToSend = loraMsgClass.open_request(
            sendMsgSize, NODE_ID, lora.get_tx_packet_count(), userId, millis());
        // Force wait for duty cycle completion
        while (!lora.send(msgToSend, sendMsgSize))
            delay(200);

        /*Serial.println("Sending message from broker: ");
        print_packet_in_hex(msgToSend, sendMsgSize);
        Serial.println();*/

        Serial.printf("LoRa868, Sending message to broker: %s",
                      loraMsgClass.to_string(msgToSend, sendMsgSize));
        Serial.println();

        // Cleanup allocated message
        loraMsgClass.free_message(msgToSend);

        uint8_t buffer[LORA_PAYLOAD];
        uint8_t recSize = lora.receive(buffer);

        Serial.printf("LoRa868, Received message from broker: %s",
                      loraMsgClass.to_string(msgToSend, sendMsgSize));
        Serial.println();

        if (loraMsgClass.get_authorized(buffer, recSize))
            Serial.printf("> Player ID %d, Authorized to open node %d",
                          loraMsgClass.get_user_id(buffer, recSize),
                          loraMsgClass.get_node_id(buffer, recSize));
        else
            Serial.printf("> Player ID %d, Authorized to open node %d",
                          loraMsgClass.get_user_id(buffer, recSize),
                          loraMsgClass.get_node_id(buffer, recSize));

        return loraMsgClass.get_authorized(buffer, recSize);
    } else {
        Serial.println("LoRa868, GeoCache not connected to Broker");
        return false;
    }

    Serial.println();
}

void open_box() {
    Serial.println("->Opening box");
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("GeoCache Terminal ready");
    Serial.println();

    // Setup BLE Server
    bleServer.setup();

    // Setup LoRa868
    while (!lora.configure(VERBOSE))
        delay(3000);

    // Create LoRa Task to receive data
    //xTaskCreate(receive_lora, "receive_lora", 8000, NULL, 1, &loraTask);

    // Create BLE Task to receive data
    xTaskCreate(receive_ble, "receive_ble", 8000, NULL, 1, &bleTask);

    Serial.println();
}

void loop() {
    // used to not trigger the watchdog timer of the tasks
    delay(2000);
}
