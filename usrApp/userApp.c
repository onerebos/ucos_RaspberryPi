#include "uart.h"
#include "ucos/includes.h"
#include "fifo.h"

//#define TMP_BUFFER  0x00800

static INT8U buf[1024];
void userApp2(void * args)
{
    INT32U forNum = 0;
    INT32U rc = 0;
    Fifo_t fifo;
    INT32S sRc;

    INT8U sentence[] = "App2 Started\n\r";

    INT32U uart_dev = dev_open( "uart1", 0, NULL );  

    if( DEV_OPEN_FAIL == uart_dev )
    {
        showstop();
    }

    dev_write( uart_dev, sentence, sizeof( sentence ), 0, NULL ); 


    while(1)
	{
        rc = dev_read( uart_dev, buf, 1, 0, NULL );
        if( 1 == rc )
        {
            if( '@' == buf[0] )
            {
                extern int __bss_end;
                DisableInterrupts();
                sRc = xmodemTransmit( 0x0000,1024*150 );
                EnableInterrupts();
                if ( sRc < 0 )
                {
                    dev_write( uart_dev, 
                               "Trans Error \n", 
                               sizeof("Trans Error \n"),
                               0,
                               NULL );
                }
            }
            dev_write( uart_dev, buf, 1, 0, NULL );
        }
        OSTimeDly( 5 );
	}
}

void userApp1(void * args)
{

	while(1)
	{
		//uart_string("in userApp1");
        if( GET32(AUX_MU_IIR_REG) & ENABLE_RX_INTR )
        {
         //   uart_send( 'Y' );
        }
        else
        {
         //    uart_send( 'N' );
        }
        //uart_string_dma("!!!!!!!!");
        //EnableInterrupt( 29 );
        
		OSTimeDly(100);
	}
}
