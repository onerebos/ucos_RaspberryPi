#ifndef _GPIO_H_
#define _GPIO_H_

#include "includes.h"


#define GPIO_ALT_FUNCTION_0 0b100
#define GPIO_ALT_FUNCTION_1 0b101
#define GPIO_ALT_FUNCTION_2 0b110
#define GPIO_ALT_FUNCTION_3 0b111
#define GPIO_ALT_FUNCTION_4 0b011
#define GPIO_ALT_FUNCTION_5 0b010



void SetGPIOFunction(INT32U GPIO, INT32U functionCode);
void initJTAG( void );

#endif
