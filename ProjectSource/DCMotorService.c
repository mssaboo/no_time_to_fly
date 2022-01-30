/****************************************************************************
 Module
   DCMotorService.c

 Revision
   1.0.1

 Description
 This is a DC Motor Service set up to work with the Lego NXT gear motor with 
 encoder. The encoder has 360 counts per revolution.

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/

// Events & Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "dbprintf.h"

// PORT HAL
#include "PIC32_PORT_HAL.h"

//Service Headers
#include "LEDMissileService.h"

// PWM Lib
#include "PWM_PIC32.h"

// This Module
#include "DCMotorService.h"

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

//define directions for motor
#define CW true
#define CCW false

// H-Bridge Ports/Pins
#define HBRIDGE_PORT _Port_B
#define H1_PIN _Pin_6
#define H2_PIN _Pin_7
#define H1 PORTBbits.RB6
#define H2 PORTBbits.RB7
#define HOUT PORTBINV

// PWM Hardware Constants
// Enable A
#define ENA_CHANNEL 4
#define ENA_PIN PWM_RPB2
#define ENA_TIMER _Timer3_
#define ENA_FREQ 500 // Hz

// Encoder Ports
#define ENC_PORT _Port_B
#define ENCA_PIN _Pin_9
#define ENCB_PIN _Pin_8
#define ENCA PORTBbits.RB9
#define ENCB PORTBbits.RB8

// Motor Limits
#define LIMIT_COUNT false
#define MAX_COUNT ((int32_t)1 * 360)
#define MIN_COUNT ((int32_t)-1 * 360)

// define timeout duration for timers
#define ENCODER_TIME 20
#define MOTOR_RESET_TIME 33

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
void SetDir(bool dir);
void SetSpeed(uint8_t cmd);
bool DecodeQuadrature(uint8_t CurrentEncAState, uint8_t CurrentEncBState);
void DecodeMotorKey(char key);

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;

// Motor direction: true: cw, false: ccw
static bool LastDir = CW;

// Motor speed: (0-100) percent
static uint8_t SpeedCmd = 0;

// Motor angular position
static int32_t Count = 0; // Encoder count starts at the bottom

// encoder states
static uint8_t LastEncAState;
static uint8_t LastEncBState;

// bool for initialization
static bool InitComplete = false;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDCMotorService

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
bool InitDCMotorService(uint8_t Priority)
{
    ES_Event_t ThisEvent;

    //set priority
    MyPriority = Priority;

    // Configure H-Bridge pins as digital outputs
    if (!PortSetup_ConfigureDigitalOutputs(HBRIDGE_PORT, H1_PIN))
    {
        return false;
    }
    else if (!PortSetup_ConfigureDigitalOutputs(HBRIDGE_PORT, H2_PIN))
    {
        return false;
    }

    // Configure Motor Encoder pins as digital inputs
    if (!PortSetup_ConfigureDigitalInputs(ENC_PORT, ENCA_PIN))
    {
        return false;
    }
    else if (!PortSetup_ConfigureDigitalInputs(ENC_PORT, ENCB_PIN))
    {
        return false;
    }

    // Then initialize Encoder states by reading the encoder pins
    LastEncAState = ENCA;
    LastEncBState = ENCB;

    /********************************************
   Initialization sequence for timers to do motor drive
   *******************************************/
    uint8_t HowMany = ENA_CHANNEL;
    //PWM Configuration for DC Motor

    //Basic Config
    //Set Frequency
    //Assign Channel to Timer
    //SetDutyOnChannel
    //Map Channel to Output Pin
    if (!PWMSetup_BasicConfig(ENA_CHANNEL))
    {
        return false;
    }
    else if (!PWMSetup_SetFreqOnTimer(ENA_FREQ, ENA_TIMER))
    {
        return false;
    }
    else if (!PWMSetup_AssignChannelToTimer(ENA_CHANNEL, ENA_TIMER))
    {
        return false;
    }
    else if (!PWMOperate_SetDutyOnChannel(SpeedCmd, ENA_CHANNEL))
    {
        return false;
    }
    else if (!PWMSetup_MapChannelToOutputPin(ENA_CHANNEL, ENA_PIN))
    {
        return false;
    }

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (!ES_PostToService(MyPriority, ThisEvent))
    {
        return false;
    }

    return true;
}

