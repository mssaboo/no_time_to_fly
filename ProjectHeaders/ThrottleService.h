/*
Service for getting values from Potentiometer that is used as a Throttle 
*/

#ifndef ThrottleService_H
#define ThrottleService_H

#include <stdint.h>
#include <stdbool.h>
#include "ES_Events.h"
#include "ES_Port.h"

// Public Function Prototypes

bool InitThrottleService(uint8_t Priority);
bool PostThrottleService(ES_Event_t ThisEvent);
ES_Event_t RunThrottleService(ES_Event_t ThisEvent);

#endif /*ThrottleService_H*/
