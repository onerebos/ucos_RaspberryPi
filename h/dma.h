#ifndef _DMA_H_
#define _DMA_H_

#include "includes.h"

#define DMA_BASE            0x20007000
#define DMA_CHANL_DIST      0x100
#define DMA15_BASE          0x20E05000
#define DMA_CHANNEL_EN_OFFSET   0xFF0
typedef struct 
{
INT32U  TI;
INT32U  SrcAddr;
INT32U  DesAddr;
INT32U  TransLen;
INT32U  TwoDStride;
INT32U  NextCBAddr;
} DMA_t;

// DMA TransferInfo
#define     NO_WIDE_BURSTS  (1<<26)
#define     WAITS(a)        ((a)<<21)
#define     PERMAP(a)       ((a)<<16)
#define     BURST_LENGTH(a) ((a)<<12)
#define     SRC_IGNORE      (1<<11)
#define     SRC_DREQ        (1<<10)
#define     SRC_WIDTH       (1<<9)
#define     SRC_INC         (1<<8)
#define     DEST_IGNORE     (1<<7)
#define     DEST_DREQ       (1<<6)
#define     DEST_WIDTH      (1<<5)
#define     DEST_INC        (1<<4)
#define     WAIT_RESP       (1<<3)
#define     TDMODE          (1<<1)
#define     INTEN           1

// DMA Control and Status
#define DMA_CS_OFFSET   0

#define DMA_CS_INT      (2<<2)
#define DMA_CS_END      (1<<2)
#define DMA_CS_ACTIVE   1

// DMA Control Block Address
#define DMA_CB_ADDR_OFFSET  4




void dma_transfer(INT32U channel, DMA_t* dmaCb, INT32U contr_status );
void dma_init();

#endif
