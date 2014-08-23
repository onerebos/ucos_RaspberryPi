#include "gpio.h"

void SetGPIOFunction(INT32U GPIO, INT32U functionCode)
{
    INT32U registerIndex = GPIO / 10;
    INT32U bit = (GPIO % 10) * 3;

    INT32U *m_pAltFunctionRegisters = 0x20200000;

    INT32U oldValue = m_pAltFunctionRegisters[registerIndex];
    INT32U mask = 0b111 << bit;
    m_pAltFunctionRegisters[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}


void initJTAG( void )
{

    SetGPIOFunction(22, GPIO_ALT_FUNCTION_4);
    SetGPIOFunction(23, GPIO_ALT_FUNCTION_4);
    SetGPIOFunction(24, GPIO_ALT_FUNCTION_4);
    SetGPIOFunction(25, GPIO_ALT_FUNCTION_4);
    SetGPIOFunction(26, GPIO_ALT_FUNCTION_4);
    SetGPIOFunction(27, GPIO_ALT_FUNCTION_4);

}