/****************************************************************************
 Function
     PostDCMotorService

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
bool PostDCMotorService(ES_Event_t ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDCMotorService

 Parameters
   ES_Event_t : the event to process

 Returns
   ES_Event_t, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
    Moves DC Motor at speed posted from GameService

 Author
     Aaron Brown, 11/06/21, 18:36
****************************************************************************/
ES_Event_t RunDCMotorService(ES_Event_t ThisEvent)
{
    ES_Event_t ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    switch (ThisEvent.EventType)
    {

    case (RESET):
    {
        //Reset Configuration and Stop the Motor
        SetSpeed(101);
        if (!InitComplete)
        {
            H1 = 1;
            InitComplete = true;
            PostDCMotorService(ThisEvent);
        }
        else
        {
            H2 = 0;
            Count = GetAngleDeg();
        }
        //Init Timer to post Angle to LEDMissileService at Fixed Frequency
        ES_Timer_InitTimer(ENCODER_TIMER, ENCODER_TIME);
    }
    break;
    case (ES_INIT):
    {
        //Initialize motor configuration
        if (!InitComplete)
        {
            H1 = 1;
            InitComplete = true;
            PostDCMotorService(ThisEvent);
        }
        else
        {
            H2 = 0;
            SetDir(CW);
            InitComplete = false;
        }
        //Init Timer to post Angle to LEDMissileService at Fixed Frequency
        ES_Timer_InitTimer(ENCODER_TIMER, ENCODER_TIME);
    }
    break;

    case ES_TIMEOUT:
    {
        //If ENCODER_TIMER timeout -> post angle to LEDMissileService
        if (ThisEvent.EventParam == ENCODER_TIMER)
        {
            ES_Event_t Event2Post;
            Event2Post.EventType = ENCODER_UPDATE;
            uint16_t angle = GetAngleDeg();
            Event2Post.EventParam = angle;
            PostLEDMissileService(Event2Post);
            ES_Timer_InitTimer(ENCODER_TIMER, ENCODER_TIME);
        }

        //If MOTOR_RESET_TIMER timeout-> post MOTOR_RESET to this service
        if (ThisEvent.EventParam == MOTOR_RESET_TIMER)
        {
            ThisEvent.EventType = MOTOR_RESET;
            PostDCMotorService(ThisEvent);
        }
    }
    break;

    case (MOTOR_CMD):
    {
        //Set speed of motor as event parameter
        uint8_t cmd = (uint8_t)ThisEvent.EventParam;
        SetSpeed(cmd);
    }
    break;

    case ES_NEW_KEY: // parse commands
    {
        //Used to control motor with keyboard
        char key = (char)ThisEvent.EventParam;
        DecodeMotorKey(key);
    }
    break;

    default:
    {
    }
    break;
    }
    return ReturnEvent;
}

/* Query Functions */

//GetAngleDeg converts Encoder Counts to angle [0,360]
uint16_t GetAngleDeg(void)
{
    uint16_t ang;
    if (Count < 0)
    {
        ang = 360 - ((-Count) % 360);
    }
    else
    {
        ang = Count % 360;
    }
    return ang;
}

/*Event Checkers*/

//CheckEncoderEvents is used to get encoder counts
bool CheckEncoderEvents(void)
{
    bool ReturnVal = false;
    ES_Event_t ThisEvent;

    // Read encoder pins
    uint8_t CurrentEncAState = ENCA;
    uint8_t CurrentEncBState = ENCB;

    if (CurrentEncAState != LastEncAState && CurrentEncAState == 0)
    {
        // State has changed and CurrentState is low

        //get direction of motor rotation
        bool dir = DecodeQuadrature(CurrentEncAState, CurrentEncBState);
        if (dir == CCW)
        {
            //if direction is CCW, increment count by 2
            Count = Count + 2;
        }
        else
        {
            //if direction is CW, decrement count by 2
            Count = Count - 2;
        }
    }
    //Update Last Encoder States
    LastEncAState = CurrentEncAState;
    LastEncBState = CurrentEncBState;

    return ReturnVal;
}

//CheckCountLimits function is used to move motor by a fixed angle
bool CheckCountLimits(void)
{
    bool ReturnVal = false;
    ES_Event_t ThisEvent;

#if (LIMIT_COUNT)
    if (Count >= MAX_COUNT && LastDir == CW)
    {
        DB_printf("\rCount Maxed At: %d\r\n", Count);
        ThisEvent.EventType = MOTOR_MAX;
        PostDCMotorService(ThisEvent);
        ReturnVal = false;
    }
    else if (Count <= MIN_COUNT && LastDir == CCW)
    {
        DB_printf("\rCount Minned At: %d\r\n", Count);
        ThisEvent.EventType = MOTOR_MIN;
        PostDCMotorService(ThisEvent);
        ReturnVal = false;
    }
#endif

    return ReturnVal;
}

