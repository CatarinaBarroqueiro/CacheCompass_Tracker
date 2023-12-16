#include <message.h>

void print_packet_in_hex(uint8_t* buffer, uint8_t size) {

    for (int i = 0; i < size; i++) {
        if (buffer[i] < 10)
            Serial.print(F("0"));
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
}

/*
    ##########################################################################
    ############            LoRa Message declaration              ############
    ##########################################################################
*/

LoraMessage::LoraMessage() : lastMessagePtr(nullptr) {}

LoraMessage::~LoraMessage() {
    free_message(lastMessagePtr);
}

void LoraMessage::free_message(uint8_t* messagePtr) {
    if (messagePtr != nullptr) {
        free(messagePtr);
        if (lastMessagePtr == messagePtr) {
            lastMessagePtr = nullptr;
        }
    }
}

uint8_t* LoraMessage::save_message_memory(uint8_t* message, size_t size) {
    if (lastMessagePtr != nullptr) {
        free_message(lastMessagePtr);
    }
    lastMessagePtr = reinterpret_cast<uint8_t*>(malloc(size));
    if (lastMessagePtr != nullptr) {
        memcpy(lastMessagePtr, message, size);
        return lastMessagePtr;
    }
    return nullptr;  // Indicates failure to allocate memory
}

uint8_t* LoraMessage::hello(size_t& size, uint16_t nodeId, uint32_t packetId) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = HELLO;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &nodeId, MESSAGE_NODE_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE), &packetId,
           MESSAGE_PACKET_ID_SIZE);

    return save_message_memory(tempMessage, size);
}

uint8_t* LoraMessage::hello_response(size_t& size, uint16_t nodeId,
                                     uint32_t packetId) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = HELLO_RESPONSE;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &nodeId, MESSAGE_NODE_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE), &packetId,
           MESSAGE_PACKET_ID_SIZE);

    return save_message_memory(tempMessage, size);
}

uint8_t* LoraMessage::open_request(size_t& size, uint16_t nodeId,
                                   uint32_t packetId, uint16_t userId,
                                   unsigned long timestamp) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE +
           MESSAGE_USER_ID_SIZE + MESSAGE_TIMESTAMP_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = OPENING_REQUEST;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &nodeId, MESSAGE_NODE_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE), &packetId,
           MESSAGE_PACKET_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE +
                          MESSAGE_PACKET_ID_SIZE),
           &userId, MESSAGE_USER_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE +
                          MESSAGE_PACKET_ID_SIZE + MESSAGE_USER_ID_SIZE),
           &timestamp, MESSAGE_TIMESTAMP_SIZE);

    return save_message_memory(tempMessage, size);
}

uint8_t* LoraMessage::open_response(size_t& size, uint16_t nodeId,
                                    uint32_t packetId, bool authorized) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE +
           MESSAGE_AUTHORIZED_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = OPENING_RESPONSE;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &nodeId, MESSAGE_NODE_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE), &packetId,
           MESSAGE_PACKET_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE +
                          MESSAGE_PACKET_ID_SIZE),
           &authorized, MESSAGE_AUTHORIZED_SIZE);

    return save_message_memory(tempMessage, size);
}

uint8_t* LoraMessage::open_response_ack(size_t& size, uint16_t nodeId,
                                        uint32_t packetId) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = OPENING_RESPONSE_ACK;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &nodeId, MESSAGE_NODE_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE), &packetId,
           MESSAGE_PACKET_ID_SIZE);

    return save_message_memory(tempMessage, size);
}

MESSAGE_TYPE LoraMessage::get_type(uint8_t* message, size_t messageSize) {
    if (messageSize < MESSAGE_TYPE_SIZE)
        return INVALID;

    uint8_t type;
    memcpy(&type, message, MESSAGE_TYPE_SIZE);

    if (type < HELLO || type > OPENING_RESPONSE_ACK)
        return INVALID;

    return static_cast<MESSAGE_TYPE>(type);
}

