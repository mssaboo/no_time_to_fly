/*
Game Service is a centralized service which interacts with all other services
and contains rule engines to make decisions at various stages in game. 
*/

#ifndef GameService_H
#define GameService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */
#include "ES_Events.h"
#include "ES_Port.h" // needed for definition of REENTRANT

//standard c libraries
#include <stdint.h>
#include <stdbool.h>

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
    InitGame,
    GameWelcome,
    GameWaiting,
    GamePlaying,
    GameLose,
    GameWin,
    GameRestart
} GameServiceState_t;

// Public Function Prototypes

bool InitGameService(uint8_t Priority);
bool PostGameService(ES_Event_t ThisEvent);
ES_Event_t RunGameService(ES_Event_t ThisEvent);
GameServiceState_t QueryGameService(void);

#endif /* GameService_H */