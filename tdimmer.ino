/***************************************************************************//**
* @file tdimmer.ino
*******************************************************************************/

#include "sm_dimmer.h"


#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
   #define KeyA_pin   3
   #define KeyB_pin   4
#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
   #define KeyA_pin   8
   #define KeyB_pin   9
#else
   #error Unknown CPU: provide code for your CPU
#endif



/***************************************************************************//**
* @brief Timer class for generate cyclic signals
*******************************************************************************/
struct Tmr {
   int32_t  Period;        // cycle length (it must be initialized!)
   uint32_t TrefMillis;
   //---------------------------------------------------------------------------
   bool Check(uint32_t t) {            // check with external timebase
      int32_t tdif = t - TrefMillis;
      if (tdif >= Period) { // period expired
         TrefMillis += Period;
         return true;
      }
      return false;
   };

   bool Check() {                      // check with millisec timebase
      uint32_t t = millis();
      int32_t tdif = t - TrefMillis;
      if (tdif >= Period) { // period expired
         TrefMillis += Period;
         return true;
      }
      return false;
   };
};

Tmr Tmr10ms  = { .Period = 10   };
Tmr Tmr100ms = { .Period = 100  };
Tmr Tmr1s    = { .Period = 1000 };


/***************************************************************************//**
* @brief Arduino init
*******************************************************************************/
void setup() {
   pinMode(KeyA_pin, INPUT_PULLUP);
   pinMode(KeyB_pin, INPUT_PULLUP);

   DimmerSM_Init();
}

/***************************************************************************//**
* @brief Main loop
*******************************************************************************/
void loop() {
   uint32_t t = millis();

   if (Tmr10ms.Check(t)) { // execute every 10ms
      static uint8_t KeyA = 0xFF;
      uint8_t a = digitalRead(KeyA_pin);
      if (a != KeyA) {
         if (a) DimmerSM_EventProc(EV_DSM_SW1_REL);
         else   DimmerSM_EventProc(EV_DSM_SW1_PRESS);
         KeyA = a;
      }
      static uint8_t KeyB = 0xFF;
      uint8_t b = digitalRead(KeyB_pin);
      if (b != KeyB) {
         if (b) DimmerSM_EventProc(EV_DSM_SW2_REL);
         else   DimmerSM_EventProc(EV_DSM_SW2_PRESS);
         KeyB = b;
      }
      DimmerSM_EventProc(EV_TIMER_TICK);
      DimmerSM_EventProc(EV_DSM_TICK_10MS);
   }

   if (Tmr100ms.Check(t)) { // execute every 100ms
      DimmerSM_EventProc(EV_DSM_TICK_100MS);
   }

   if (Tmr1s.Check(t)) { // execute every 1s
      DimmerSM_EventProc(EV_DSM_TICK_1S);
   }
}

