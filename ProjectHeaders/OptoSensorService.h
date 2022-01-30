/* Description
 This is OptoSensorService - used to get reading from optosensor that is used
 as a signal to start the game.*/

#ifndef OptoSensorService_H
#define OptoSensorService_H

#include <stdint.h>
#include <stdbool.h>
#include "ES_Events.h"

// State definitions
typedef enum
{
    Active,
    Idle
} OptoState_t;

// Public Function Prototypes

bool InitOptoSensorService(uint8_t Priority);
bool PostOptoSensorService(ES_Event_t ThisEvent);
ES_Event_t RunOptoSensorService(ES_Event_t ThisEvent);

#endif /* OptoSensorService_H */