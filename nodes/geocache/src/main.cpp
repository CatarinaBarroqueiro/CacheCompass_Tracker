#include <Arduino.h>
#include <bleServer.h>
#include <lora.h>
#include <message.h>
#include <wifiClient.h>

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
LoraMessage loraMsgClass;

BleServer bleServer(NODE_ID);
TaskHandle_t bleTask;
BleMessage bleMsgClass;

WifiClient wifiClient(SSID_AD_HOC, PASSWORD_AD_HOC);
/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
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

void WifiClient::on_receive(void* arg, AsyncClient* client, void* data,
                            size_t len) {
    char* receivedData = (char*)data;
    Serial.print("Received from Box Opener: ");
    Serial.println(receivedData);
    Serial.println(" ");
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
    Serial.println();

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
    Serial.print("BLE, user device sent: ");
    Serial.println(bleMsgClass.to_string(data, len));

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

        Serial.print("LoRa868, Sending message to broker: ");
        Serial.println(loraMsgClass.to_string(msgToSend, sendMsgSize));

        // Cleanup allocated message
        loraMsgClass.free_message(msgToSend);

        uint8_t buffer[LORA_PAYLOAD];
        uint8_t recSize = lora.receive(buffer);

        Serial.print("LoRa868, Received message from broker: ");
        Serial.println(loraMsgClass.to_string(buffer, recSize));
        Serial.println();

        return loraMsgClass.get_authorized(buffer, recSize);
    } else {
        Serial.println("LoRa868, GeoCache not connected to Broker");
        return false;
    }

    Serial.println();
}

void open_box() {
    Serial.println("-> Sending Message to open box to Box Opener");
    Serial.println();
    wifiClient.ping();
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

    // Setup wifi with box opener
    wifiClient.connect();

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
