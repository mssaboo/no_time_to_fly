/****************************************************************************
 Module
   ServoService.c

 Description:
 This is servoservice - used to control the servo motor that is used to indicate
 the time duration since the start of game.  
*****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "bitdefs.h"

//C Standard Libraries
#include <stdbool.h>

//Services Headers
#include "ServoService.h"
#include "AudioService.h"
#include "GameService.h"

// PWM Lib
#include "PWM_PIC32.h"

/*----------------------------- Module Defines ----------------------------*/
// these times assume a 1.000mS/tick timing
#define ONE_SEC 1000
#define HALF_SEC (ONE_SEC / 2)
#define TWO_SEC (ONE_SEC * 2)
#define FIVE_SEC (ONE_SEC * 5)
#define TEN_MS (10)
#define TWENTY_FIVE_MS (25)
#define FIFTY_MS (50)

// TICS_PER_MS assumes a 20MHz PBClk /8 = 2.5MHz clock rate
#define TICS_PER_MS 2500

// these are the initial extents of servo motion
#define FULL_CW ((uint16_t)(0.7 * TICS_PER_MS))
#define FULL_CCW ((uint16_t)(2.25 * TICS_PER_MS))
#define MID_POINT (cwLimit + ((ccwLimit - cwLimit) / 2))

// these are related to how fast we move. full range of motion in 100 steps
#define TICKS_PER_STEP ((FULL_CCW - FULL_CW) / 100)

#define DIRECTION_CW true
#define DIRECTION_CCW false

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
void DecodeKey(char KeyToDecode);
void TakeMoveStep(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match that of enum in header file

// with the introduction of Gen2, we need a module level Priority var as well

static uint8_t MyPriority;

static ServoServiceState_t CurrentState;

// track where we are in number of PWM ticks
static uint16_t CurrentPosition;
// allow us to experiment with changing rotation limits
static uint16_t cwLimit = FULL_CW;
static uint16_t ccwLimit = FULL_CCW;
// allow us to demo changing speed for slews
static uint16_t timeStep = TWENTY_FIVE_MS * 21;
// variables that need to be accessible both in the run and decode functions
static bool isMoving = false;              // start off not moving
static bool moveDirection = DIRECTION_CCW; // arbitrary initial move direction

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitServoService

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

bool InitServoService(uint8_t Priority)
{
  ES_Event_t ThisEvent;
  bool ReturnVal = true; // assume that everything will be OK

  //set currentState to InitServo
  CurrentState = InitServo;

  //set priority
  MyPriority = Priority;

  // PWM Configuration Steps

  //Basic Config
  //Set Frequency
  //Assign Channel to Timer
  //Set Pulse Width
  //Map Channel to Outpin Pin
  if (!PWMSetup_BasicConfig(2))
  {
    ReturnVal = false;
  }
  else if (!PWMSetup_SetFreqOnTimer(50, _Timer2_)) // set freq to 50Hz
  {
    ReturnVal = false;
  }
  else if (!PWMSetup_AssignChannelToTimer(2, _Timer2_))
  {
    ReturnVal = false;
  }
  else if (!PWMOperate_SetPulseWidthOnChannel(MID_POINT, 2))
  {
    ReturnVal = false;
  }
  else if (!PWMSetup_MapChannelToOutputPin(2, PWM_RPB5))
  {
    ReturnVal = false;
  }
  else
  {
    // no else clause
  }

  //set currentPosition to ccwLimit
  CurrentPosition = ccwLimit;

  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (!ES_PostToService(MyPriority, ThisEvent))
  {
    ReturnVal = false;
  }
  else
  {
    printf("success ES_INIT servo service");
    // no else clause
  }
  return ReturnVal;
}

/****************************************************************************
 Function
     PostServoService

 Parameters
     ES_Event_t ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     Gaby Uribe
****************************************************************************/
bool PostServoService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunServoService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
    Used to control servo motor. Moves servo motor at every timeout event

 Author
    Gaby Uribe
****************************************************************************/
ES_Event_t RunServoService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (CurrentState)
  {
  case InitServo: // If current state is initial Pseudo State
  {
    if (ThisEvent.EventType == ES_INIT) // only respond to ES_Init
    {
      ThisEvent.EventParam = 'r';
      DecodeKey(ThisEvent.EventParam); // decode and perform actions

      CurrentState = ServoWaiting; // move into waiting state
    }
  }
  break;

  case ServoWaiting:
  {
    switch (ThisEvent.EventType)
    {

    case ES_INIT:
    {
      ThisEvent.EventParam = 'r';
      DecodeKey(ThisEvent.EventParam); // decode and perform actions
    }
    break;

    case ES_TIMEOUT: // implement motion
    {
      if ((SERVO_STEP_TIMER == ThisEvent.EventParam) && (true == isMoving))
      {
        // now take steps in the indicated direction observing limits
        TakeMoveStep();
        if (true == isMoving) // if still moving
        {
          ES_Timer_InitTimer(SERVO_STEP_TIMER, timeStep); //restart for next step
        }
      }
    }
    break;

    case ES_WIN:
    {
      ES_Timer_StopTimer(SERVO_STEP_TIMER); // stop servo
    }
    break;

    case ES_LOSE:
    {
      ES_Timer_StopTimer(SERVO_STEP_TIMER); // stop servo
    }
    break;

    case (RESET):
    {
      // reset the servo
      ThisEvent.EventParam = 'r';
      DecodeKey(ThisEvent.EventParam); // decode and perform actions
      //stop timer
      ES_Timer_StopTimer(SERVO_STEP_TIMER);
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

/*------------------Private Functions----------------*/

//DecodeKey is used to control the servo motor
void DecodeKey(char KeyToDecode)
{
  switch (KeyToDecode)
  {

  case 'r': // move to CCW limit
  {
    PWMOperate_SetPulseWidthOnChannel(ccwLimit, 2);
    CurrentPosition = ccwLimit;
    ES_Timer_StopTimer(SERVO_STEP_TIMER);
  }
  break;

  case 'b': // slow move CW
  {
    ES_Timer_InitTimer(SERVO_STEP_TIMER, timeStep);
    isMoving = true;
    moveDirection = DIRECTION_CW;
  }
  break;

  default:
  {
  }
  break;
  }
  return;
}

// moves servo by fixed number of steps
void TakeMoveStep(void)
{
  if (DIRECTION_CCW == moveDirection)
  {
    CurrentPosition += TICKS_PER_STEP;
    if (ccwLimit >= CurrentPosition) // below the CCW limit?
    {
      PWMOperate_SetPulseWidthOnChannel(CurrentPosition, 2); //yes, execute
    }
    else // no, so restore position & stop
    {
      CurrentPosition -= TICKS_PER_STEP;
      isMoving = false;
    }
  }
  else if (DIRECTION_CW == moveDirection)
  {
    CurrentPosition -= TICKS_PER_STEP;
    if (cwLimit <= CurrentPosition) // below the CW limit?
    {
      PWMOperate_SetPulseWidthOnChannel(CurrentPosition, 2); //yes, execute
    }
    else // no, so restore position & stop
    {
      CurrentPosition += TICKS_PER_STEP;
      isMoving = false;
      ES_Event_t newEvent;
      //indicates that servo has reached its CW limit -> game won
      newEvent.EventType = ES_PROGRESS_DONE;
      PostGameService(newEvent);
    }
  }

  return;
}
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
