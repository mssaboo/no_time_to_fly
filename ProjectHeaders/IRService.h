/*
Service for getting values from IR Sensor 
*/

#ifndef IRService_H
#define IRService_H

#include <stdint.h>
#include <stdbool.h>
#include "ES_Events.h"
#include "ES_Port.h"
// Public Function Prototypes

bool InitIRService(uint8_t Priority);
bool PostIRService(ES_Event_t ThisEvent);
ES_Event_t RunIRService(ES_Event_t ThisEvent);

#endif /* IRService_H */
