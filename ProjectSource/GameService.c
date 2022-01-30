/****************************************************************************
 Module
   GameService.c

 Description:
    Game Service is a centralized service which interacts with all other services
    and contains rule engines to make decisions at various stages in game.   
*/

/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "bitdefs.h"
#include "dbprintf.h"

//Standard c libraries
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

//Services Headers
#include "GameService.h"
#include "ServoService.h"
#include "AudioService.h"
#include "DCMotorService.h"
#include "LEDMissileService.h"
#include "LEDFuelService.h"
#include "ThrottleService.h"
#include "OptoSensorService.h"
#include "IRService.h"

// PWM Lib
#include "PWM_PIC32.h"

//Analog Lib
#include "PIC32_AD_Lib.h"

/*----------------------------- Module Defines ----------------------------*/

// define time for audio files
#define CountdownTime 4000   // 4 seconds
#define ProgressTime 60000   // 60 seconds
#define InactivityTime 30000 // 30 seconds
#define FuelTime 60000       // 60 seconds
#define FuelBarTime 1000     // 1 second

//define time for missile timers
#define InitMissileTime 100
#define MissileFireTime 4000

// define constants used to control dc motor
#define MAX_READ 1023
#define MIN_READ 300
#define RANGE MAX_READ - MIN_READ
#define MID 661 //((uint16_t) 0.5*RANGE)
#define THRESH 10
#define RANGE_CMDS RANGE - 2 * THRESH
#define CWTHRESH MID + THRESH
#define CCWTHRESH MID - THRESH
#define CWRANGE MAX_READ - MID - THRESH
#define CCWRANGE MID - THRESH - MIN_READ

//delta used to determine if throttle values changed
#define deltaThrottle 10

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

//function to send speed to dc motor
void SendCmd(uint16_t val);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match that of enum in header file

// with the introduction of Gen2, we need a module level Priority var as well

static uint8_t MyPriority;
bool Missile = true;
static GameServiceState_t CurrentState;
static uint16_t lastThrottleValue;
int numMissiles = 1;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitGameService

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

