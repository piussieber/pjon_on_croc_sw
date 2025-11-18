
// --------------------------------------------------
// Document:  PJON_CROC_Interface.h
// Project:   PJON_ASIC
// Function:  IO-Mapping for the PJON-Library on CROC
// Autor:     Pius Sieber
// Date:      24.02.2025
// Comments:  This interface does not work together with the PJONVirtualBusRouter 
//            and all Libraries that use it. This is beacaue the millies function 
//            was left away for simplicity.
// --------------------------------------------------
#pragma once

#if defined(CROC)
  #include "PJON_CROC_Timing.h"

  #define OUTPUT 1
  #define INPUT 0
  #define HIGH 1
  #define LOW 0
  #define INPUT_PULLUP 0x2
  #define LSBFIRST 1
  #define MSBFIRST 2

  #define PJON_HW_BASE_ADDR 0x20001000
  #define PJON_HW_OFFSET_ID 0x20
  #define PJON_HW_OFFSET_ROUTER_MODE 0x24

  /* Generic constants ---------------------------------------------------- */
  #ifndef A0
    #define A0 0
  #endif

  #ifndef LED_BUILTIN
    #define LED_BUILTIN -1
  #endif

  /* CROC IO system calls ---------------------------------------------- */

  #ifndef PJON_ANALOG_READ
    #define PJON_ANALOG_READ(P) 0 // no analog i/o on croc
  #endif

  #ifndef PJON_IO_WRITE
    #define PJON_IO_WRITE(P,V) V ? gpio_pin_set(P) : gpio_pin_clear(P)
      /*if(V){ \
        gpio_pin_set(P); \
      } else { \
        gpio_pin_clear(P); \
      }*/
  #endif

  #ifndef PJON_IO_READ
    #define PJON_IO_READ gpio_pin_read
  #endif

  #ifndef PJON_IO_MODE
    #define PJON_IO_MODE(P,V) V ? gpio_pin_set_output(P) : gpio_pin_set_input(P)
      /*if(V){ \
        gpio_pin_set_output(P); \
      } else { \
        gpio_pin_set_input(P); \
      }*/
  #endif

  #ifndef PJON_IO_PULL_DOWN
    #define PJON_IO_PULL_DOWN(P) gpio_pin_set_input(P) // no pulldown in croc -> use normal input
  #endif

  /* Random --------------------------------------------------------------- */ // ToDo: implement real random functions

  long random(long min, long max) {
    //return min/2 + max/2;
    return 0; // zero for simulation speed ToDo:replace
  } 

  long random(long max) {
    //return random(0, max);
    return 0; // zero for simulation speed ToDo:replace
  } 

  #ifndef PJON_RANDOM
    #define PJON_RANDOM random
  #endif

  #ifndef PJON_RANDOM_SEED
    #define PJON_RANDOM_SEED(V) (void)0
  #endif

  /* Serial --------------------------------------------------------------- */ // -> not implemented on croc yet

  #ifndef PJON_SERIAL_TYPE
    #define PJON_SERIAL_TYPE (void)0
  #endif

  #ifndef PJON_SERIAL_AVAILABLE
    #define PJON_SERIAL_AVAILABLE(S) (void)0
  #endif

  #ifndef PJON_SERIAL_WRITE
    #define PJON_SERIAL_WRITE(S, C) (void)0
  #endif

  #ifndef PJON_SERIAL_READ
    #define PJON_SERIAL_READ(S) S->read()
  #endif

  #ifndef PJON_SERIAL_FLUSH
    #define PJON_SERIAL_FLUSH(S) (void)0
  #endif

  /* Timing --------------------------------------------------------------- */

  #ifndef PJON_DELAY
    #define PJON_DELAY delay
  #endif

  #ifndef PJON_DELAY_MICROSECONDS
    #define PJON_DELAY_MICROSECONDS delayMicroseconds
  #endif

  #ifndef PJON_MICROS
    #define PJON_MICROS micros
  #endif

  #ifndef PJON_MILLIS
    #define PJON_MILLIS millis
  #endif

#endif
    