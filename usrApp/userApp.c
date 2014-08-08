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
    //INT8U buf[1024];
    //fifoCreate( &fifo, TMP_BUFFER, 1023 );
    
    INT8U sentence[] = "in userApp2";
    
	while(1)
	{
        /*
        // Test FIFO function
        while( forNum-- > 0 )
        {
            fifoPut( &fifo, sentence, sizeof("in userApp2") );
        }
        forNum = 0;

        uart_string( "Fifo Put Done." );
        while( forNum-- > 0 )
        {
            fifoGet( &fifo, buf, sizeof("in userApp2") );
        }
        forNum = 0;
        uart_string( "Fifo Get Done." );
	    buf[ sizeof("in userApp2") ] = 0;	
        uart_string( buf );
        */
        // Test uart rx driver 
        //rc = uartRecv( 0, buf, 1, 0 );
        
        //hexstring( rc );
        //if( rc > 0 ) uart_send(buf[0]);
        while( 100 == uartRecv(0, buf, 100, 0) )
        {
            uartTransmit( 0, buf, 100, 0 );
        }
        //PUT32( AUX_MU_IO_REG, '*' );

        //PUT32( AUX_MU_IER_REG , 7 );
        //uart_send('.');
        //PUT32( AUX_MU_IER_REG, 3 );
		//OSTimeDly(5);
        //uartTransmit( 0, "Hello World!\n\r", sizeof("hello world!\n\r"), 0 );
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
