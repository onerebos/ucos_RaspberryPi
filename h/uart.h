
#ifndef UART_H_
#define UART_H_

#include "includes.h"
#include "regs.h"
#include "fifo.h"

//AUX_MU_IIR_REG
#define     ENABLE_RX_INTR      (1<<1)
#define     ENABLE_TX_INTR      (1<<0)

#define     AUX_UART_INTR_NUM   (29)


//UART FIFO
#define UART_RECV_FIFO_SIZE     (1024)
#define UART_TRANS_FIFO_SIZE    (1024)

//#define UART_MEMORY         0x800000
#define UART_MEMORY_SIZE    0x800
#define UART_MEM_BLK_SIZE   0x4000
#define UART_MEM_BLK_NUM    2

#define UART_TRANS_INTR_CONTINUE    1
#define UART_TRANS_INTR_END         0

unsigned int uart_lcr ( void );
unsigned int uart_recv ( void );
void uart_send ( unsigned int );
void uart_flush ( void );
void hexstrings ( unsigned int );
void hexstring ( unsigned int );
void uart_init ( void );
void uart_string(char *);

void uart_string_dma(char* s);


// used in showstop only
void uart_string_showstop (char* s);
void hexstring_showstop ( unsigned int d );

typedef struct
{
    INT32U uart_all_reg[12];
} uart_reg_t;
//void aux_uart_isr( );
#endif /* UART_H_ */
