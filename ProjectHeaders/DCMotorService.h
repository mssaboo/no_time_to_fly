/****************************************************************************

 Description
 This is a DC Motor Service set up to work with the Lego NXT gear motor with 
 encoder. The encoder has 360 counts per revolution.


 ****************************************************************************/

#ifndef DCMotorService_H
#define DCMotorService_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum
{
  Init
} TemplateState_t;

// Public Function Prototypes

bool InitDCMotorService(uint8_t Priority);
bool PostDCMotorService(ES_Event_t ThisEvent);
ES_Event_t RunDCMotorService(ES_Event_t ThisEvent);

//Function to convert encoder counts to angle
uint16_t GetAngleDeg(void);

//Event Checkers
bool CheckEncoderEvents(void);
bool CheckCountLimits(void);

#endif /* DCMotorService_H */
