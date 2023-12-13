#pragma once

#include <Arduino.h>

/*
    ##########################################################################
    ############                  Definitions                     ############
    ##########################################################################
*/
//! Maximum size of the message in bytes
#define MAX_MESSAGE_SIZE 255
//! Size of the message type field in bytes
#define MESSAGE_TYPE_SIZE 2
//! Size of the node ID field in bytes
#define MESSAGE_NODE_ID_SIZE 2
//! Size of the packet ID field in bytes
#define MESSAGE_PACKET_ID_SIZE 4
//! Size of the timestamp field in bytes
#define MESSAGE_TIMESTAMP_SIZE 8
//! Size of the user ID field in bytes
#define MESSAGE_USER_ID_SIZE 4
//! Size of the authorized field in bytes
#define MESSAGE_AUTHORIZED_SIZE 1

/*
    ##########################################################################
    ############                 Message Types                    ############
    ##########################################################################
*/
//! Enum defining different message types
enum MESSAGE_TYPE {
    HELLO,                 //!< HELLO message type
    HELLO_RESPONSE,        //!< HELLO_RESPONSE message type
    OPENING_REQUEST,       //!< OPENING_REQUEST message type
    OPENING_RESPONSE,      //!< OPENING_RESPONSE message type
    OPENING_RESPONSE_ACK,  //!< OPENING_RESPONSE_ACK message type
    INVALID                //!< INVALID message type
};

/*
    ##########################################################################
    ############              Message declaration                 ############
    ##########################################################################
*/
//! Class for managing message creation and extraction
class Message {
   private:
    /*!
    Allocate memory and save the message in the HEAP, don't forget to `free it`!
      \param message Pointer to the message
      \param size Size of the message
      \return Pointer to the allocated memory containing the saved message
    */
    uint8_t* save_message_memory(uint8_t* message, size_t size);

    //! ptr that saves the last allocated message and frees it upon class destruction
    uint8_t* lastMessagePtr;

   public:
    Message();
    ~Message();

    /*!
    Free the memory associated with the given message pointer
      \param messagePtr Pointer to the message memory to be freed
      Frees the memory allocated for a message, if it was previously allocated.
      Updates the lastMessagePtr if it matches the message pointer being freed.
    */
    void free_message(uint8_t* messagePtr);

    /*!
    
    /*!
    Create a message with specified parameters in the HEAP, don't forget to `free` it!
      \param size [out] Size of the message created
      \param type Type of message to create
      \param nodeId Node ID
      \param packetId Packet ID
      \param userId User ID
      \param timestamp Timestamp associated with the message
      \return Pointer to the created message
    */
    uint8_t* create_message(size_t& size, MESSAGE_TYPE type,
                                     uint16_t nodeId, uint32_t packetId,
                                     uint16_t userId, unsigned long timestamp);

    /*!
    Create a HELLO message in the HEAP, don't forget to `free it`!
      \param size Size of the message
      \param nodeId Node ID
      \param packetId Packet ID
      \return Pointer to the created message
    */
    uint8_t* hello(size_t& size, uint16_t nodeId, uint32_t packetId);

    /*!
    Create a HELLO_RESPONSE message in the HEAP, don't forget to `free it`!
      \param size Size of the message
      \param nodeId Node ID
      \param packetId Packet ID
      \return Pointer to the created message
    */
    uint8_t* hello_response(size_t& size, uint16_t nodeId, uint32_t packetId);

    /*!
    Create an OPENING_REQUEST message in the HEAP, don't forget to `free it`!
      \param size Size of the message
      \param nodeId Node ID
      \param packetId Packet ID
      \param userId User ID
      \param timestamp Timestamp
      \return Pointer to the created message
    */
    uint8_t* open_request(size_t& size, uint16_t nodeId, uint32_t packetId,
                          uint16_t userId, unsigned long timestamp);

    /*!
    Create an OPENING_RESPONSE message in the HEAP, don't forget to `free it`!
      \param size Size of the message
      \param nodeId Node ID
      \param packetId Packet ID
      \param authorized Authorization status
      \return Pointer to the created message
    */
    uint8_t* open_response(size_t& size, uint16_t nodeId, uint32_t packetId,
                           bool authorized);

    /*!
    Create an OPENING_RESPONSE_ACK message in the HEAP, don't forget to `free it`!
      \param size Size of the message
      \param nodeId Node ID
      \param packetId Packet ID
      \return Pointer to the created message
    */
    uint8_t* open_response_ack(size_t& size, uint16_t nodeId,
                               uint32_t packetId);

    /*!
    Get the type of the message from the given message
      \param message Pointer to the message
      \param messageSize Size of the message
      \return Type of the message (MESSAGE_TYPE)
    */
    static MESSAGE_TYPE get_type(uint8_t* message, size_t messageSize);

    /*!
    Get the node ID from the given message
      \param message Pointer to the message
      \param messageSize Size of the message
      \return Node ID extracted from the message
    */
    static uint16_t get_node_id(uint8_t* message, size_t messageSize);

    /*!
    Get the packet ID from the given message
      \param message Pointer to the message
      \param messageSize Size of the message
      \return Packet ID extracted from the message
    */
    static uint32_t get_packet_id(uint8_t* message, size_t messageSize);

    /*!
    Get the user ID from the given message
      \param message Pointer to the message
      \param messageSize Size of the message
      \return User ID extracted from the message
    */
    static uint16_t get_user_id(uint8_t* message, size_t messageSize);

    /*!
    Get the timestamp from the given message
      \param message Pointer to the message
      \param messageSize Size of the message
      \return Timestamp extracted from the message
    */
    static unsigned long get_timestamp(uint8_t* message, size_t messageSize);

    /*!
    Get the authorization status from the given message
      \param message Pointer to the message
      \param messageSize Size of the message
      \return Authorization status extracted from the message
    */
    static bool get_authorized(uint8_t* message, size_t messageSize);
};
