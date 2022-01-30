/****************************************************************************
 Module
     ES_Configure.h
 Description
     This file contains macro definitions that are edited by the user to
     adapt the Events and Services framework to a particular application.
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 12/19/16 20:19  jec     removed EVENT_CHECK_HEADER definition. This goes with
                         the V2.3 move to a single wrapper for event checking
                         headers
  10/11/15 18:00 jec     added new event type ES_SHORT_TIMEOUT
  10/21/13 20:54 jec     lots of added entries to bring the number of timers
                         and services up to 16 each
 08/06/13 14:10 jec      removed PostKeyFunc stuff since we are moving that
                         functionality out of the framework and putting it
                         explicitly into the event checking functions
 01/15/12 10:03 jec      started coding
*****************************************************************************/

#ifndef ES_CONFIGURE_H
#define ES_CONFIGURE_H

/****************************************************************************/
// The maximum number of services sets an upper bound on the number of
// services that the framework will handle. Reasonable values are 8 and 16
// corresponding to an 8-bit(uint8_t) and 16-bit(uint16_t) Ready variable size
#define MAX_NUM_SERVICES 16

/****************************************************************************/
// This macro determines that number of services that are *actually* used in
// a particular application. It will vary in value from 1 to MAX_NUM_SERVICES
#define NUM_SERVICES 9

/****************************************************************************/
// These are the definitions for Service 0, the lowest priority service.
// Every Events and Services application must have a Service 0. Further
// services are added in numeric sequence (1,2,3,...) with increasing
// priorities
// the header file with the public function prototypes
#define SERV_0_HEADER "OptoSensorService.h"//"AudioService.h"
// the name of the Init function
#define SERV_0_INIT InitOptoSensorService//InitAudioService
// the name of the run function
#define SERV_0_RUN RunOptoSensorService//RunAudioService
// How big should this services Queue be?
#define SERV_0_QUEUE_SIZE 5

