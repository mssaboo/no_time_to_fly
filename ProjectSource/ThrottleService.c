/****************************************************************************
 Module
   ThrottleService.c

 Description
 This is ThrottleService used to get the potentiometer (throttle) values
 at a defined frequency and posts it to GameService.

****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

//Services Headers
#include "ThrottleService.h"
#include "GameService.h"

// Hardware
#include <xc.h>

//C Standard Libraries
#include <stdbool.h>
#include <stdlib.h>

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_Port.h"
#include "terminal.h"
#include "dbprintf.h"

//PWM Library
#include "PWM_PIC32.h"

//Analog Library
#include "PIC32_AD_Lib.h"

/*----------------------------- Module Defines ----------------------------*/

//define variables for frequency of measurement updates
#define POST_THROTTLE_TIME 125

//Standard Service Module variables
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitThrottleService

 Parameters
     uint8_t : the priority of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority and does any
     other required initialization for this Service
 
 Author
     Mahesh Saboo
****************************************************************************/

bool InitThrottleService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  //set priority of this service
  MyPriority = Priority;

  /*
  ADC Configuration is done in GameService
  Pin Used for Throttle: Pin24 (AN11)
  ADC_ConfigAutoScan(BIT11HI,1);Pin 24
  */

  //Post ES_INIT event to this service
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
     PostThrottleService

 Parameters
     ES_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this Service's queue

 Author
      Mahesh Saboo
****************************************************************************/
bool PostThrottleService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunThrottleService

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Posts the value of Pin24 (AN11) to GameService at a fixed 
   frequency defined by the timeouts

 Author
    Mahesh Saboo
****************************************************************************/
ES_Event_t RunThrottleService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (ThisEvent.EventType)
  {
  case (ES_INIT):
  {
    //Set Timer
    ES_Timer_InitTimer(THROTTLE_TIMER, POST_THROTTLE_TIME);
  }
  break;

  case (ES_TIMEOUT):
  {
    //Read ADC Values
    uint32_t currentVal[3];
    ADC_MultiRead(currentVal);
    //Post Potentiometer value as EventParam to GameService
    ES_Event_t Event2Post;
    Event2Post.EventType = THROTTLE_VALUE;
    Event2Post.EventParam = (uint16_t)currentVal[1];
    PostGameService(Event2Post);
    //Re-Init Timer
    ES_Timer_InitTimer(THROTTLE_TIMER, POST_THROTTLE_TIME);
  }
  break;

  default:
  {
  }
  break;
  }
  return ReturnEvent;
}
