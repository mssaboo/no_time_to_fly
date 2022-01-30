#include <xc.h>
#include <stdbool.h>
#include "PIC32_PORT_HAL.h"


/****************************************************************************
 Function
    PortSetup_ConfigureDigitalInputs

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin to be configured as digital inputs

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port as digital inputs, disabling analog input(s).
Example
   PortSetup_ConfigureDigitalInputs(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigureDigitalInputs( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        uint32_t bitmask = 0xFFFFFFE0;//11111111111111111111111111100000
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ANSELA &= ~WhichPin;//ANSELx bits cleared 
            TRISA |= WhichPin;//TRISx bits set
            return true;
        }
    }
    if(WhichPort == _Port_B){
        uint32_t bitmask = 0xFFFF0000;//11111111111111110000000000000000
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ANSELB &= ~WhichPin;
            TRISB |= WhichPin;
            return true;
        }
    }
    return false;
} 
/****************************************************************************
 Function
    PortSetup_ConfigureDigitalOutputs

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin(s) to be configured as digital outputs

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port as digital outputs
Example
   PortSetup_ConfigureDigitalOutputs(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigureDigitalOutputs( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        uint32_t bitmask = 0xFFFFFFE0;//11111111111111111111111111100000
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ANSELA &= ~WhichPin;//ANSELx bits cleared
            TRISA &= ~WhichPin;//TRISx bits cleared
            return true;
        }
    }
    if(WhichPort == _Port_B){
        uint32_t bitmask = 0xFFFF0000;//11111111111111110000000000000000
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ANSELB &= ~WhichPin;
            TRISB &= ~WhichPin;
            return true;
        }
    }
    return false;
}

/****************************************************************************
 Function
    PortSetup_ConfigureAnalogInputs

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin(s) to be configured as analog inputs

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port as analog inputs
Example
   PortSetup_ConfigureAnalogInputs(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigureAnalogInputs( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        PortSetup_Pin_t bitmask = 0x0000001C;//11100
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ANSELA |= WhichPin;//ANSELx bits set
            TRISA |= WhichPin;//TRISx bits set
            return true;
        }
    }
    if(WhichPort == _Port_B){
        PortSetup_Pin_t bitmask = 0x00000FF0;//111111110000
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ANSELB |= WhichPin;
            TRISB |= WhichPin;
            return true;
        }
    }
    return false;    
}

/****************************************************************************
 Function
    PortSetup_ConfigurePullUps

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin(s) to be configured with weak pull-ups

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port with weak pull-ups
Example
   PortSetup_ConfigurePullUps(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigurePullUps( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        PortSetup_Pin_t bitmask = 0xFFFFFFE0;
        if(bitmask&WhichPin!=0)
            return false;
        else{
            CNPUA |= WhichPin;//CNPUx bits set
            return true;
        }
    }
    if(WhichPort == _Port_B){
        PortSetup_Pin_t bitmask = 0xFFFF0000;
        if(bitmask&WhichPin!=0)
            return false;
        else{
            CNPUB |= WhichPin;
            return true;
        }
    }
    return false;
}
/****************************************************************************
 Function
    PortSetup_ConfigurePullDowns

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin(s) to be configured with weak pull-downs

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port with weak pull-downs. 
Example
   PortSetup_ConfigurePullDowns(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigurePullDowns( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        PortSetup_Pin_t bitmask = 0xFFFFFFE0;
        if(bitmask&WhichPin!=0)
            return false;
        else{
            CNPDA |= WhichPin;//CNPDx bits set
            return true;
        }
    }
    if(WhichPort == _Port_B){
        PortSetup_Pin_t bitmask = 0xFFFF0000;
        if(bitmask&WhichPin!=0)
            return false;
        else{
            CNPDB |= WhichPin;
            return true;
        }
    }
    return false;
}
/****************************************************************************
 Function
    PortSetup_ConfigureOpenDrain

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin(s) to be configured as open drain outputs

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port as open drain outputs
Example
   PortSetup_ConfigureOpenDrain(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigureOpenDrain( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        PortSetup_Pin_t bitmask = 0xFFFFFFE0;
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ODCA |= WhichPin;//ODCx bits set
            return true;
        }
    }
    if(WhichPort == _Port_B){
        PortSetup_Pin_t bitmask = 0xFFFF0000;
        if(bitmask&WhichPin!=0)
            return false;
        else{
            ODCB |= WhichPin;
            return true;
        }
    }
    return false;    
}

/****************************************************************************
 Function
    PortSetup_ConfigureChangeNotification

 Parameters
   PortSetup_Port_t: the port to be configured
   PortSetup_Pin_t: the pin(s) to be enabled for change notification

 Returns
   bool: true if port and pins represent legal ports and pins; otherwise, false

 Description
   Configures the specified pin(s) on the specified port to enable change notifications. If any bits are set in the PortSetup_Pin_t parameter, then change notifications are enabled. If that parameter is 0, then change notifications are disabled globally.
Example
   PortSetup_ConfigureChangeNotification(_Port_A, _Pin_0 | _Pin_1);
****************************************************************************/

bool PortSetup_ConfigureChangeNotification( PortSetup_Port_t WhichPort, PortSetup_Pin_t WhichPin){
    if(WhichPort == _Port_A){
        PortSetup_Pin_t bitmask = 0xFFFFFFE0;
        if(WhichPin == 0){
            CNCONAbits.ON = 0;
            return true;
        }
        if(bitmask&WhichPin!=0)
            return false;
        else{
            CNCONAbits.ON = 1;//CNCONx bits set
            CNENA |= WhichPin;//CNENx bits set
            return true;
        }
    }
    if(WhichPort == _Port_B){
        PortSetup_Pin_t bitmask = 0xFFFF0000;
        if(WhichPin == 0){
            CNCONBbits.ON = 0;
            return true;
        }
        if(bitmask&WhichPin!=0)
            return false;
        else{
            CNCONBbits.ON = 1;
            CNENB |= WhichPin;
            return true;
        }
    }
    return false;        
}

