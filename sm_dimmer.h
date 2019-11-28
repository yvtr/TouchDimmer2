#ifndef __DIMMERSM_H__
#define __DIMMERSM_H__

#ifdef __cplusplus
extern "C" {
#endif
#if 0
} // workaround for CB folding bug
#endif

#include "stdint.h"
#include "statemachine.h"


enum DIMMER_SM_EVENTS_en {
   EV_DSM_TICK_10MS = EV_USER_FIRST,
   EV_DSM_TICK_100MS,
   EV_DSM_TICK_1S,
   EV_DSM_SW1_PRESS,
   EV_DSM_SW1_REL,
   EV_DSM_SW2_PRESS,
   EV_DSM_SW2_REL,
};

extern void DimmerSM_Init();
extern void DimmerSM_EventProc(uint16_t event);
extern void DimmerSM_EventSend(uint16_t event);


#if 0
{  // workaround for CB folding bug
#endif
#ifdef __cplusplus
}
#endif

#endif
