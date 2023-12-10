#pragma once

#include <Arduino.h>

// in bytes
#define MAX_MESSAGE_SIZE 255
#define MESSAGE_TYPE_SIZE 2
#define MESSAGE_NODE_ID_SIZE 2
#define MESSAGE_PACKET_ID_SIZE 4
#define MESSAGE_TIMESTAMP_SIZE 8
#define MESSAGE_USER_ID_SIZE 4
#define MESSAGE_AUTHORIZED_SIZE 1

enum MESSAGE_TYPE {
    HELLO,
    HELLO_RESPONSE,
    OPENING_REQUEST,
    OPENING_RESPONSE,
    OPENING_RESPONSE_ACK,
    INVALID
};

class Message {
   private:
    uint8_t* save_message_memory(uint8_t* message, size_t size);

    uint8_t* lastMessagePtr;

   public:
    Message();
    ~Message();

    void free_message(uint8_t* messagePtr);
    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* hello(size_t& size, uint16_t nodeId, uint32_t packetId);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* hello_response(size_t& size, uint16_t nodeId, uint32_t packetId);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* open_request(size_t& size, uint16_t nodeId, uint32_t packetId,
                          uint16_t userId, long timestamp);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* open_response(size_t& size, uint16_t nodeId, uint32_t packetId,
                           bool authorized);

    //! Attention, this function returns a memory allocated pointer, please FREE it!
    uint8_t* open_response_ack(size_t& size, uint16_t nodeId,
                               uint32_t packetId);

    static MESSAGE_TYPE get_type(uint8_t* message, size_t messageSize);
};
