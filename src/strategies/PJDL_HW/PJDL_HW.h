// --------------------------------------------------
// Document:  PJDL_HW.h
// Project:   PJON_ASIC/PJON_Library
// Function:  strategy for PJON-Library to connect to the PJDL_HW
// Autor:     Pius Sieber
// Date:      12.05.2023
// Comments:  Heavily based on SoftareBitBang strategy by Giovanni Blu Mitolo
// --------------------------------------------------

#pragma once

/* Set here the selected transmission mode - default STANDARD */
#ifndef SWBB_MODE
  #define SWBB_MODE         1
#endif

// Used to signal communication failure
#define SWBB_FAIL       65535
// Used for pin handling
#define SWBB_NOT_ASSIGNED 255

/* Transmission speed modes (see Timing.h)
  MODE   1: 1.97kB/s - 15808Bd
  MODE   2: 2.21kB/s - 17696Bd
  MODE   3: 3.10kB/s - 24844Bd
  MODE   4: 3.34kB/s - 26755Bd */

// ************ PJDL HW CONSTANTS ************

// PJDL HW Register Addresses
#define PJDL_HW_BASE_ADDR 0x20001000
#define PDJL_HW_OFFSET_ACTIVATE_DMA_RECEIVING 0
#define PJDL_HW_OFFSET_PREAMBLE 4
#define PJDL_HW_OFFSET_PAD 8
#define PJDL_HW_OFFSET_DATA 12
#define PJDL_HW_OFFSET_ACCEPTANCE 16
#define PJDL_HW_OFFSET_ENABLE_MODULE 20
#define PJDL_HW_OFFSET_AXIS 0x18
#define PJDL_HW_OFFSET_STATUS 0x1C
#define PJDL_HW_OFFSET_DMA_RECEIVED_DATA_INDICATOR 0x28

// PJDL HW Data bit definitions
#define PJDL_HW_LAST_BIT  0x00000100
#define PJDL_HW_ACK_BIT   0x00000200
#define PJDL_HW_ACK_REQUEST_BIT 0x00000400

#define PJDL_HW_DIRECT_SEND_DONE_MASK 0x01
#define PJDL_HW_DIRECT_DATA_READY_MASK 0x02
#define PJDL_HW_SENDING_IN_PROGRESS_MASK 0x04
#define PJDL_HW_RECEIVING_IN_PROGRESS_MASK 0x08

// *******************************************

#include "Timing.h"
#include "util_cpp.h"
#include "idma_OBIfrontend.h"

// Recommended receive time for this strategy, in microseconds
#ifndef SWBB_RECEIVE_TIME
  #define SWBB_RECEIVE_TIME 1000
#endif

class PJDL_HW {
  public:
    /* Returns the delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < SWBB_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };


    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      PJON_DELAY(PJON_RANDOM(SWBB_INITIAL_DELAY) + did);

      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_PREAMBLE) = PJDL_HW_PREAMBLE;
      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_PAD) = PJDL_HW_PAD;
      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_DATA) = PJDL_HW_DATA;
      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_ACCEPTANCE) = PJDL_HW_ACCEPTANCE;

      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_ENABLE_MODULE) = 1;

      *reg32(PJDL_HW_BASE_ADDR, PDJL_HW_OFFSET_ACTIVATE_DMA_RECEIVING) = 0x00000001; // activate dma-receiving

      return true;
    };


    /* Check if the channel is free for transmission:
       If reading 10 bits no 1 is detected there is no active transmission */

