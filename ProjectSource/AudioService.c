/****************************************************************************
 Module
   AudioService.c

 Description
 This is AudioService -  used to play sounds on AudioModule.   
*/

/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

//Services Headers
#include "AudioService.h"
#include "GameService.h"

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "bitdefs.h"

//Standard C Lib
#include <stdbool.h>

//PIC32 Port HAL
#include "PIC32_PORT_HAL.h"

/*----------------------------- Module Defines ----------------------------*/

#define IntroAudio 4000
#define ExplosionAudio 1500
#define WinAudio 18200
#define LoseAudio 1500

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match that of enum in header file

// with the introduction of Gen2, we need a module level Priority var as well

static uint8_t MyPriority;

static AudioServiceState_t CurrentState;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitAudioService

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
    Gaby Uribe
****************************************************************************/

bool InitAudioService(uint8_t Priority)
{
  //set audio module pins as digital outputs
  PortSetup_ConfigureDigitalOutputs(_Port_B, _Pin_3 | _Pin_4 | _Pin_10 | _Pin_11);
  // bringing all the audio pins high so they don't play
  LATBbits.LATB3 = 1;
  LATBbits.LATB4 = 1;
  LATBbits.LATB10 = 1;
  LATBbits.LATB11 = 1;

  ES_Event_t ThisEvent;
  bool ReturnVal = true; // assume that everything will be OK

  //set Initial State
  CurrentState = InitAudio;

  //set Priority
  MyPriority = Priority;

  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (!ES_PostToService(MyPriority, ThisEvent))
  {
    ReturnVal = false;
  }
  else
  {
    // no else clause
  }
  return ReturnVal;
}

/****************************************************************************
 Function
     PostAudioService

 Parameters
     EF_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Gaby Uribe
****************************************************************************/
bool PostAudioService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunAudioService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Play audio files on audio module depending on event received

 Author
  Gaby Uribe
****************************************************************************/
ES_Event_t RunAudioService(ES_Event_t ThisEvent)
{

  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
  case InitAudio: // If current state is initial Pseudo State
  {
    if (ThisEvent.EventType == ES_INIT) // only respond to ES_Init
    {
      printf("\r\nES_INIT received in audio service\r\n");

      CurrentState = AudioWaiting; // move into waiting state
    }
  }
  break;

  case AudioWaiting:
  {
    switch (ThisEvent.EventType)
    {

    case ES_HAND_DETECTED:
    {
      //set hand_detected audio pin to low
      LATBbits.LATB3 = 0;
      //start timer
      ES_Timer_InitTimer(AUDIO_TIMER, IntroAudio);
    }
    break;

    case ES_MISSILE_HIT:
    {
      // play explosion audio
      LATBbits.LATB4 = 0;
      //set timer
      ES_Timer_InitTimer(AUDIO_TIMER, ExplosionAudio);
    }
    break;

    case ES_LOSE:
    {
      // play game over audio audio
      LATBbits.LATB11 = 0;
      //set timer
      ES_Timer_InitTimer(AUDIO_TIMER, LoseAudio);
    }
    break;

    case ES_WIN:
    {
      // play bond theme audio
      LATBbits.LATB10 = 0;
      //set timer
      ES_Timer_InitTimer(AUDIO_TIMER, WinAudio);
    }
    break;

    case ES_TIMEOUT:
    {
      // bringing all the audio pins high so they don't play
      LATBbits.LATB3 = 1;
      LATBbits.LATB4 = 1;
      LATBbits.LATB10 = 1;
      LATBbits.LATB11 = 1;
    }
    break;

    default:;
      break;
    }
  }
  break;

  default:;
    break;
  }
  return ReturnEvent;
}
