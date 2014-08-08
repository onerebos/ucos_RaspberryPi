#include "fifo.h"
#include "includes.h"

Fifo_t * fifoCreate( Fifo_t* pFifo, INT8U * pMem, INT32U size )
{
    hexstring( pMem );
    pFifo->pFifo = pMem;
    pFifo->head = 0;
    pFifo->tail = 0;
    pFifo->size = size - 1;

    return pFifo;
}

INT32U fifoGet( Fifo_t* pFifo, INT8U * pMem, INT32U num )
{
    INT32U i;
    // Get data num in fifo
    INT32U dataNum = pFifo->head + pFifo->size + 1 - pFifo->tail;
    dataNum %= (pFifo->size + 1);

    //hexstring( dataNum );

    if( dataNum >= num )
    {
        for( i = 0; i < num ; i ++ )
        {
            pMem[i] = pFifo->pFifo[pFifo->tail];
            (pFifo->tail)++;
            pFifo->tail %= ( pFifo->size + 1 );
        }
        dataNum = pFifo->head + pFifo->size + 1 - pFifo->tail;
        dataNum %= (pFifo->size + 1);
     //   hexstring( dataNum );

        return num;
    }    
    else
    {
        return 0;
    }
    
}
    
INT32U fifoPut( Fifo_t* pFifo, INT8U* pMem, INT32U num )
{
    INT32U i;
    // get empty data num
    INT32U slotNum = pFifo->head + pFifo->size + 1 - pFifo->tail;
    slotNum %= (pFifo->size + 1);
    slotNum = pFifo->size - slotNum;


        //hexstring( GET_SP() ); 
    //hexstring( slotNum );

    if( slotNum < num )
    {
        return 0;
    }
    else
    {
        for( i = 0; i < num; i++ )
        {
            //hexstring( pFifo->pFifo );
            //hexstring( &(pFifo->pFifo[pFifo->head] ) );
            pFifo->pFifo[ pFifo->head ] = pMem[i];
            pFifo->head++;
            pFifo->head %= pFifo->size + 1;
        }
        slotNum = pFifo->head + pFifo->size + 1 - pFifo->tail;
        slotNum %= (pFifo->size + 1);
        slotNum = pFifo->size - slotNum;
     //   hexstring( slotNum );
        return num;
    }
}

void fifoEmpty( Fifo_t* pFifo )
{
    pFifo->tail = pFifo->head;
}
