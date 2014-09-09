#include "regs.h"
#include "interrupts.h"
#include "uart.h"
#include "dma.h"

extern INTERRUPT_VECTOR g_VectorTable[BCM2835_INTC_TOTAL_IRQ];
extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );


static OS_MEM * UartMemPartition;
// Struct holding tx/rx fifo
Fifo_t uartRecvFifo;
Fifo_t uartTransFifo;






unsigned int uart_lcr ( void )
{
    return(GET32(AUX_MU_LSR_REG));
}

unsigned int uart_recv ( void )
{
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x01) break;
    }
    return(GET32(AUX_MU_IO_REG)&0xFF);
}

void uart_send ( unsigned int c )
{
    //PUT32( AUX_MU_IER_REG , 1 );
	int i;
    while(1)
    {
        if(GET32(AUX_MU_LSR_REG)&0x20) break;
    }
    //for(i=0;i<0x7FF;i++);
    PUT32(AUX_MU_IO_REG,c);
}

void uart_flush ( void )
{
    while(1)
    {
        if((GET32(AUX_MU_LSR_REG)&0x100)==0) break;
    }
}

void hexstrings ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}

void hexstring_showstop ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}

void hexstring ( unsigned int d )
{
    return;
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}
static INT8U rByte;
static INT8U tByte;
static INT32U uartTransIntrFlag;

static INT32U counter = 0;
static void aux_uart_isr ( )
{
    DisableInterrupt(29);

    INT32U rb;
    INT32U rc; // Return code    

    PUT32( AUX_MU_IIR_REG, 1 );
    //uart_string("&");
    while(1) //resolve all interrupts to uart
    {
        rb = GET32(AUX_MU_IIR_REG);
        if((rb&1)==1) break; //no more interrupts

        if((rb&6)==4)// Receive
        {
            //uart_send( '&' );
            //receiver holds a valid byte
            rByte = (INT8U) (0xff & GET32(AUX_MU_IO_REG) ); //read byte from rx fifo
        
            fifoPut( &uartRecvFifo, &rByte , 1 );
        }
        //rb = GET32( AUX_MU_STAT_REG );

        if( (rb & 6) == 2 ) // Transmit Interrupt
        {

            //uart_send( '%' );
            
            rc = fifoGet( &uartTransFifo, &tByte, 1);
            if( 1 == rc )
            {
                PUT32( AUX_MU_IO_REG, (INT32U) tByte );
                uartTransIntrFlag = UART_TRANS_INTR_CONTINUE;
            }
            else
            {
                PUT32( AUX_MU_IER_REG, 1 ); // Disable 
                uartTransIntrFlag = UART_TRANS_INTR_END;
            }
            
        }
    
    }
    //uart_send('^');


    EnableInterrupt( 29 );

    //uart_string("&");
    //OSIntEnter();
    //GET32( AUX_MU_IO_REG );
    //uart_string(" RX INTR\n");
    //OSIntExit();
}
static INT8U UART_MEMORY_STATIC[2048];
void uart_init ( void )
{
    unsigned int ra;

    PUT32(AUX_ENABLES,1);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
    /////////////////////////////////////////////////////////
    // Here, when initializing, we can only enable recv intr.
    // As for transmit intr, when enable, it means that tx fifo is empty
    // and ready to receive data. We will enable when we plan to transmit]
    // data ( usually more than one byte.
    PUT32(AUX_MU_IER_REG,0|1);//ENABLE_RX_INTR);
    /////////////////////////////////////////////////////////
    PUT32(AUX_MU_IIR_REG,0xC6);
    PUT32(AUX_MU_BAUD_REG,270);
    ra=GET32(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    ra&=~(7<<15); //gpio15
    ra|=2<<15;    //alt5
    PUT32(GPFSEL1,ra);
    PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,(1<<14)|(1<<15));
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,0);
    PUT32(AUX_MU_CNTL_REG,3);

    uart_string("uart intr ENABLED\n");
    //FN_INTERRUPT_HANDLER fn = &aux_uart_isr;
    RegisterInterrupt( AUX_UART_INTR_NUM , aux_uart_isr ,0 );

	//g_VectorTable[29].pfnHandler = aux_uart_isr ;
    EnableInterrupt( AUX_UART_INTR_NUM );

    INT8U uartErr = 0;

    uart_string( "MEM" );
    hexstring( OSMemFreeList );
    hexstring(OS_MAX_MEM_PART);
    UartMemPartition = OSMemCreate( UART_MEMORY_STATIC, 
                                    UART_MEM_BLK_NUM, 
                                    UART_MEM_BLK_SIZE,
                                    &uartErr );
    if ( uartErr != 0 )
    {
        hexstring(uartErr);
        // Mem Partition Create
        showstop();
    }
    // Get mem for recv fifo
    INT8U *pUartRecvFifoMem = OSMemGet( UartMemPartition, &uartErr );
    if ( NULL == pUartRecvFifoMem )
    {
        showstop();
    }
    // Get mem for trans fifo
    INT8U *pUartTransFifoMem = OSMemGet( UartMemPartition, &uartErr );
    if ( NULL == pUartTransFifoMem )
    {
        showstop();
    }

    // Create FIFO data structures
    fifoCreate( &uartRecvFifo, pUartRecvFifoMem, UART_RECV_FIFO_SIZE );
    fifoCreate( &uartTransFifo, pUartTransFifoMem, UART_TRANS_FIFO_SIZE );

    uartTransIntrFlag = UART_TRANS_INTR_END;

    PUT32( AUX_ENABLES, 0 );// Disable uart function, until it is opened.

    // Register dev in the system
    static dev_t uart_dev;

    uart_dev.open = uart_open;
    uart_dev.close = uart_close;
    uart_dev.read = uart_read;
    uart_dev.write = uart_write;
    uart_dev.dev_name = "uart1";
    uart_dev.isOpen = FALSE;

    if( DEV_SUCCESS != dev_reg( uart_dev ) )
    {
        showstop();
    }
}


