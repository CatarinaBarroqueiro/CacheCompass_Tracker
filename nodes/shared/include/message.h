#pragma once

#include <Arduino.h>

// in bytes
#define MESSAGE_TYPE_SIZE 2
#define MESSAGE_NODE_ID_SIZE 4
#define MESSAGE_PACKET_ID
#define MESSAGE_PAYLOAD_SIZE
#define MESSAGE_TIMESTAMP_SIZE
#define MAX_MESSAGE_SIZE 

enum MESSAGE_TYPE {HELLO, HELLO_RESPONSE, OPENING_REQUEST, OPENING_RESPONSE, OPENING_RESPONSE_ACK}

class Message {
   private:
    uint16_t nodeId;

   public:
    Message(uint16_t nodeId);
    ~Message();
    
    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* hello(uint32_t& size, uint32_t packetId);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* hello_response(uint32_t& size, uint32_t packetId);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* open_request(uint16_t userId, uint32_t packetId, long timestamp, uint32_t& size);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* open_response(uint32_t& size, uint32_t packetId);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* open_response_ack(uint32_t& size, uint32_t packetId);

    uint8_t get_type(uint8_t* message, size_t messageSize);
};