uint16_t LoraMessage::get_node_id(uint8_t* message, size_t messageSize) {
    int headerBefore = MESSAGE_TYPE_SIZE;
    if (messageSize < headerBefore + MESSAGE_NODE_ID_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    uint16_t nodeId;
    memcpy(&nodeId, message + headerBefore, MESSAGE_NODE_ID_SIZE);

    return nodeId;
}

uint32_t LoraMessage::get_packet_id(uint8_t* message, size_t messageSize) {
    int headerBefore = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE;
    if (messageSize < headerBefore + MESSAGE_PACKET_ID_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    uint32_t packetId;
    memcpy(&packetId, message + headerBefore, MESSAGE_PACKET_ID_SIZE);

    return packetId;
}

uint16_t LoraMessage::get_user_id(uint8_t* message, size_t messageSize) {
    int headerBefore =
        MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE;
    if (messageSize < headerBefore + MESSAGE_USER_ID_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    uint16_t userId;
    memcpy(&userId, message + headerBefore, MESSAGE_USER_ID_SIZE);

    return userId;
}

unsigned long LoraMessage::get_timestamp(uint8_t* message, size_t messageSize) {
    int headerBefore = MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE +
                       MESSAGE_PACKET_ID_SIZE + MESSAGE_USER_ID_SIZE;
    if (messageSize < headerBefore + MESSAGE_TIMESTAMP_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    unsigned long timestamp;
    memcpy(&timestamp, message + headerBefore, MESSAGE_TIMESTAMP_SIZE);

    return timestamp;
}

bool LoraMessage::get_authorized(uint8_t* message, size_t messageSize) {
    int headerBefore =
        MESSAGE_TYPE_SIZE + MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID_SIZE;
    if (messageSize < headerBefore + MESSAGE_AUTHORIZED_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    bool authorized;
    memcpy(&authorized, message + headerBefore, MESSAGE_AUTHORIZED_SIZE);

    return authorized;
}

/*
    ##########################################################################
    ############             BLE Message declaration              ############
    ##########################################################################
*/

BleMessage::BleMessage() : lastMessagePtr(nullptr) {}
BleMessage::~BleMessage() {
    free_message(lastMessagePtr);
}

void BleMessage::free_message(uint8_t* messagePtr) {
    if (messagePtr != nullptr) {
        free(messagePtr);
        if (lastMessagePtr == messagePtr) {
            lastMessagePtr = nullptr;
        }
    }
}

uint8_t* BleMessage::save_message_memory(uint8_t* message, size_t size) {
    if (lastMessagePtr != nullptr) {
        free_message(lastMessagePtr);
    }
    lastMessagePtr = reinterpret_cast<uint8_t*>(malloc(size));
    if (lastMessagePtr != nullptr) {
        memcpy(lastMessagePtr, message, size);
        return lastMessagePtr;
    }
    return nullptr;  // Indicates failure to allocate memory
}

uint8_t* BleMessage::open_request(size_t& size, uint32_t packetId,
                                  uint16_t userId) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE + MESSAGE_USER_ID_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = OPENING_REQUEST;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &packetId, MESSAGE_PACKET_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE), &userId,
           MESSAGE_USER_ID_SIZE);

    return save_message_memory(tempMessage, size);
}

uint8_t* BleMessage::open_response(size_t& size, uint32_t packetId,
                                   bool authorized) {
    size = MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE + MESSAGE_AUTHORIZED_SIZE;
    uint8_t tempMessage[MAX_MESSAGE_SIZE];

    // Place the header in the new message
    MESSAGE_TYPE msgType = OPENING_RESPONSE;
    memcpy(tempMessage, &msgType, MESSAGE_TYPE_SIZE);
    memcpy(tempMessage + MESSAGE_TYPE_SIZE, &packetId, MESSAGE_PACKET_ID_SIZE);
    memcpy(tempMessage + (MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE),
           &authorized, MESSAGE_AUTHORIZED_SIZE);

    return save_message_memory(tempMessage, size);
}

MESSAGE_TYPE BleMessage::get_type(uint8_t* message, size_t messageSize) {
    if (messageSize < MESSAGE_TYPE_SIZE)
        return INVALID;

    uint8_t type;
    memcpy(&type, message, MESSAGE_TYPE_SIZE);

    if (type < HELLO || type > OPENING_RESPONSE_ACK)
        return INVALID;

    return static_cast<MESSAGE_TYPE>(type);
}

uint32_t BleMessage::get_packet_id(uint8_t* message, size_t messageSize) {
    int headerBefore = MESSAGE_TYPE_SIZE;
    if (messageSize < headerBefore + MESSAGE_PACKET_ID_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    uint32_t packetId;
    memcpy(&packetId, message + headerBefore, MESSAGE_PACKET_ID_SIZE);

    return packetId;
}

uint16_t BleMessage::get_user_id(uint8_t* message, size_t messageSize) {
    int headerBefore = MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE;
    if (messageSize < headerBefore + MESSAGE_USER_ID_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    uint16_t userId;
    memcpy(&userId, message + headerBefore, MESSAGE_USER_ID_SIZE);

    return userId;
}

bool BleMessage::get_authorized(uint8_t* message, size_t messageSize) {
    int headerBefore = MESSAGE_TYPE_SIZE + MESSAGE_PACKET_ID_SIZE;
    if (messageSize < headerBefore + MESSAGE_AUTHORIZED_SIZE) {
        Serial.println("Error: Unable to get attribute from message");
        return 0;
    }

    bool authorized;
    memcpy(&authorized, message + headerBefore, MESSAGE_AUTHORIZED_SIZE);

    return authorized;
}