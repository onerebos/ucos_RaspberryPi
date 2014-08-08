#include "regs.h"
#include "dma.h"
#include "includes.h"

static inline void dcache_clean(void)
{
    const int zero = 0;
    /* clean entire D cache -> push to external memory. */
    __asm volatile ("1: mrc p15, 0, r15, c7, c10, 3\n"
                        " bne 1b\n" ::: "cc");
    /* drain the write buffer */
    __asm volatile ("mcr 15, 0, %0, c7, c10, 4"::"r" (zero));
}

void dma_transfer(INT32U channel, DMA_t* dmaCb, INT32U contr_status )
{
    char buf[100];
    //dcache_clean();
    dma_init();
    //memset(0xc0001000, 0xff, 8);
    //memset(0xc0002000,0x01, 8);

    //uart_string("\nBefore:");
    //hexstring( GET32(DMA_BASE+0x14) );
    // Clear dma stop flat
    //PUT32( DMA_BASE, 0x80000002 );
    OSTimeDly(5);
    PUT32( DMA_BASE + channel*DMA_CHANL_DIST + 4 , (INT32U)dmaCb);
    PUT32( DMA_BASE + channel*DMA_CHANL_DIST , contr_status );

    // DMA started

    //uart_string("\nAfter:");
    hexstring( GET32(DMA_BASE+0x14) );
    while(1)
    {
        if ( ( (GET32( DMA_BASE ) & 2) == 0x2 ) ||
             ( (GET32( DMA_BASE ) & 1) == 0 ) )
        {
            uart_string("DMA Done\n");
            PUT32( DMA_BASE, GET32(DMA_BASE)|2 ); //clear done bit
            hexstring(GET32( DMA_BASE + 0x14) );
            hexstring(GET32(0x00012000));
            hexstring(GET32( DMA_BASE ));
            break;
        }
        OSTimeDly(50);
        if( GET32( DMA_BASE ) & (1<<8) )
        {
            //uart_send('E');
        }
        else
        {
            //hexstring(GET32( DMA_BASE ));
            //uart_send('.');
        }
        if( GET32(DMA_BASE + 0x14) == 0x100 )
        {
            //uart_send('F');
            //hexstring( GET32(DMA_BASE) );
        }
        else
        {
            //hexstring( GET32(DMA_BASE+0x14) );
        }
    }
}

void dma_isr( void )
{
    //uart_send('@');

    // CLear DMA interrput end and DMA end bits in CS reg
    PUT32(DMA_BASE+0*DMA_CHANL_DIST+DMA_CS_OFFSET, DMA_CS_INT|DMA_CS_END);
}

void dma_init( void )
{
    PUT32( DMA_BASE + DMA_CHANNEL_EN_OFFSET, 1 );
   // RegisterInterrupt( dma_i
}