void uart_string_showstop (char* s)
{
//    return;
    // cpu Version
    
	while(*s!=0)
    {
    	uart_send((unsigned int)*s);
    	s++;
    }
	uart_send(0x0D);
	uart_send(0x0A);
    
}
void uart_string (char* s)
{
    return;
    // cpu Version
    
	while(*s!=0)
    {
    	uart_send((unsigned int)*s);
    	s++;
    }
	uart_send(0x0D);
	uart_send(0x0A);
    
}

static DMA_t dma_channel_reg = 
{
    SRC_INC|DEST_INC,0xc0001000,0xc0002000,256,0,0
    //SRC_INC|0xb0000|DEST_DREQ,0x00001000,0x7E215040,0x100,0,0
};

void uart_string_dma(char* s)
{
    dma_init();
    uart_send('!');
    //DMA Version
    //static DMA_t    dma_channel_reg;
#define DMA_CB_ADDR     0x40000000
    DMA_t * uncacheReg = (DMA_CB_ADDR);
    //DMA_t * uncacheReg = (0xc0000000 + (INT32U)&dma_channel_reg);
    INT32U cnt_status;
    memset(0x40011000, '%', 0x100);
    memset(0x00012000,0x00, 8);
    //uncacheReg->TI = SRC_INC | DEST_DREQ | PERMAP(14) | INTEN  ;
    uncacheReg->TI = SRC_INC | PERMAP(14) | WAITS(0x1f)  ;
    //uncacheReg->TI = SRC_INC| DEST_INC ;
    uncacheReg->SrcAddr = 0x40011000;
    //uncacheReg->SrcAddr = (INT32U)s;
    //uncacheReg->DesAddr = 0x00012000;
    uncacheReg->DesAddr = 0x7E215040;// AUX_MU_IO_REG;
    uncacheReg->TransLen = 0x00f0;//strlen( s );
    uncacheReg->TwoDStride = 0;
    uncacheReg->NextCBAddr = 0;

    /*
    dma_channel_reg.TI = SRC_INC | DEST_DREQ | PERMAP(14) | INTEN  ;
    dma_channel_reg.SrcAddr = (INT32U)s;
    dma_channel_reg.DesAddr = (AUX_MU_IO_REG);
    dma_channel_reg.TransLen = 4;//strlen( s );
    dma_channel_reg.TwoDStride = 0;
    dma_channel_reg.NextCBAddr = 0;
    */
    cnt_status = 1;

    uart_string("CB:\n");
    /*
    hexstring(dma_channel_reg.TI);
    hexstring(dma_channel_reg.SrcAddr);
    hexstring(dma_channel_reg.DesAddr);
    hexstring(dma_channel_reg.TransLen);
    hexstring(dma_channel_reg.TwoDStride);
    hexstring(dma_channel_reg.NextCBAddr);
    */
    hexstring(((DMA_t*)(DMA_CB_ADDR))->TI);
    hexstring(((DMA_t*)(DMA_CB_ADDR))->SrcAddr);
    hexstring(((DMA_t*)(DMA_CB_ADDR ))->DesAddr);
    hexstring(((DMA_t*)(DMA_CB_ADDR))->TransLen);
    hexstring(((DMA_t*)(DMA_CB_ADDR))->TwoDStride);
    hexstring(((DMA_t*)(DMA_CB_ADDR))->NextCBAddr);
 

    dma_transfer( 0, DMA_CB_ADDR, cnt_status );
    //dma_transfer( 0, 0xc0000000 + (INT32U)&dma_channel_reg, cnt_status );
}


