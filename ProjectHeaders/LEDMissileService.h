/*
Service for changing LEDs states to indicate Missiles 
*/

#ifndef LEDMissileService_H
#define LEDMissileService_H

#include <stdint.h>
#include <stdbool.h>
#include "ES_Events.h"
#include "ES_Port.h"

// Public Function Prototypes

typedef enum
{
  InitPState,
  MissileFiring
} MissileState_t;

bool InitLEDMissileService(uint8_t Priority);
bool PostLEDMissileService(ES_Event_t ThisEvent);
ES_Event_t RunLEDMissileService(ES_Event_t ThisEvent);
MissileState_t QueryLEDMIssileService(void);

#endif /* LEDMissileService_H */
