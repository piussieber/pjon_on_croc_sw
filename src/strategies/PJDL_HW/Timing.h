
/* PJON SoftwareBitBang strategy Transmission Timing table
   Copyright 2010-2022, Giovanni Blu Mitolo All rights reserved.

   Often timing in two different machines do not match, code execution
   time can variate and time measurements are not perfectly equal.
   Consider that durations defined below may differ from what is specified in
   PJDL v5.0. This is done to accomodate machine's inner workings and
   effectively produce the specified timing.

   Arduino Duemilanove/UNO/Nano is used as timing master, or the machine used
   to test all new supported MCUs.

   Benchmarks can be executed using NetworkAnalysis and SpeedTest examples.

   MODE   1: 1.97kB/s - 15808Bd
   MODE   2: 2.21kB/s - 17696Bd
   MODE   3: 3.10kB/s - 24844Bd
   MODE   4: 3.34kB/s - 26755Bd

   Use the same pin number on all connected devices to achieve maximum
   timing efficiency, not all different pin combinations work nominally
   because of execution timing discrepancies between physical pins. */

#pragma once

// HW defines
#if SWBB_MODE == 1
  #if defined(CROC_FGPA)
    #define PJDL_HW_PREAMBLE 0
    #define PJDL_HW_PAD 2200
    #define PJDL_HW_DATA 880
    #define PJDL_HW_ACCEPTANCE 1120
  #else
    #define PJDL_HW_PREAMBLE 0
    #define PJDL_HW_PAD 8800
    #define PJDL_HW_DATA 3520
    #define PJDL_HW_ACCEPTANCE 4480 // calculated from atmega value, ToDo: check for better values
  #endif
#endif
#if SWBB_MODE == 2
  #define PJDL_HW_PREAMBLE 0
  #define PJDL_HW_PAD 7360
  #define PJDL_HW_DATA 3200
  #define PJDL_HW_ACCEPTANCE 4480
#endif
#if SWBB_MODE == 3
  #define PJDL_HW_PREAMBLE 0
  #define PJDL_HW_PAD 5600
  #define PJDL_HW_DATA 2240
  #define PJDL_HW_ACCEPTANCE 2400
#endif
#if SWBB_MODE == 4
  #define PJDL_HW_PREAMBLE 0
  #define PJDL_HW_PAD 5200
  #define PJDL_HW_DATA 2080
  #define PJDL_HW_ACCEPTANCE 2400
#endif

/* CROC ---------------------------------------------------------------- */
#if defined(CROC)
  #if SWBB_MODE == 1
      #define SWBB_BIT_WIDTH   44
      #define SWBB_BIT_SPACER 110
      #define SWBB_ACCEPTANCE  62
      #define SWBB_READ_DELAY   0
  #endif
  #if SWBB_MODE == 2
      #define SWBB_BIT_WIDTH   40
      #define SWBB_BIT_SPACER  92
      #define SWBB_ACCEPTANCE  57
      #define SWBB_READ_DELAY   0
  #endif
  #if SWBB_MODE == 3
      #define SWBB_BIT_WIDTH   28
      #define SWBB_BIT_SPACER  70
      #define SWBB_ACCEPTANCE  40
      #define SWBB_READ_DELAY   0
  #endif
  #if SWBB_MODE == 4
      #define SWBB_BIT_WIDTH   26
      #define SWBB_BIT_SPACER  65
      #define SWBB_ACCEPTANCE  37 
      #define SWBB_READ_DELAY   0
  #endif
#endif

/* Avoid error if any previous defined ------------------------------------ */
#if SWBB_MODE == 1
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   40
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER 112
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  56
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   4
  #endif
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY     13
  #endif
#endif
#if SWBB_MODE == 2
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   36
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER  88
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  56
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   4
  #endif
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY     10
  #endif
#endif
#if SWBB_MODE == 3
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   24
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER  66
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  30
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   8
  #endif
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY      8
  #endif
#endif
#if SWBB_MODE == 4
  #ifndef SWBB_BIT_WIDTH
    #define SWBB_BIT_WIDTH   22
  #endif
  #ifndef SWBB_BIT_SPACER
    #define SWBB_BIT_SPACER  61
  #endif
  #ifndef SWBB_ACCEPTANCE
    #define SWBB_ACCEPTANCE  30
  #endif
  #ifndef SWBB_READ_DELAY
    #define SWBB_READ_DELAY   7
  #endif
  #ifndef SWBB_LATENCY
    #define SWBB_LATENCY      5
  #endif
#endif


/* Frame preamble length (by default set to 1 x SWBB_BIT_SPACER)
   Maximum allowed value is 100 or a preamble of 100 x SWBB_BIT_SPACER
   When devices execute other tasks frames may be lost unheard. The preamble
   can be used to avoid transmission failure. To avoid retransmissions the
   preamble length should be slightly longer than the task's duration. */

#ifndef SWBB_PREAMBLE
  #define SWBB_PREAMBLE 1
#endif

/* Maximum frame preamble length (by default set to 1 x SWBB_BIT_SPACER)
   Maximum allowed value is 100 or a preamble of 100 x SWBB_BIT_SPACER
   To have full interoperability set SWBB_MAX_PREAMBLE = 100 */

#ifndef SWBB_MAX_PREAMBLE
  #define SWBB_MAX_PREAMBLE 1
#endif

/* Synchronous acknowledgement response offset.
   If (latency + CRC computation) > (SWBB_RESPONSE_OFFSET * length)
   synchronous acknowledgement reliability could be affected or disrupted
   set a higher SWBB_RESPONSE_OFFSET if necessary. */

#ifndef SWBB_RESPONSE_OFFSET
  #define SWBB_RESPONSE_OFFSET 20
#endif

/* Maximum initial delay in milliseconds: */

#ifndef SWBB_INITIAL_DELAY
  #define SWBB_INITIAL_DELAY 1000
#endif

/* Maximum delay in case of collision in microseconds: */

#ifndef SWBB_COLLISION_DELAY
  #define SWBB_COLLISION_DELAY 16
#endif

/* Maximum transmission attempts */

#ifndef SWBB_MAX_ATTEMPTS
  #define SWBB_MAX_ATTEMPTS    20
#endif

/* Back-off exponential degree */

#ifndef SWBB_BACK_OFF_DEGREE
  #define SWBB_BACK_OFF_DEGREE  4
#endif
