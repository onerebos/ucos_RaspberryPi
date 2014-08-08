#ifndef _FIFO_H_
#define _FIFO_H_
#include "includes.h"

typedef struct 
{
    INT8U* pFifo;
    INT32U head;
    INT32U tail;
    INT32U size;
} Fifo_t;


Fifo_t* fifoCreate( Fifo_t* pFifo, INT8U * pMem, INT32U size );
INT32U fifoGet( Fifo_t* pFifo, INT8U * pMem, INT32U num );
INT32U fifoPut( Fifo_t* pFifo, INT8U* pMem, INT32U num );
void fifoEmpty( Fifo_t* pFifo );

#endif