/****************************************************************************/
// The following sections are used to define the parameters for each of the
// services. You only need to fill out as many as the number of services
// defined by NUM_SERVICES
/****************************************************************************/
// These are the definitions for Service 1
#if NUM_SERVICES > 1
// the header file with the public function prototypes
#define SERV_1_HEADER "GameService.h"
// the name of the Init function
#define SERV_1_INIT InitGameService
// the name of the run function
#define SERV_1_RUN RunGameService
// How big should this services Queue be?
#define SERV_1_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 2
#if NUM_SERVICES > 2
// the header file with the public function prototypes
#define SERV_2_HEADER "AudioService.h"
// the name of the Init function
#define SERV_2_INIT InitAudioService
// the name of the run function
#define SERV_2_RUN RunAudioService
// How big should this services Queue be?
#define SERV_2_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 3
#if NUM_SERVICES > 3
// the header file with the public function prototypes
#define SERV_3_HEADER "ServoService.h"
// the name of the Init function
#define SERV_3_INIT InitServoService
// the name of the run function
#define SERV_3_RUN RunServoService
// How big should this services Queue be?
#define SERV_3_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 4
#if NUM_SERVICES > 4
// the header file with the public function prototypes
#define SERV_4_HEADER "IRService.h"
// the name of the Init function
#define SERV_4_INIT InitIRService
// the name of the run function
#define SERV_4_RUN RunIRService
// How big should this services Queue be?
#define SERV_4_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 5
#if NUM_SERVICES > 5
// the header file with the public function prototypes
#define SERV_5_HEADER "DCMotorService.h"
// the name of the Init function
#define SERV_5_INIT InitDCMotorService
// the name of the run function
#define SERV_5_RUN RunDCMotorService
// How big should this services Queue be?
#define SERV_5_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 6
#if NUM_SERVICES > 6
// the header file with the public function prototypes
#define SERV_6_HEADER "ThrottleService.h"
// the name of the Init function
#define SERV_6_INIT InitThrottleService
// the name of the run function
#define SERV_6_RUN RunThrottleService
// How big should this services Queue be?
#define SERV_6_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 7
#if NUM_SERVICES > 7
// the header file with the public function prototypes
#define SERV_7_HEADER "LEDMissileService.h"
// the name of the Init function
#define SERV_7_INIT InitLEDMissileService
// the name of the run function
#define SERV_7_RUN RunLEDMissileService
// How big should this services Queue be?
#define SERV_7_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 8
#if NUM_SERVICES > 8
// the header file with the public function prototypes
#define SERV_8_HEADER "LEDFuelService.h"
// the name of the Init function
#define SERV_8_INIT InitLEDFuelService
// the name of the run function
#define SERV_8_RUN RunLEDFuelService
// How big should this services Queue be?
#define SERV_8_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 9
#if NUM_SERVICES > 9
// the header file with the public function prototypes
#define SERV_9_HEADER "TestHarnessService9.h"
// the name of the Init function
#define SERV_9_INIT InitTestHarnessService9
// the name of the run function
#define SERV_9_RUN RunTestHarnessService9
// How big should this services Queue be?
#define SERV_9_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 10
#if NUM_SERVICES > 10
// the header file with the public function prototypes
#define SERV_10_HEADER "TestHarnessService10.h"
// the name of the Init function
#define SERV_10_INIT InitTestHarnessService10
// the name of the run function
#define SERV_10_RUN RunTestHarnessService10
// How big should this services Queue be?
#define SERV_10_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 11
#if NUM_SERVICES > 11
// the header file with the public function prototypes
#define SERV_11_HEADER "TestHarnessService11.h"
// the name of the Init function
#define SERV_11_INIT InitTestHarnessService11
// the name of the run function
#define SERV_11_RUN RunTestHarnessService11
// How big should this services Queue be?
#define SERV_11_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 12
#if NUM_SERVICES > 12
// the header file with the public function prototypes
#define SERV_12_HEADER "TestHarnessService12.h"
// the name of the Init function
#define SERV_12_INIT InitTestHarnessService12
// the name of the run function
#define SERV_12_RUN RunTestHarnessService12
// How big should this services Queue be?
#define SERV_12_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 13
#if NUM_SERVICES > 13
// the header file with the public function prototypes
#define SERV_13_HEADER "TestHarnessService13.h"
// the name of the Init function
#define SERV_13_INIT InitTestHarnessService13
// the name of the run function
#define SERV_13_RUN RunTestHarnessService13
// How big should this services Queue be?
#define SERV_13_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 14
#if NUM_SERVICES > 14
// the header file with the public function prototypes
#define SERV_14_HEADER "TestHarnessService14.h"
// the name of the Init function
#define SERV_14_INIT InitTestHarnessService14
// the name of the run function
#define SERV_14_RUN RunTestHarnessService14
// How big should this services Queue be?
#define SERV_14_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 15
#if NUM_SERVICES > 15
// the header file with the public function prototypes
#define SERV_15_HEADER "TestHarnessService15.h"
// the name of the Init function
#define SERV_15_INIT InitTestHarnessService15
// the name of the run function
#define SERV_15_RUN RunTestHarnessService15
// How big should this services Queue be?
#define SERV_15_QUEUE_SIZE 3
#endif

