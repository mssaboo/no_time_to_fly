/****************************************************************************

 Description:
 This is servoservice - used to control the servo motor that is used to indicate
 the time duration since the start of game.  

 ****************************************************************************/

#ifndef ServoService_H
#define ServoService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

#include <stdint.h>
#include <stdbool.h>

#include "ES_Events.h"
#include "ES_Port.h" // needed for definition of REENTRANT

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
    InitServo,
    ServoWaiting,
    ServoPlaying
} ServoServiceState_t;

// Public Function Prototypes

bool InitServoService(uint8_t Priority);
bool PostServoService(ES_Event_t ThisEvent);
ES_Event_t RunServoService(ES_Event_t ThisEvent);
ServoServiceState_t QueryServoService(void);

void DecodeKey(char KeyToDecode);
void TakeMoveStep(void);

#endif /* ServoService_H */