INT32U uart_open( INT32U param, void* pParam )
{
    //RegisterInterrupt( AUX_UART_INTR_NUM , aux_uart_isr ,0 );
    EnableInterrupt( AUX_UART_INTR_NUM );
    PUT32( AUX_ENABLES, 1 );
    fifoEmpty( &uartRecvFifo );
    fifoEmpty( &uartTransFifo );

    return DEV_SUCCESS;
}

INT32U uart_close( void )
{
    // Disable Interrupt
    DisableInterrupt( AUX_UART_INTR_NUM );
    PUT32( AUX_ENABLES, 0 );
    fifoEmpty( &uartRecvFifo );
    fifoEmpty( &uartTransFifo );

    return DEV_SUCCESS;

}

INT32U uart_read ( INT8U * recBuf, 
                   INT32U num,
                   INT32U param,
                   void* pParam )
{
    INT32U recNum;
    DisableInterrupt(29);
    recNum = fifoGet(&uartRecvFifo, recBuf, num );
    EnableInterrupt(29);
    return recNum;
}

INT32U uartRecv( INT32U uartDev, 
                 INT8U * recBuf, 
                 INT32U num,
                 INT32U timeout )
{
    INT32U recNum;
    DisableInterrupt(29);
    recNum = fifoGet(&uartRecvFifo, recBuf, num );
    EnableInterrupt(29);
    return recNum;
}

INT32U uart_write (  INT8U * transBuf, 
                     INT32U transNum, 
                     INT32U param,
                     void* pParam)
{
    INT32U rc = 0;
    INT32U RTC = 0;
    INT8U rByte = 0;
    
    if( uartTransIntrFlag == UART_TRANS_INTR_CONTINUE )
    {
        rc = fifoPut( &uartTransFifo, transBuf, transNum );
       
        RTC = rc;
    }
    else if( uartTransIntrFlag == UART_TRANS_INTR_END )
    {
        //uart_string("FIFO Put");
        rc = fifoPut( &uartTransFifo, transBuf, transNum );
        
        if( 0 == rc )
        {
            RTC = 0;
        }
        else
        {
            uartTransIntrFlag = UART_TRANS_INTR_CONTINUE;
            fifoGet( &uartTransFifo, &rByte, 1 );
            while(1)
            {
                if(GET32(AUX_MU_LSR_REG)&0x20) break;
            }
            PUT32( AUX_MU_IO_REG, (INT32U) 0xff&rByte );
            PUT32( AUX_MU_IER_REG, 3 );// Enable transmit Interrupt
            RTC = transNum;
        }
    }
    else
    {
        showstop();
    }

    return RTC;
}
INT32U uartTransmit( INT32U uartDev, 
                     INT8U * transBuf, 
                     INT32U transNum, 
                     INT32U timeout )
{
    INT32U rc = 0;
    INT32U RTC = 0;
    INT8U rByte = 0;
    
    if( uartTransIntrFlag == UART_TRANS_INTR_CONTINUE )
    {
        rc = fifoPut( &uartTransFifo, transBuf, transNum );
       
        RTC = rc;
    }
    else if( uartTransIntrFlag == UART_TRANS_INTR_END )
    {
        //uart_string("FIFO Put");
        rc = fifoPut( &uartTransFifo, transBuf, transNum );
        
        if( 0 == rc )
        {
            RTC = 0;
        }
        else
        {
            uartTransIntrFlag = UART_TRANS_INTR_CONTINUE;
            fifoGet( &uartTransFifo, &rByte, 1 );
            while(1)
            {
                if(GET32(AUX_MU_LSR_REG)&0x20) break;
            }
            PUT32( AUX_MU_IO_REG, (INT32U) 0xff&rByte );
            PUT32( AUX_MU_IER_REG, 3 );// Enable transmit Interrupt
            RTC = transNum;
        }
    }
    else
    {
        showstop();
    }

    return RTC;
}


