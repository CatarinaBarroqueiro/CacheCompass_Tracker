#include <message.h>

Message::Message(uint16_t nodeId) {
    this->nodeId = nodeId;
}

Message::~Message() {}

uint8_t* Message::hello(uint32_t& size){
    size = MESSAGE_NODE_ID_SIZE + MESSAGE_PACKET_ID;
    uint8_t* message = (uint8_t*) malloc(size);
    
    // Place the header in the new message
    memcpy(message, &messageType, sizeof(messageType));
    memcpy(message + sizeof(messageType), &photoCount, sizeof(photoCount));
    memcpy(newMessage + sizeof(messageType) + sizeof(photoCount), &bytesIndex, sizeof(bytesIndex));

    // Place the payload data
    memcpy(newMessage + headerSize, message, messageSize);

    return newMessage;
}

uint8_t* Message::hello_response(uint32_t& size){

}

uint8_t* Message::open_request(uint16_t userId, long timestamp, uint32_t& size){

}

uint8_t* Message::open_response(uint32_t& size){

}

uint8_t* Message::open_response_ack(uint32_t& size){

}

uint8_t Message::get_type(uint8_t* message, size_t messageSize){

}
