#include "includes.h"

#ifndef _XMODEM_H_
#define _XMODEM_H_

typedef INT8S int8_t; 
typedef INT8U uint8_t;
typedef INT16S int16_t;
typedef INT16U uint16_t; 


int8_t xmodemTransmit(uint8_t *src, uint16_t src_size);
int8_t xmodemReceive(uint8_t *dest, uint16_t dest_size);