/***************************************************************************
 private functions
 ***************************************************************************/

// SetDir is used to set CW/CCW direction of motor
void SetDir(bool dir)
{
    if (dir != LastDir)
    {
        HOUT = (H1_PIN | H2_PIN);
        LastDir = dir;
    }
}

// SetSpeed is used to set the speed of the motor
/*
parameter: uint8_t cmd

cmd<=100 -> ccw direction with min speed at cmd = 0 and max speed at cmd = 100 -> set duty = cmd
cmd>100 and cmd<=201 -> cw direction -> update cmd = cmd - 100 -> set duty = cmd 
*/
void SetSpeed(uint8_t cmd)
{
    if (cmd <= 100)
    {
        SetDir(CCW);
    }
    else if (cmd > 100 && cmd <= 201)
    {
        SetDir(CW);
        cmd = cmd - 101;
    }
    else
    {
        // Bad command, so stop the motor
        cmd = 0;
    }
    SpeedCmd = cmd;
    PWMOperate_SetDutyOnChannel(SpeedCmd, ENA_CHANNEL);
}

// DecodeQuadrature outputs direction of motor rotation
bool DecodeQuadrature(uint8_t CurrentEncAState, uint8_t CurrentEncBState)
{
    bool dir = LastDir;
    // Decode quadrature
    if (CurrentEncAState != LastEncAState)
    {
        // A changed
        if (LastEncAState == 0 && CurrentEncBState > 0)
        {
            // Rising edge, B leading A, CCW
            dir = CCW;
        }
        else if (LastEncAState == 0 && CurrentEncBState == 0)
        {
            // Rising edge, A leading B, CW
            dir = CW;
        }
        else if (LastEncAState == 1 && CurrentEncBState > 0)
        {
            // Falling edge, A leading B, CW
            dir = CW;
        }
        else
        {
            // Falling edge, B leading A, CCW
            dir = CCW;
        }
    }
    return dir;
}

// DecodeMotorKey is used to control motor direction and speed with keyboard
void DecodeMotorKey(char key)
{
    if (key == 'd')
    {
        SetDir(!LastDir);
    }
    else if (key == 'w')
    {
        if (LastDir == CW)
        {
            if (SpeedCmd < 100)
            {
                SetSpeed(SpeedCmd + 1);
            }
        }
        else
        {
            if (SpeedCmd < 100)
            {
                SetSpeed(101 + SpeedCmd + 1);
            }
        }
    }
    else if (key == 's')
    {
        if (LastDir == CW)
        {
            if (SpeedCmd > 0)
            {
                SetSpeed(SpeedCmd - 1);
            }
        }
        else
        {
            if (SpeedCmd > 0)
            {
                SetSpeed(101 + SpeedCmd - 1);
            }
        }
    }
    else if (key == '9')
    {
        if (LastDir == CW)
        {
            SetSpeed(90);
        }
        else
        {
            SetSpeed(101 + 90);
        }
    }
    else if (key == '8')
    {
        if (LastDir == CW)
        {
            SetSpeed(80);
        }
        else
        {
            SetSpeed(101 + 80);
        }
    }
    else if (key == '7')
    {
        if (LastDir == CW)
        {
            SetSpeed(70);
        }
        else
        {
            SetSpeed(101 + 70);
        }
    }
    else if (key == '6')
    {
        if (LastDir == CW)
        {
            SetSpeed(60);
        }
        else
        {
            SetSpeed(101 + 60);
        }
    }
    else if (key == '5')
    {
        if (LastDir == CW)
        {
            SetSpeed(50);
        }
        else
        {
            SetSpeed(101 + 50);
        }
    }
    else if (key == '4')
    {
        if (LastDir == CW)
        {
            SetSpeed(40);
        }
        else
        {
            SetSpeed(101 + 40);
        }
    }
    else if (key == '3')
    {
        //        if (LastDir == CW)
        //        {
        //            SetSpeed(30);
        //        }
        //        else
        {
            SetSpeed(101 + 40);
        }
    }
    else if (key == '2')
    {
        //        if (LastDir == CW)
        {
            SetSpeed(40);
        }
        //        else
        //        {
        //            SetSpeed(101+20);
        //        }
    }
    else if (key == '1')
    {
        if (LastDir == CW)
        {
            SetSpeed(10);
        }
        else
        {
            SetSpeed(101 + 10);
        }
    }
    else if (key == '0')
    {
        if (LastDir == CW)
        {
            SetSpeed(0);
        }
        else
        {
            SetSpeed(101 + 0);
        }
    }
}
