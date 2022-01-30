/****************************************************************************
 Module
   LEDMissileService.c

 Description
 This is LEDMissileService used to Blink the LEDs(Missiles). Active LEDs are
 sent from GameService. This service also detects if the helicopter has been 
 hit by the Missile.

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

//Services Headers
#include "LEDMissileService.h"
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

//PIC32 Port HAL
#include "PIC32_PORT_HAL.h"

/*----------------------------- Module Defines ----------------------------*/

//define statements for timer frequency
#define INIT_TIME 200
#define BLINK_TIME 200
#define MISSILE_TIME 1000

//define statements for angles and threshold parameter for collision
#define delta 30
#define thresh 10

//Standard Service Module variables
static uint8_t MyPriority;
MissileState_t CurrentState;

//Module level variables to indicate status of Missile
int BlinkFlag;
int MissileFlag;

//temp is used to decode parameter from GameService
uint16_t temp;

//array to indicates state of all 12 LEDs
int ledStatus[12] = {0};

//variables to index of active missiles
uint16_t led1 = 0;
uint16_t led2 = 0;
uint16_t led3 = 0;

//variable to check if missile is active when encoder event is received
bool EncoderFlag = false;

/*----------------------------- Private Function Prototypes ----------------------------*/

//Init Function to Turn all LEDs off
void INIT();

//Function to set LEDs l1,l2,l3 to state 'status (ON/OFF)'
void ledOn(uint16_t l1, uint16_t l2, uint16_t l3, int status);

//Function to decode Active LEDs sent by GameService
void decodeParam(uint16_t param, int *ledStatus);

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitLEDMissileService

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

bool InitLEDMissileService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    //set priority for service
    MyPriority = Priority;

    //configure pins for shift register as output pins
    PortSetup_ConfigureDigitalOutputs(_Port_A, _Pin_2 | _Pin_3 | _Pin_4);

    ThisEvent.EventType = ES_INIT;

    //call INIT() to turn off all LEDs
    INIT();

    //set currentState of SM to InitPState
    CurrentState = InitPState;

    //Post ES_INIT to Service
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
     PostLEDMissileService

 Parameters
     ES_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this Service's queue

 Author
      Mahesh Saboo
****************************************************************************/

bool PostLEDMissileService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunLEDMissileService

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
    Controls states of LEDs (Missiles) and detects if the helicopter is hit by
    the Missile.
    To indicate a Missile, LEDs blink thrice. First two blinks indicate approaching
    state of missile and the third blink indicates that the missile has arrived
    at that particular position.

 Author
    Mahesh Saboo
