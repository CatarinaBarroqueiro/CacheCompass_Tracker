#include <lora.h>

LoRa868::LoRa868(uint16_t nodeId, bool applyDutyCycle) {
    this->dutyCycleEnabled = applyDutyCycle;
    this->nodeId = nodeId;

    pinMode(2, INPUT_PULLUP);
    digitalWrite(NSS, HIGH);
    pinMode(NSS, OUTPUT);
};

LoRa868::~LoRa868() {
    terminate();
};

void LoRa868::packet_is_error(int16_t packetRSSI, int8_t packetSNR) {
    // read the LoRa device IRQ status register
    uint16_t IRQStatus;
    IRQStatus = LoRa.readIrqStatus();

    // check if the error occurred by an RX timeout
    if (IRQStatus & IRQ_RX_TIMEOUT) {
        // No error occurred - timeout
        //Serial.println(" Received Timeout on receive packet");
        ;
    } else {
        // Error occurred in receiving packet
        RxPacketCount++;
        Serial.print(F(" PacketError"));
        Serial.print(F(",RSSI,"));
        Serial.print(packetRSSI);
        Serial.print(F("dBm,SNR,"));
        Serial.print(packetSNR);
        Serial.print(F("dB,Length,"));
        Serial.print(LoRa.readRXPacketL());  //get the real packet length
        Serial.print(F(",Packets,"));
        Serial.print(RxPacketCount);
        Serial.print(F(",Errors,"));
        Serial.print(F(",IRQreg,"));
        Serial.print(IRQStatus, HEX);
        LoRa.printIrqStatus();  //print the names of the IRQ registers set
        Serial.println();
    }
}

bool LoRa868::configure(DEBUG_MODE mode) {
    if (configDone) {
        Serial.println("Error: LoRa868, configuration already performed");
        return false;
    }
    if (mode == VERBOSE)
        Serial.println("LoRa868, initializing system...");

    /* 
    terminated variable is set to true, when the LoRa module is terminated
        or sent to sleep, so we need to signal the SPI pins to wake up the 
        device 
    */
    if (terminated) {
        // Re-attach LoRa-related GPIO pins after sleep
        rtc_gpio_hold_dis(GPIO_NUM_4);
        rtc_gpio_hold_dis(GPIO_NUM_12);
    }

    // Start SPI interface to communicate with Sx127 module
    SPI.begin(SCK, MISO, MOSI, NSS);

    // Initialize LoRa module
    if (LoRa.begin(NSS, NRESET, LORA_DEVICE)) {
        if (mode == VERBOSE)
            Serial.println(" - LoRa device found");
    } else {
        Serial.println(" - Error: LoRa Device error");
        return false;
    }

    // Configure LoRa with the defined parameters
    LoRa.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate,
                   Optimisation);
    if (mode == VERBOSE)
        Serial.println("LoRa868 initialization complete");

    configDone = true;

    return true;
}

bool LoRa868::connected() {
    // Create and send a Hello message to the Broker
    LoraMessage msgClass;
    size_t size;
    uint8_t* msgToSend = msgClass.hello(size, nodeId, TxPacketCount);
    send(msgToSend, size);

    // free the created message
    msgClass.free_message(msgToSend);

    // Wait for a response from the Broker
    uint8_t response[LORA_PAYLOAD];
    uint8_t received = receive(response);

    Serial.println();
    // Check if the response is a Hello response
    if (received > 0 && msgClass.get_type(response, received) == HELLO_RESPONSE)
        return true;
    else
        return false;
}

bool LoRa868::send(uint8_t* buffer, size_t size) {
    if (!configDone) {
        Serial.println("Error: LoRa868 Classic initialization not performed");
        return false;
    }

    // Check if duty cycle is enabled and if it's still complying with timing constraints
    unsigned long elapsedTime = millis() - lastTransmissionTime;
    if ((elapsedTime < dutyCycleTime) && dutyCycleEnabled) {
        return false;
    }

    /*Serial.printf(
        "LoRa868, Sending packet no. %d with %d bytes and %d dbm power... ",
        TxPacketCount, size, TxPower);*/

    // Tick time before calling function to send
    unsigned long timeBeforeSending = millis();
    // Call Blocking function to send the packet to the receiver, which returns
    //  the packet length if OK, otherwise 0 if error occurred
    uint8_t bytesSent =
        LoRa.transmitIRQ(buffer, size, TxTimeoutMs, TxPower, WAIT_TX);
    // Tick time after calling function to send
    lastTransmissionTime = millis();

    // Calculate duty cycle, according to the time it took to send the packet
    dutyCycleTime =
        (lastTransmissionTime - timeBeforeSending) * 100;  // time in seconds

    if (bytesSent) {
        // Packet sent successfully
        //Serial.println("Packet Sent ");
        TxPacketCount++;
    } else {
        // Error in packet sending
        Serial.println();
        Serial.println("Error: Problem sending packet, diagnostics: ");
        uint16_t IRQStatus;
        IRQStatus = LoRa.readIrqStatus();
        Serial.print(" - IRQreg: ");
        Serial.println(IRQStatus, HEX);
        Serial.print(" - TXpower: ");
        Serial.println(TxPower);
        Serial.print(" - IrqStatus: ");
        LoRa.printIrqStatus();
        Serial.println();
        return false;
    }
    return true;
}

bool LoRa868::terminate() {
    if (!configDone) {
        Serial.println(
            "Error: LoRa868, initialization needs to be performed before "
            "terminating module");
        return false;
    }

    // Ensure LoRa module is in sleep mode (if supported)
    LoRa.setSleep(CONFIGURATION_RETENTION);

    // Release LoRa-related GPIO pins
    rtc_gpio_hold_en(GPIO_NUM_4);
    //hold LoRa device off in sleep
    rtc_gpio_hold_en(GPIO_NUM_12);

    // Reset SPI to release the LoRa-related pins
    SPI.end();

    terminated = true;
    configDone = false;

    return true;
}

uint8_t LoRa868::receive(uint8_t* buffer) {
    // call blocking function, which will listen for a packet within the configured timeout period
    uint8_t packetSize =
        LoRa.receiveIRQ(buffer, LORA_PAYLOAD, RxTimeoutMs, WAIT_RX);

    // Read the received packets RSSI value
    int16_t packetRSSI = LoRa.readPacketRSSI();

    // Read the received packets SNR value
    int8_t packetSNR = LoRa.readPacketSNR();

    // if and error occurred the packetSize is equal to 0
    if (packetSize == 0) {
        packet_is_error(packetRSSI, packetSNR);
    } else {
        // packet received successfully
        /*Serial.printf("LoRa868, Received packet no. %d with %d bytes",
                      RxPacketCount, packetSize);
        Serial.println();*/
        RxPacketCount++;
    }

    return packetSize;
}

bool LoRa868::get_configDone() {
    return configDone;
}

int LoRa868::get_tx_packet_count() {
    return TxPacketCount;
}