    bool can_start() {
      return ((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)
              & PJDL_HW_RECEIVING_IN_PROGRESS_MASK) == 0x00);
      // ToDo: add aditional delay?
    };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return SWBB_MAX_ATTEMPTS;
    };


    /* Returns the recommended receive time for this strategy: */

    static uint16_t get_receive_time() {
      return SWBB_RECEIVE_TIME;
    };


    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(SWBB_COLLISION_DELAY));
    };

    /* Receive byte response:
       Transmitter emits a SWBB_BIT_WIDTH / 4 long bit and tries
       to get a response cyclically for SWBB_RESPONSE_TIMEOUT microseconds.
       Receiver synchronizes to the falling edge of the last incoming
       bit and transmits PJON_ACK */

    uint16_t receive_response() {
      // ToDo: compare timeout to original to make sure it is correct
      // ToDo: solve problem: + 1 should be enough in the next line, but does not work (do not receive anz more after last pulse or bevor firsts!)
      uint8_t timeout_repetitions = (uint8_t)((_timeout / ((2*SWBB_BIT_SPACER) + (SWBB_BIT_WIDTH/4))) + 2);
      *reg32(PJDL_HW_BASE_ADDR, PDJL_HW_OFFSET_ACTIVATE_DMA_RECEIVING) = 0x00000000; // deactivate dma-receiving
      //printf("timeout: %x, one: %x\n", _timeout, (2*SWBB_BIT_SPACER) + (SWBB_BIT_WIDTH/4));
      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_AXIS) = PJDL_HW_LAST_BIT | PJDL_HW_ACK_REQUEST_BIT | timeout_repetitions; // request ACK
      while((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)&PJDL_HW_DIRECT_SEND_DONE_MASK) == 0x00); // wait for command to start
      while((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)&PJDL_HW_SENDING_IN_PROGRESS_MASK) != 0x00) // wait for sending to end (receiving to start)
      while((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)&PJDL_HW_RECEIVING_IN_PROGRESS_MASK) != 0x00); // wait for receiving to end
      uint32_t start =  PJON_MILLIS();
      while((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)&PJDL_HW_DIRECT_DATA_READY_MASK) == 0x00){ // wait for response
        if((uint32_t)(PJON_MILLIS() - start) > 1) { // should't take more than 1ms -> possible even less
          return SWBB_FAIL; // receiving ended, no response
        }
      }; 
      *reg32(PJDL_HW_BASE_ADDR, PDJL_HW_OFFSET_ACTIVATE_DMA_RECEIVING) = 0x00000001; // activate dma-receiving
      return (*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_AXIS)&0x000000FF); // return response
    };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      // ToDo: check if receiving is not still in progress
      if(max_length == PJON_PACKET_MAX_LENGTH){ // ToDo: first bytes should be returned earlier
        //if((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)&PJDL_HW_DIRECT_DATA_READY_MASK) != 0x00){
        if(*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_DMA_RECEIVED_DATA_INDICATOR) != 0x00){ // check if dma has received data
          *reg32(IDMA_BASE_ADDR, IDMA_SOURCE_OFFSET) = PJDL_HW_BASE_ADDR + PJDL_HW_OFFSET_AXIS; // source
          *reg32(IDMA_BASE_ADDR, IDMA_DESTINATION_OFFSET) = (uint32_t)(data); // destination
          *reg32(IDMA_BASE_ADDR, IDMA_LENGTH_OFFSET) = max_length; // length & start

          while(*reg32(IDMA_BASE_ADDR, IDMA_DONE_OFFSET)==0x00); // wait for idma to complete the task
          *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_DMA_RECEIVED_DATA_INDICATOR) = 0x00; // reset the indicator

          //printf("PJON received %x, %x, %x, %x, %x, %x, %x, %x, %x\n", data[0], data[1], data[2], data[3] ,data[4],data[5],data[6],data[7],data[8]);
          //uart_write_flush();

          return 1;
        }
        return SWBB_FAIL;
      }else{
        return 1;
      }
      // ToDo: iDMA has to stop transfer after byte with tlast set
      // ToDo: dessign possibility for non-blocking receiving
    };


    /* Send byte response:
       Transmitter sends a SWBB_BIT_WIDTH / 4 microseconds long HIGH bit and
       tries to receive a response cyclically for SWBB_RESPONSE_TIMEOUT
       microseconds. Receiver synchronizes to the falling edge of the last
       incoming bit and transmits its response */

    void send_response(uint8_t response) {
      uint32_t full_response = PJDL_HW_LAST_BIT | PJDL_HW_ACK_BIT | (uint32_t)response; 

      //while(*reg32(IDMA_BASE_ADDR, IDMA_DONE_OFFSET) == 0x00); // wait for idma to complete the previous task

      *reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_AXIS) = full_response;

      // wait for command to be processed
      while((*reg32(PJDL_HW_BASE_ADDR, PJDL_HW_OFFSET_STATUS)&PJDL_HW_DIRECT_SEND_DONE_MASK) == 0x00); 
      // wait makes sure, that no dma transfer is stated, wich would have higher priority; response should always have priority
      // ToDo: solve problem of the possibility that higher priority values are already in the queue!
    };


    /* The data is prepended with a frame initializer composed by 3
       synchronization pads to signal the start of a frame.
     _________________ __________________________________
    |   FRAME INIT    | DATA 1-65535 bytes               |
    |_____ _____ _____|________________ _________________|
    |Sync |Sync |Sync |Sync | Byte     |Sync | Byte      |
    |___  |___  |___  |___  |     __   |___  |      _   _|
    |   | |   | |   | |   | |    |  |  |   | |     | | | |
    | 1 |0| 1 |0| 1 |0| 1 |0|0000|11|00| 1 |0|00000|1|0|1|
    |___|_|___|_|___|_|___|_|____|__|__|___|_|_____|_|_|_|

    Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      _timeout = (length * SWBB_RESPONSE_OFFSET) + SWBB_LATENCY;
      //while(*reg32(IDMA_BASE_ADDR, IDMA_DONE_OFFSET)==0x00); // wait for idma to complete the previous task
      while((*reg32(IDMA_BASE_ADDR, IDMA_STATUS_OFFSET) & 0x80) !=0x00); // wait for the iDMA buffer to be empty

      *reg32(IDMA_BASE_ADDR, IDMA_SOURCE_OFFSET) = (uint32_t)(data); // source
      *reg32(IDMA_BASE_ADDR, IDMA_DESTINATION_OFFSET) = PJDL_HW_BASE_ADDR + PJDL_HW_OFFSET_AXIS; // destination
      *reg32(IDMA_BASE_ADDR, IDMA_LENGTH_OFFSET) = length; // length & start
    };

  private:
    uint16_t _timeout;
};
