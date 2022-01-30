/****************************************************************************
 Module
   OptoSensorService.c

 Revision
   1.0.1

 Description
 This is OptoSensorService - used to get reading from optosensor that is used
 as a signal to start the game.

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

// Events & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "dbprintf.h"

// Analog Lib
#include "PIC32_AD_Lib.h"

//Service Headers
#include "OptoSensorService.h"
#include "GameService.h"

/*----------------------------- Module Defines ----------------------------*/
// these times assume a 1.000mS/tick timing
#define ONE_MILI_SEC 1
#define ONE_SEC 1000
#define TICS_PER_MS 2500
#define THRESH 970
#define WAIT 300 * ONE_MILI_SEC // wait before reading sensor again 300 ms
#define deltaToPost 5

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
static OptoState_t CurrentState;
static uint8_t MyPriority;
/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitOptoSensorService

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     Aaron Brown, 11/06/21, 18:36
****************************************************************************/
bool InitOptoSensorService(uint8_t Priority)
{
  ES_Event_t ThisEvent;
  //set the priority
  MyPriority = Priority;

  /*
  ADC Configuration is done in GameService
  Pin Used for Throttle: RB15 (AN9)
  */

  //set current state as active
  CurrentState = Active;

  //post ES_INIT event to this service
  ThisEvent.EventType = ES_INIT;
  if (ES_PostToService(MyPriority, ThisEvent) == true)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/****************************************************************************
 Function
     PostOptoSensorService

 Parameters
     ES_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Aaron Brown, 11/06/21, 18:36
****************************************************************************/
bool PostOptoSensorService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunOptoSensorService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Reads and Posts values of optosensor to the Game at a fixed frequency defined by timeouts.
 
 Author
     Aaron Brown, 11/06/21, 18:36
****************************************************************************/
ES_Event_t RunOptoSensorService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
  case (Active):
  {
    switch (ThisEvent.EventType)
    {
    case (RESET):
    {
      //Post ES_INIT to this service if RESET Event is received
      ThisEvent.EventType = ES_INIT;
      PostOptoSensorService(ThisEvent);
    }
    break;
    case (ES_INIT):
    {
      //Initialize the timer
      ES_Timer_InitTimer(OPTO_TIMER, WAIT);
    }
    break;

    case (ES_TIMEOUT):
    {
      //On timeout, post READ event to this service
      ES_Event_t Event2Post;
      Event2Post.EventType = ROS_READ;
      PostOptoSensorService(Event2Post);
    }
    break;

    case (ROS_READ):
    {
      //Read values from ADC Pins and post optosensor value to game
      uint32_t currentVal[3];
      ADC_MultiRead(currentVal);
      //if value is less than threshold -> hand detected is posted to Game
      if (currentVal[0] < THRESH)
      {
        //Switch States
        CurrentState = Idle;
        ES_Event_t newEvent;
        newEvent.EventType = ES_HAND_DETECTED;
        PostGameService(newEvent); //post to game service
      }
      else
      {
        //Restart Timer
        ES_Timer_InitTimer(OPTO_TIMER, WAIT);
      }
    }
    break;
    default:
    {
    }
    break;
    }
  }
  break;

  case (Idle):
  {
    switch (ThisEvent.EventType)
    {
    case (ROS_RESET):
    {
      //Change State to Active and Init Timer
      CurrentState = Active;
      ES_Timer_InitTimer(OPTO_TIMER, WAIT);
    }
    break;
    case (RESET):
    {
      // Change State to Active and Post ES_INIT to this service
      CurrentState = Active;
      ThisEvent.EventType = ES_INIT;
      PostOptoSensorService(ThisEvent);
    }
    break;

    default:
    {
    }
    break;
    }
  }
  break;

  default:
  {
  }
  break;
  }
  return ReturnEvent;
}