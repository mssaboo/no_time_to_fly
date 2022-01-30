/****************************************************************************
 Module
   IRService.c

 Description
 This is IRService used to get values of IR Sensor at a fixed frequency.

****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

//Services Headers
#include "IRService.h"
#include "GameService.h"

// Analog lib
#include "PIC32_AD_Lib.h"

// Hardware
#include <xc.h>

//Standard C Libraries
#include <stdbool.h>
#include <stdlib.h>

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_Port.h"
#include "terminal.h"
#include "dbprintf.h"
#include "PWM_PIC32.h"

/*----------------------------- Module Defines ----------------------------*/
#define ONE_MILI_SEC 1
#define WAIT 300 * ONE_MILI_SEC
#define deltaToPost 20

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
static uint32_t LastPostedValue[3];

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitIRService

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

bool InitIRService(uint8_t Priority)
{
  ES_Event_t ThisEvent;

  //set priority of this service
  MyPriority = Priority;

  /*
  ADC Configuration is done in GameService
  Pin Used for Throttle: RB12 (AN12)
  ADC_ConfigAutoScan(BIT12HI,1);
  */

  //Read Pin Value to Initialize LastPostedValue
  ADC_MultiRead(LastPostedValue);
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
     PostIRService

 Parameters
     ES_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this Service's queue

 Author
      Mahesh Saboo
****************************************************************************/
bool PostIRService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunIRService

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Posts the value of RB12 (AN12) to GameService at a fixed 
   frequency defined by the timeouts and also depending on change in values
    (delta).

 Author
    Mahesh Saboo
****************************************************************************/
ES_Event_t RunIRService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (ThisEvent.EventType)
  {
  case (ES_INIT):
  {
    //Read Value to LastPostedValue
    ADC_MultiRead(LastPostedValue);
    //Start Timer
    ES_Timer_InitTimer(IR_TIMER, WAIT);
  }
  break;

  case (ES_TIMEOUT):
  {
    //On timeout, Post Event IR_READ to this Service
    ES_Event_t Event2Post;
    Event2Post.EventType = IR_READ;
    PostIRService(Event2Post);
  }
  break;

  case (IR_READ):
  {
    //Read value of IR Sensor
    uint32_t currentVal[3];
    ADC_MultiRead(currentVal);
    //calculate delta
    int delta = currentVal[2] - LastPostedValue[2];
    //if delta is more than deltaToPost, Post value to GameService
    if (((delta > deltaToPost) || (delta < (-1 * deltaToPost))))
    {
      ES_Event_t Event2Post;
      Event2Post.EventType = IR_VALUE;
      Event2Post.EventParam = (uint16_t)currentVal[2];
      PostGameService(Event2Post);
    }
    //set lastPostValue to currentValue
    LastPostedValue[2] = currentVal[2];
    //Re-Init Timer
    ES_Timer_InitTimer(IR_TIMER, WAIT);
  }
  default:
  {
  }
  break;
  }
  return ReturnEvent;
}
