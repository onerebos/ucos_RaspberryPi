#include "showstop.h"

void showstop( INT32U ssCode )
{
    // In showstop, need to disable irq as FW meets 
    // fatal error, and will be reset.

    DisableInterrupts();

    // Tell minicom that we are in showstop
    uart_string_showstop("showstop");

    // Store all current regs to debug register 

    // Collecting data and copy it to buffer
      
    // Starting transmiting to minicom 
    //xmodemTransmit();

    while(1);

}
