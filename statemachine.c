/***************************************************************************//**
* @file statemachine.c
*
* Simple finite state machine
*
*******************************************************************************/

#include <stdint.h>
#include <string.h>

#include "statemachine.h"

void StateMachine(STATE_MACHINE* const me, uint16_t ev) {
   me->State(me, ev);                     // call actual state
   if (me->StateNew != NULL) {            // new state request
      me->StateReturn = me->State;        // remeber last state
      me->State(me, EV_STATE_EXIT);       // closing last state
      me->State = me->StateNew;           // switching state
      me->StateNew = NULL;
      me->State(me, EV_STATE_ENTER);      // init new state
   }
}

void SM_ST_StateDelayed(STATE_MACHINE* const me, uint16_t event) {
   switch (event) {
      case EV_TIMER_TICK: {
         if (me->Timer_StateDelay) {
            me->Timer_StateDelay--;
         }else {
            SM_SET_STATE(me->StateDelayed);
         }
      }break;
      default: break;
   }
}

void StateMachineInit(STATE_MACHINE* const me, SM_STATE_FUNC* const init_state) {
   me->State = init_state;
   me->StateNew = NULL;
   me->StateDelayed = init_state;
   me->StateReturn = init_state;

   me->State(me, EV_STATE_ENTER); // enter into initial state (initial transition)
}

