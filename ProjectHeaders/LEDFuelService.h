/*
Service for calculating fuel value and indicating using 3 LEDs 
*/

#ifndef LEDFuelService_H
#define LEDFuelService_H

#include <stdint.h>
#include <stdbool.h>

#include "ES_Events.h"
#include "ES_Port.h" // needed for definition of REENTRANT
// Public Function Prototypes

bool InitLEDFuelService(uint8_t Priority);
bool PostLEDFuelService(ES_Event_t ThisEvent);
ES_Event_t RunLEDFuelService(ES_Event_t ThisEvent);

#endif /* LEDFuelService_H */
