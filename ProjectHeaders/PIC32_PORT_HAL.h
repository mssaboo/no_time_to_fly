#ifndef PIC32PortHAL
#define PIC32PortHAL
#endif

#include <xc.h>
#include <stdbool.h>

/****************************************************************************
This enum defines a data type for the possible ports to be configured.
Note: explicit assignment used to eliminate any possible compiler dependency
****************************************************************************/
typedef enum
{
  _Port_A = 0,
  _Port_B = 1,
}PortSetup_Port_t;

/****************************************************************************
This enum defines a data type for the possible pins to be configured.
the values are a constant with a single '1' in the chosen bit position 
****************************************************************************/
typedef enum
{
  _Pin_0 = (1<<0),
  _Pin_1 = (1<<1),
  _Pin_2 = (1<<2),
  _Pin_3 = (1<<3),
  _Pin_4 = (1<<4),
  _Pin_5 = (1<<5),
  _Pin_6 = (1<<6),
  _Pin_7 = (1<<7),
  _Pin_8 = (1<<8),
  _Pin_9 = (1<<9),
  _Pin_10 = (1<<10),
  _Pin_11 = (1<<11),
  _Pin_12 = (1<<12),
  _Pin_13 = (1<<13),
  _Pin_14 = (1<<14),
  _Pin_15 = (1<<15),
}PortSetup_Pin_t;

bool PortSetup_ConfigureDigitalInputs( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);
bool PortSetup_ConfigureDigitalOutputs( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);
bool PortSetup_ConfigureAnalogInputs( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);
bool PortSetup_ConfigurePullUps( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);
bool PortSetup_ConfigurePullDowns( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);
bool PortSetup_ConfigureOpenDrain( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);
bool PortSetup_ConfigureChangeNotification( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin);