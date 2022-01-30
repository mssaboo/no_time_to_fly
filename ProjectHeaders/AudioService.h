/****************************************************************************

 Description
 This is AudioService -  used to play sounds on AudioModule.   
 ****************************************************************************/

#ifndef AudioService_H
#define AudioService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"

#include <stdint.h>
#include <stdbool.h>

#include "ES_Port.h" // needed for definition of REENTRANT

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
    InitAudio,
    AudioWaiting
} AudioServiceState_t;

// Public Function Prototypes

bool InitAudioService(uint8_t Priority);
bool PostAudioService(ES_Event_t ThisEvent);
ES_Event_t RunAudioService(ES_Event_t ThisEvent);
AudioServiceState_t QueryAudioService(void);

#endif /* AudioService_H */