/****************************************************************************/
// Name/define the events of interest
// Universal events occupy the lowest entries, followed by user-defined events
typedef enum
{
  ES_NO_EVENT = 0,
  ES_ERROR,                 /* used to indicate an error from the service */
  ES_INIT,                  /* used to transition from initial pseudo-state */
  ES_TIMEOUT,               /* signals that the timer has expired */
  ES_SHORT_TIMEOUT,         /* signals that a short timer has expired */
  /* User-defined events start here */
  ES_NEW_KEY,               /* signals a new key received from terminal */
  ES_LOCK,
  ES_UNLOCK,
  ES_HAND_DETECTED,
  ES_COUNTDOWN_TIMEOUT,
  ES_PROGRESS_DONE,
  ES_INACTIVITY_TIMEOUT,
  ES_FUEL_TIMEOUT,
  ES_FUEL_BAR_TIMEOUT,
  ES_MISSILE_HIT,
  ES_LOSE,
  ES_WIN,
  /* IR distance sensor events */
  IR_READ,
          IR_VALUE,
  /* DC Motor events */
  MOTOR_CMD,                /* Motor speed cmd */
  MOTOR_MAX,
  MOTOR_MIN,
          MOTOR_RESET,
  /*Reflective opto sensor events*/
          ROS_READ,
          ROS_RESET,
         THROTTLE_UPDATE,
          THROTTLE_VALUE,
          FIRE_MISSILE,
          UPDATEDISPLAY,
          WRITE2DISPLAY,
          FUEL_UPDATE,
          FUEL_DONE,
          ENCODER_UPDATE,
          SERVO_RESET,
          RESET_ALL,
          RESET
}ES_EventType_t;

/****************************************************************************/
// These are the definitions for the Distribution lists. Each definition
// should be a comma separated list of post functions to indicate which
// services are on that distribution list.
#define NUM_DIST_LISTS 0
#if NUM_DIST_LISTS > 0
#define DIST_LIST0 PostTestHarnessService0, PostTestHarnessService0
#endif
#if NUM_DIST_LISTS > 1
#define DIST_LIST1 PostTestHarnessService1, PostTestHarnessService1
#endif
#if NUM_DIST_LISTS > 2
#define DIST_LIST2 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 3
#define DIST_LIST3 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 4
#define DIST_LIST4 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 5
#define DIST_LIST5 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 6
#define DIST_LIST6 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 7
#define DIST_LIST7 PostTemplateFSM
#endif

/****************************************************************************/
// This is the list of event checking functions
#define EVENT_CHECK_LIST CheckEncoderEvents, Check4Keystroke

/****************************************************************************/
// These are the definitions for the post functions to be executed when the
// corresponding timer expires. All 16 must be defined. If you are not using
// a timer, then you should use TIMER_UNUSED
// Unlike services, any combination of timers may be used and there is no
// priority in servicing them
#define TIMER_UNUSED ((pPostFunc)0)
#define TIMER0_RESP_FUNC TIMER_UNUSED
#define TIMER1_RESP_FUNC TIMER_UNUSED
#define TIMER2_RESP_FUNC PostGameService
#define TIMER3_RESP_FUNC PostDCMotorService
#define TIMER4_RESP_FUNC PostDCMotorService
#define TIMER5_RESP_FUNC PostLEDMissileService
#define TIMER6_RESP_FUNC PostThrottleService
#define TIMER7_RESP_FUNC PostIRService
#define TIMER8_RESP_FUNC PostOptoSensorService
#define TIMER9_RESP_FUNC PostAudioService
#define TIMER10_RESP_FUNC PostGameService
#define TIMER11_RESP_FUNC PostGameService
#define TIMER12_RESP_FUNC PostGameService
#define TIMER13_RESP_FUNC PostGameService
#define TIMER14_RESP_FUNC PostGameService
#define TIMER15_RESP_FUNC PostServoService

/****************************************************************************/
// Give the timer numbers symbolic names to make it easier to move them
// to different timers if the need arises. Keep these definitions close to the
// definitions for the response functions to make it easier to check that
// the timer number matches where the timer event will be routed
// These symbolic names should be changed to be relevant to your application

#define SERVO_STEP_TIMER 15
#define COUNTDOWN_TIMER 14
#define PROGRESS_TIMER 13
#define INACTIVITY_TIMER 12
#define FUEL_TIMER 11
#define FUEL_BAR_TIMER 10
#define AUDIO_TIMER 9
#define IR_TIMER  7 //IR Sensor Service
#define OPTO_TIMER 8 // Opto Sensor Service
#define LED_MISSILE_TIMER 5
#define THROTTLE_TIMER 6
#define ENCODER_TIMER 4
#define MOTOR_RESET_TIMER 3
#define MISSILE_TIMER 2

#endif /* ES_CONFIGURE_H */
