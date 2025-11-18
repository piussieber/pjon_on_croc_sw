
// --------------------------------------------------
// Document:    PJON_CROC_Timing.h
// Project:     PJON_ASIC
// Function:    provides the standard arduino timing functions for croc
// Autor:       Pius Sieber
// Date:        10.03.2025
// --------------------------------------------------
#pragma once

#if defined(CROC)
    
    #if defined(CROC_FGPA)
        #define CLOCK_SPEED 20000000 // 20 MHz
    #else
        #define CLOCK_SPEED 80000000 // 80 MHz
    #endif
    #define CLOCK_CYCLES_PER_MICROSECOND (CLOCK_SPEED / 1000000)
    #define CLOCK_CYCLES_PER_MILLISECOND (CLOCK_SPEED / 1000)

    #define HIGH 1
    #define LOW 0
    #define TRUE 1
    #define FALSE 0


    uint32_t millis()
    {
        unsigned long clock_cycles;
        //asm volatile(csrrwi %0, mcycle, x0 : "=r"(clock_cycles)); // read cycle counter -. ToDo: 32 or 64 bits?
        asm volatile("csrr %0, mcycle" : "=r"(clock_cycles)::"memory");

        return clock_cycles/CLOCK_CYCLES_PER_MILLISECOND;
    }

    uint32_t micros() {
        unsigned long clock_cycles;
        //asm volatile(csrrwi %0, mcycle, x0 : "=r"(clock_cycles)); // read cycle counter
        asm volatile("csrr %0, mcycle" : "=r"(clock_cycles)::"memory");

        return clock_cycles/CLOCK_CYCLES_PER_MICROSECOND; 
    }

    void delay(unsigned long ms)
    {
        uint32_t start = micros();

        while (ms > 0) {
            while ( ms > 0 && (micros() - start) >= 1000) {
                ms--;
                start += 1000;
            }
        }
    }

    void delayMicroseconds(unsigned int us)
    {
        /*uint32_t current_clock_cycles;
        asm volatile("csrr %0, mcycle" : "=r"(current_clock_cycles)::"memory");

        uint32_t end = current_clock_cycles + us * CLOCK_CYCLES_PER_MICROSECOND;

        do{
            asm volatile("csrr %0, mcycle" : "=r"(current_clock_cycles)::"memory");
        } while ((current_clock_cycles<end) || ((current_clock_cycles-end)>1000)); // second condition is for the overflow
        */


        uint32_t start;
        asm volatile("csrr %0, mcycle" : "=r"(start)::"memory");

        uint32_t current_clock_cycles;
        while (us > 0) {
            asm volatile("csrr %0, mcycle" : "=r"(current_clock_cycles)::"memory");
            while ( us > 0 && (current_clock_cycles - start) >= CLOCK_CYCLES_PER_MICROSECOND) {
                us--;
                start += CLOCK_CYCLES_PER_MICROSECOND;
            }
        }
    }
#endif