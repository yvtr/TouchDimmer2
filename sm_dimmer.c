/***************************************************************************//**
* @file sm_dimmer.c
*******************************************************************************/

#include "Arduino.h"

#include "sm_dimmer.h"



// Typedefs ====================================================================
struct DIMMER_SM_st {
   STATE_MACHINE sm;
   uint32_t Events;
   uint16_t Timer;
};
typedef struct DIMMER_SM_st  DIMMER_SM;
typedef void DIMMER_STATE_FUNC (DIMMER_SM* me, uint16_t event);


// Local variables =============================================================
static DIMMER_SM DSM;
#define DEFINE_MY_OBJECT()    DIMMER_SM* const me = &DSM;


#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
   #define LedPwm_pin      1
   #define LedExtra_pin    2
#elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
   #define LedPwm_pin      12
   #define LedExtra_pin    11
#else
   #error Unknown CPU: provide code for your CPU
#endif

#define  LED_PWM_DIMMED    64


const uint16_t TholdDim = 1500 / 10;     // [10ms]
const uint16_t ToutExtra = 5000 / 10;    // [10ms]
const uint16_t ToutLongSw = 1500 / 10;   // [10ms]

// Local function declarations =================================================
static void DimmerSt_LedOff(DIMMER_SM* me, uint16_t event);
static void DimmerSt_StartDim(DIMMER_SM* me, uint16_t event);
static void DimmerSt_Dimmed(DIMMER_SM* const me, uint16_t event);
static void DimmerSt_NormalStart(DIMMER_SM* const me, uint16_t event);
static void DimmerSt_NormalBright(DIMMER_SM* const me, uint16_t event);
static void DimmerSt_ExtraBright(DIMMER_SM* const me, uint16_t event);


// Function definitions ========================================================
static inline void Led_SetPwm(uint8_t pwm) { OCR1A = ~pwm; }
static inline void LedExtra_On() { digitalWrite(LedExtra_pin, HIGH); }
static inline void LedExtra_Off() { digitalWrite(LedExtra_pin, LOW); }


//u @startuml
//u skinparam defaultTextAlignment left
//u state DimmerSM {

/***************************************************************************//**
* @brief Initialize state machine
*******************************************************************************/
void DimmerSM_Init() {
   DEFINE_MY_OBJECT();

   Led_SetPwm(0);                            // LED switch off
   #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
   TCCR1 = 1<<PWM1A | 3<<COM1A0 | 1<<CS10;   // Timer1: PWM mode
   #elif defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
   TCCR1A = 3<<COM1A0 | 1<<WGM10;   // Timer1: fast PWM 8bit, clr at top
   TCCR1B = 1<<WGM12 | 1<<CS10;     // Timer1 start, presc:/1
   #else
   #error Unknown CPU: provide code for Timer1 PWM setting
   #endif
   pinMode(LedPwm_pin, OUTPUT);
   pinMode(LedExtra_pin, OUTPUT);

   me->Events = 0;
   StateMachineInit(&me->sm, (SM_STATE_FUNC*)&DimmerSt_LedOff); //u [*] -left-> LedOff
}

/***************************************************************************//**
* @brief Execute event handler
*******************************************************************************/
void DimmerSM_EventProc (uint16_t event) {
   DEFINE_MY_OBJECT();
   StateMachine((STATE_MACHINE*)me, event);
}

/***************************************************************************//**
* @brief Check and execute asynchron event (from event buffer)
*******************************************************************************/
void DimmerSM_CheckEvent (void) {
   DEFINE_MY_OBJECT();

   if (me->Events) {
      uint32_t mask = 1;
      uint8_t ev;
      for (ev = 0; ev < 32; ev++) {
         if (me->Events & mask) {
            me->Events &= ~mask;
            StateMachine((STATE_MACHINE*)me, ev);
            break;
         }
         mask <<= 1;    // shift left
      }
   }
}

/***************************************************************************//**
* @brief Send (asynchron) event to state machine
*******************************************************************************/
static inline void EventSend (DIMMER_SM* const me, uint16_t event) {
   me->Events |= (1 << event);
}


// State definitions ===========================================================


//u state LedOff {
/***************************************************************************//**
* @brief LEDs switched off
*******************************************************************************/
void DimmerSt_LedOff(DIMMER_SM* const me, uint16_t event) {
   switch (event) {
      case EV_STATE_ENTER: {  //u LedOff: entry:
         Led_SetPwm(0);       //u LedOff: LED PWM: 0%
      }break;
      case EV_STATE_EXIT: {
      }break;

      case EV_DSM_SW1_PRESS:
      case EV_DSM_SW2_PRESS: {
         SM_SET_STATE(&DimmerSt_StartDim); //u LedOff -left-> StartDim : push SW1\nor SW2
      }break;
   }
}
//u }

