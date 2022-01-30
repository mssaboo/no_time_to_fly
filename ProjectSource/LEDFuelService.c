/****************************************************************************
 Module
   LEDFuelService.c

 Description
Service for calculating fuel value and indicating using 3 LEDs 
 Notes
  LED Matrix is now replaced by 3 LEDs
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/

//Services Headers
#include "LEDFuelService.h"
#include "GameService.h"

// Hardware
#include <xc.h>

// Event & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_Port.h"
#include "terminal.h"
#include "dbprintf.h"

// Standard C lib
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//PIC32 HAL and Lib
#include "PIC32_SPI_HAL.h"
#include "PIC32_PORT_HAL.h"
#include "DM_Display.h"
#include "FontStuff.h"

/*----------------------------- Module Defines ----------------------------*/

#define LED1_PIN _Pin_1
#define LED2_PIN _Pin_1
#define LED3_PIN _Pin_14
#define LED1_PORT _Port_B
#define LED2_PORT _Port_A
#define LED3_PORT _Port_B
#define LED1 PORTBbits.RB1
#define LED2 PORTAbits.RA1
#define LED3 PORTBbits.RB14

/*----------------------------- Module Variables ----------------------------*/
static uint8_t MyPriority;

//variable that stores total LEDs cleared till now (max is 255)
float clrLEDs;
//initial or max fuel value is pow(2,32)
uint32_t fuel = 0b11111111111111111111111111111111;
//variable that stores LEDs to be cleared in current update step
static int currClrLED = 1;

//variables used to clear LEDs as fuel level drops
int new_rows;
int prev_rows;

/*----------------------------- Private Functions ----------------------------*/

//this function decides fuel burn rate depending on the throttle value
float throttleToLED(uint16_t throttle);

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitLEDFuelService

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

bool InitLEDFuelService(uint8_t Priority)
{
  ES_Event_t ThisEvent;
  //set priority
  MyPriority = Priority;

  //set LED pins as output
  PortSetup_ConfigureDigitalOutputs(LED1_PORT, LED1_PIN);
  PortSetup_ConfigureDigitalOutputs(LED2_PORT, LED2_PIN);
  PortSetup_ConfigureDigitalOutputs(LED3_PORT, LED3_PIN);

  // post INIT event to this service
  ThisEvent.EventType = ES_INIT;

  //Turn on LEDs -> indicating max fuel level
  LED1 = 1;
  LED2 = 1;
  LED3 = 1;

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
     PostLEDFuelService

 Parameters
     ES_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this Service's queue

 Author
      Mahesh Saboo
****************************************************************************/

bool PostLEDFuelService(ES_Event_t ThisEvent)
{
  return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunLEDFuelService

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
  Updates value of fuel remaining in response to FUEL_UPDATE event

 Author
    Mahesh Saboo
****************************************************************************/

ES_Event_t RunLEDFuelService(ES_Event_t ThisEvent)
{
  ES_Event_t ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

  switch (ThisEvent.EventType)
  {
  case ES_INIT:
  { // 0 clrLEDs
    clrLEDs = 0;
    //do not clear any additional LEDs
    currClrLED = 0;
    //indicate max fuel level -> turn all leds on
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
  }
  break;
  case (RESET):
  {
    //Reset this service
    //Max Fuel Levels
    ThisEvent.EventType = ES_INIT;
    PostLEDFuelService(ThisEvent);
  }
  break;
  case FUEL_UPDATE:
  {
    //Update Fuel

    //Read Throttle Value
    uint16_t throttle = ThisEvent.EventParam;
    //Decide Fuel Drop Rate
    float currClrLEDs = throttleToLED(throttle);
    //Total Number of LEDs to be Cleared (255 mapped to 3)
    clrLEDs = clrLEDs + currClrLEDs;

    //if out of fuel, post fuel_done to game
    if (clrLEDs >= 255)
    {
      ES_Event_t Event2Post;
      Event2Post.EventType = FUEL_DONE;
      LED1 = 0;
      LED2 = 0;
      LED3 = 0;
      PostGameService(Event2Post);
    }

    //interpolate 255 leds to 3 leds and turn of LEDs depending on values
    new_rows = clrLEDs / 85;
    if (new_rows != prev_rows)
    {

      if (new_rows == 1)
      {
        LED1 = 0;
      }
      else if (new_rows == 2)
      {
        LED2 = 0;
      }
      else if (new_rows == 3)
      {
        LED3 = 0;
      }
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

//private functions

//this function decides fuel burn rate depending on the throttle value
float throttleToLED(uint16_t throttle)
{
  float returnVal = 0.00;
  if (throttle < 850)
  {
    returnVal = 0.35;
  }
  else if (throttle < 925)
  {
    returnVal = 0.53;
  }
  else if (throttle < 1024)
  {
    returnVal = 1.06;
  }
  return returnVal;
}
