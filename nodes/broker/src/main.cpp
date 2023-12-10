#include <Arduino.h>
#include <lora.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
#define NODE_ID 1

/*
    ##########################################################################
    ############                 Global Variables                 ############
    ##########################################################################
*/
LoRa868 lora(NODE_ID);

/*
    ##########################################################################
    ############                     Functions                    ############
    ##########################################################################
*/
void loop();
void setup();

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
    Serial.println("Broker Terminal ready");
    Serial.println();

    // Setup LoRa868
    while (!lora.configure(VERBOSE))
        delay(3000);

    // Create LoRa Task to receive data
    //xTaskCreate(receive_lora, "receive_lora", 8000, NULL, 1, &loraTask);

    Serial.println();
}

void loop() {}