//u state Delay1
//u state StartDim {
/***************************************************************************//**
* @brief LED start dimmed
*******************************************************************************/
void DimmerSt_StartDim(DIMMER_SM* const me, uint16_t event) {
   switch (event) {
      case EV_STATE_ENTER: {  //u StartDim: entry:
         me->Timer = 0;       //u StartDim: t start
         Led_SetPwm(LED_PWM_DIMMED); //u StartDim: LED PWM: dimmed
      }break;
      case EV_STATE_EXIT: {
      }break;

      case EV_DSM_SW1_REL:
      case EV_DSM_SW2_REL: {
         SM_SET_STATE(&DimmerSt_NormalStart); //u StartDim --> NormalStart : release\nSW1 or SW2
      }break;
      case EV_DSM_TICK_10MS: {
         if (++me->Timer > TholdDim) {                   //u StartDim -up-> Delay1 : t>TholdDim
            SM_SET_STATE_DELAYED(&DimmerSt_Dimmed, 200/10); //u Delay1 -> Dimmed
            Led_SetPwm(0);                               //u Delay1: LED PWM: 0%
         }
      }break;
   }
}
//u }

//u state Dimmed {
/***************************************************************************//**
* @brief LED Dimmed
*******************************************************************************/
void DimmerSt_Dimmed(DIMMER_SM* const me, uint16_t event) {
   switch (event) {
      case EV_STATE_ENTER: {  //u Dimmed: entry:
         Led_SetPwm(LED_PWM_DIMMED); //u Dimmed: LED PWM: dimmed
      }break;
      case EV_STATE_EXIT: {
      }break;

      case EV_DSM_SW1_PRESS:
      case EV_DSM_SW2_PRESS: {
         SM_SET_STATE(&DimmerSt_LedOff); //u Dimmed --> LedOff : push SW1\nor SW2
      }break;
   }
}
//u }

//u state NormalStart {
/***************************************************************************//**
* @brief LED normal brightness start phase
*******************************************************************************/
void DimmerSt_NormalStart(DIMMER_SM* const me, uint16_t event) {
   switch (event) {
      case EV_STATE_ENTER: {  //u NormalStart: entry:
         me->Timer = 0;       //u NormalStart: t start
         Led_SetPwm(255);     //u NormalStart: LED PWM: 100%
      }break;
      case EV_STATE_EXIT: {
      }break;

      case EV_DSM_SW1_PRESS: {
         SM_SET_STATE(&DimmerSt_LedOff); //u NormalStart -up-> LedOff : push\nSW1
      }break;
      case EV_DSM_SW2_PRESS: {
         SM_SET_STATE(&DimmerSt_ExtraBright); //u NormalStart --> ExtraBright : push\nSW2
      }break;
      case EV_DSM_TICK_10MS: {
         if (++me->Timer > ToutExtra) {
            SM_SET_STATE(&DimmerSt_NormalBright); //u NormalStart -> NormalBright : t>ToutExtra
         }
      }break;
   }
}
//u }

//u state NormalBright {
/***************************************************************************//**
* @brief LED normal brightness
*******************************************************************************/
void DimmerSt_NormalBright(DIMMER_SM* const me, uint16_t event) {
   switch (event) {
      case EV_STATE_ENTER: {  //u NormalBright: entry:
         me->Timer = 0;       //u NormalBright: t stop
      }break;                 //u NormalBright:
      case EV_STATE_EXIT: {
      }break;

      case EV_DSM_SW1_PRESS: {
         SM_SET_STATE(&DimmerSt_LedOff);  //u NormalBright -up-> LedOff : push\nSW1
      }break;
      case EV_DSM_SW2_PRESS: {
         me->Timer++;                     //u NormalBright: push SW2: t start
      }break;
      case EV_DSM_SW2_REL: {
         SM_SET_STATE(&DimmerSt_LedOff);  //u NormalBright -up-> LedOff : release\nSW2
      }break;
      case EV_DSM_TICK_10MS: {
         if (me->Timer) {                    // timer running
            if (++me->Timer > ToutLongSw) {  // expired timeout for key long pressing
               SM_SET_STATE(&DimmerSt_ExtraBright); //u NormalBright --> ExtraBright : t>ToutLongSw
            }
         }
      }break;
   }
}
//u }

//u state ExtraBright {
/***************************************************************************//**
* @brief LED extra brightness
*******************************************************************************/
void DimmerSt_ExtraBright(DIMMER_SM* const me, uint16_t event) {
   switch (event) {
      case EV_STATE_ENTER: {  //u ExtraBright: entry:
         me->Timer = 0;
         LedExtra_On();       //u ExtraBright: extra LED on
      }break;                 //u ExtraBright:
      case EV_STATE_EXIT: {   //u ExtraBright: exit:
         LedExtra_Off();      //u ExtraBright: extra LED on
      }break;

      case EV_DSM_SW1_PRESS:
      case EV_DSM_SW2_PRESS: {
         SM_SET_STATE(&DimmerSt_LedOff); //u ExtraBright -up-> LedOff : push SW1\nor SW2
      }break;
      case EV_DSM_TICK_10MS: {
      }break;
   }
}
//u }

//u }

//u @enduml