****************************************************************************/
ES_Event_t RunLEDMissileService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
    MissileState_t NextState;
    switch (CurrentState)
    {
    case (InitPState):
    {
        if (ThisEvent.EventType == ES_INIT)
        {
            //Change state to MissileFiring
            CurrentState = MissileFiring;

            //Set BlinkFlag and MissileFlag to 0
            BlinkFlag = 0;
            MissileFlag = 0;
        }
    }
    break;
    case (MissileFiring):
    {
        if (ThisEvent.EventType == RESET)
        {
            // Respond to RESET --> set Flags to 0
            BlinkFlag = 0;
            MissileFlag = 0;
        }

        if (ThisEvent.EventType == FIRE_MISSILE)
        {
            //Decode the Event_Param and get values of ledStatus
            //ledStatus is an array with information of next states of LEDs
            temp = ThisEvent.EventParam;
            decodeParam(temp, ledStatus);
            //set led1,led2,led3 to 0
            led1 = 0;
            led2 = 0;
            led3 = 0;
            //set count to 0
            //at any instant, at max 3 LEDs can act as Missiles, so max value of count can be 3
            int count = 0;

            //get values of led1,led2,led3 from ledStatus array
            for (int i = 0; i < 12; i++)
            {
                if (ledStatus[i] == 1)
                {
                    if (count == 0)
                    {
                        led1 = i + 1;
                        count++;
                    }
                    else if (count == 1)
                    {
                        led2 = i + 1;
                        count++;
                    }
                    else if (count == 2)
                    {
                        led3 = i + 1;
                        count++;
                    }
                }
            }

            //set flags to 0
            BlinkFlag = 0;
            MissileFlag = 0;

            //set a brief timer before firing missiles
            ES_Timer_InitTimer(LED_MISSILE_TIMER, INIT_TIME);
        }

        if (ThisEvent.EventType == ES_TIMEOUT)
        {

            if (BlinkFlag == 0)
            {
                //indicates first blink --> ON
                ledOn(led1, led2, led3, 1);
                //Re-Init Blink Timer
                ES_Timer_InitTimer(LED_MISSILE_TIMER, BLINK_TIME);
                BlinkFlag = 1;
            }
            else if (BlinkFlag == 1)
            {
                //indicates first blink --> OFF
                ledOn(led1, led2, led3, 0);
                //Re-Init Blink Timer
                ES_Timer_InitTimer(LED_MISSILE_TIMER, BLINK_TIME);
                BlinkFlag = 2;
            }
            else if (BlinkFlag == 2)
            {
                //indicates second blink --> ON
                ledOn(led1, led2, led3, 1);
                //Re-Init Blink Timer
                ES_Timer_InitTimer(LED_MISSILE_TIMER, BLINK_TIME);
                BlinkFlag = 3;
            }
            else if (BlinkFlag == 3)
            {
                //indicates second blink --> OFF
                ledOn(led1, led2, led3, 0);
                //Re-Init Blink Timer
                ES_Timer_InitTimer(LED_MISSILE_TIMER, BLINK_TIME);
                BlinkFlag = 4;
                MissileFlag = 0;
            }
            else if (MissileFlag == 0)
            {
                //indicates third blink (MISSILE) --> ON
                ledOn(led1, led2, led3, 1);
                ES_Timer_InitTimer(LED_MISSILE_TIMER, MISSILE_TIME);
                MissileFlag = 1;
                //set encoderflag true so that if helicopter is at this position during this time, collision is recorded
                EncoderFlag = true;
            }
            else if (MissileFlag == 1)
            {
                //indicates third blink (MISSILE) --> OFF
                ledOn(led1, led2, led3, 0);
                MissileFlag = 2;
                EncoderFlag = false;
            }
        }

        if (ThisEvent.EventType == ENCODER_UPDATE)
        {
            //if Missile is Active
            if (EncoderFlag == true)
            {
                uint16_t position = ThisEvent.EventParam;
                for (int i = 0; i < 12; i++)
                {
                    //check Missile Status
                    if (ledStatus[i] == 1)
                    {
                        //check position with some threshold
                        if ((position <= (i * delta + thresh)) && (position >= (i * delta - thresh)))
                        {
                            ES_Event_t Event2Post;
                            //indicates missile hit
                            //send event to Game
                            Event2Post.EventType = ES_MISSILE_HIT;
                            PostGameService(Event2Post);
                        }
                    }
                }
            }
        }
        break;
    }
    break;
    }
    return ReturnEvent;
}

MissileState_t QueryLEDMIssileService(void)
{
    return CurrentState;
}

//Funtion to set all LEDs off
void INIT()
{
    //set Data Line to Low
    PORTAbits.RA2 = 0;
    for (int i = 0; i < 12; i++)
    {
        //Send Clock Pulse
        PORTAbits.RA4 = 1;
        PORTAbits.RA4 = 0;
    }
    //Send Pulse
    PORTAbits.RA3 = 1;
    PORTAbits.RA3 = 0;
}

//Set LEDs states as per status. Writes data to shift register
void ledOn(uint16_t l1, uint16_t l2, uint16_t l3, int status)
{
    for (int i = 0; i < 12; i++)
    {
        if (i == 12 - l1 || i == 12 - l2 || i == 12 - l3)
        {
            PORTAbits.RA2 = status;
        }
        else
        {
            PORTAbits.RA2 = 0;
        }
        int t = 100; // added to maintain delay after sending data
        PORTAbits.RA4 = 1;
        PORTAbits.RA4 = 0;
    }
    PORTAbits.RA3 = 1;
    PORTAbits.RA3 = 0;
}

//Decodes event parameter sent from Game
//Integer to Binary
void decodeParam(uint16_t param, int *ledStatus)
{
    int i = 0;
    for (; param > 0; i++)
    {
        ledStatus[i] = param % 2;
        param = param / 2;
    }
    for (; i < 12; i++)
    {
        ledStatus[i] = 0;
    }
}