bool InitGameService(uint8_t Priority)
{

    ES_Event_t ThisEvent;
    bool ReturnVal = true; // assume that everything will be OK

    //set currentState to Init
    CurrentState = InitGame;

    //set priority
    MyPriority = Priority;

    //Analog pin configuration and initial read
    uint16_t whichPins = BIT9HI | BIT11HI | BIT12HI; //RB15-Opto-bit9, RB12-IR-bit12,Pin24-bit11 - Potentiometer
    ADC_ConfigAutoScan(whichPins, 3);
    uint32_t LastPostedValue[3];
    ADC_MultiRead(LastPostedValue);

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (!ES_PostToService(MyPriority, ThisEvent))
    {
        printf("\r\n failed ES_INIT game service");
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
     PostGameService

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
bool PostGameService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunGameService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
     Reacts to various events during gameplay

 Author
    Gaby Uribe
****************************************************************************/
ES_Event_t RunGameService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (CurrentState)
    {
    case InitGame: // If current state is initial Pseudo State
    {
        if (ThisEvent.EventType == ES_INIT) // only respond to ES_Init
        {
            //Init Servo
            PostServoService(ThisEvent);
            //Change currentstate to GameWelcome
            CurrentState = GameWelcome;
            //set Missile flag to true
            Missile = true;
        }
    }
    break;

    case GameWelcome:
    {

        switch (ThisEvent.EventType)
        {
        case ES_HAND_DETECTED:
        {
            //Start Countdown to Game Start
            ES_Timer_InitTimer(COUNTDOWN_TIMER, CountdownTime);
            //Play Countdown Audio
            PostAudioService(ThisEvent);
            //Init Throttle Value to 0
            lastThrottleValue = 0;
        }
        break;

        case ES_TIMEOUT: // case ES_COUNTDOWN_TIMEOUT:
        {
            if (ThisEvent.EventParam == COUNTDOWN_TIMER)
            {
                //Countdown ended
                //Start Servo - Progress Timer
                PostServoService(ThisEvent);
                //Change state to GamePlaying
                CurrentState = GamePlaying;
                //Init Missile Timer to fire missile
                ES_Timer_InitTimer(MISSILE_TIMER, InitMissileTime);
            }
        }
        break;

        default:;
            break;
        }
    }
    break;

    case GamePlaying:
    {
        switch (ThisEvent.EventType)
        {
        case RESET:
        {
            //change state to Welcome
            CurrentState = GameWelcome;
            Missile = true;
        }
        break;

        case IR_VALUE:
        {
            //update speed of dc motor based on IR sensor reading
            SendCmd(ThisEvent.EventParam);
            //restart inactivity timer
            ES_Timer_InitTimer(INACTIVITY_TIMER, InactivityTime);
        }
        break;

        case THROTTLE_VALUE:
        {
            //calculate change in throttle value
            int delta = ThisEvent.EventParam - lastThrottleValue;
            if ((delta > deltaThrottle) || (delta < deltaThrottle))
            {
                //re-start inactivity timer
                ES_Timer_InitTimer(INACTIVITY_TIMER, InactivityTime);
            }
            //update lastThrottleValue and post value to LEDFuelService
            lastThrottleValue = ThisEvent.EventParam;
            ES_Event_t Event2Post;
            Event2Post.EventType = FUEL_UPDATE;
            Event2Post.EventParam = lastThrottleValue;
            PostLEDFuelService(Event2Post);
            //set number of missiles based on throttle value
            if (lastThrottleValue < 850)
            {
                numMissiles = 3;
            }
            else if (lastThrottleValue < 925)
            {
                numMissiles = 2;
            }
            else if (lastThrottleValue < 1024)
            {
                numMissiles = 1;
            }
        }
        break;

        case ES_TIMEOUT:
        {
            if (ThisEvent.EventParam == MISSILE_TIMER)
            {
                //time to fire missiles
                ES_Event_t Event2Post;
                Event2Post.EventType = FIRE_MISSILE;
                int randNumMissiles = numMissiles; //1,2,3
                uint16_t param = 0;
                //use binary to int conversion to send missile numbers to LEDMissileService
                for (int i = 0; i < randNumMissiles; i++)
                {
                    int r = rand() % 12;
                    param = param + pow(2, r);
                }
                Event2Post.EventParam = param;
                //post to LEDMissileService
                PostLEDMissileService(Event2Post);
                //restart MissileTimer
                ES_Timer_InitTimer(MISSILE_TIMER, MissileFireTime);
            }

            if (ThisEvent.EventParam == INACTIVITY_TIMER)
            {
                //Inactivity for 30 sec
                //Restart Game and Reset All
                CurrentState = GameRestart;
                ThisEvent.EventType = RESET_ALL;
                PostGameService(ThisEvent);
            }
        }
        break;

        case ES_PROGRESS_DONE:
        {
            //Player won
            //stop timers
            ES_Timer_StopTimer(INACTIVITY_TIMER);
            ES_Timer_StopTimer(FUEL_TIMER);
            ES_Timer_StopTimer(FUEL_BAR_TIMER);
            //change state to win and post event
            CurrentState = GameWin;
            ThisEvent.EventType = ES_WIN;
            PostGameService(ThisEvent);
        }
        break;

        case FUEL_DONE:
        {
            //out of fuel
            //player lost
            ES_Event_t Event2Post;
            Missile = false;
            //post missile hit event
            Event2Post.EventType = ES_MISSILE_HIT;
            PostGameService(Event2Post);
        }
        break;

        case ES_MISSILE_HIT:
        {
            //Stop Motor
            SendCmd(MID);
            //change state to lose and play audio
            CurrentState = GameLose;
            PostAudioService(ThisEvent);
            //post lose event
            ThisEvent.EventType = ES_LOSE;
            PostGameService(ThisEvent);
            Missile = true;
        }
        break;

        default:;
            break;
        }
    }
    break;

    case GameLose:
    {

        if (ThisEvent.EventType == ES_LOSE)
        {
            //Play Audio
            PostAudioService(ThisEvent);
            //Stop Servo
            PostServoService(ThisEvent);
            //Restart Game and Reset
            CurrentState = GameRestart;
            ThisEvent.EventType = RESET_ALL;
            PostGameService(ThisEvent);
        }
    }
    break;

    case GameWin:
    {
        if (ThisEvent.EventType == ES_WIN)
        {
            //Play Audio
            PostAudioService(ThisEvent);
            //Stop Servo
            PostServoService(ThisEvent);
            //Restart Game and Reset All
            CurrentState = GameRestart;
            ThisEvent.EventType = RESET_ALL;
            PostGameService(ThisEvent);
        }
    }
    break;

    case GameRestart:
    {
        switch (ThisEvent.EventType)
        {

        case (MOTOR_RESET):
        {
            // Reset the DC motor
            ThisEvent.EventType = RESET;
            PostDCMotorService(ThisEvent);
            //Reset Servo Motor
            ThisEvent.EventType = SERVO_RESET;
            PostGameService(ThisEvent);
        }
        break;

        case (SERVO_RESET):
        {
            // Reset the servo
            PostServoService(ThisEvent);
            //Reset All hardware and software
            ThisEvent.EventType = RESET_ALL;
            PostGameService(ThisEvent);
        }
        break;

        case (RESET_ALL):
        {
            //Post Reset to all services
            ThisEvent.EventType = RESET;
            PostGameService(ThisEvent);
            PostOptoSensorService(ThisEvent);
            PostAudioService(ThisEvent);
            PostIRService(ThisEvent);
            PostLEDMissileService(ThisEvent);
            PostLEDFuelService(ThisEvent);
            PostServoService(ThisEvent);
            PostDCMotorService(ThisEvent);
            PostThrottleService(ThisEvent);
            //change state to welcome
            CurrentState = GameWelcome;
        }
        break;
        }
    }
    break;

    default:;
        break;
    }
    return ReturnEvent;
}

/*------------------------------ Private Functions ------------------------------*/

//function used to map and send speed to dc motor
void SendCmd(uint16_t val)
{
    // Determine cmd
    double cmd;
    if (val < MIN_READ || val > MAX_READ)
    {
        //if values are out of bound - stop motor
        cmd = 0;
    }
    else
    {
        //map ir values to dc motor speed
        cmd = 100.00 * (val - MIN_READ) / (MAX_READ - MIN_READ);
        cmd = cmd * (45.00 - 25.00) / 100.00 + 25.00;
    }

    //post speed to dc motor service
    ES_Event_t Event2Post;
    Event2Post.EventType = MOTOR_CMD;
    Event2Post.EventParam = (uint16_t)cmd;
    PostDCMotorService(Event2Post);
}
