#include "cpu.h"
#include "uart.h"
/*
reset
undefined_instruction
swi
prefetch_abort
data_abort
unused
OS_CPU_IRQ_ISR;@irqHandler to be modified
fiq
*/
void unused ( void )
{
    uart_string("unused");

    while(1);
}

void fiq( void )
{
    uart_string( "fiq" );

    while(1);
}

void undefined_instruction( void )
{
    uart_string("undefined_instruction");
    
    while(1);
}

void swi( void )
{
    uart_string( "swi" );

    while(1);
}

void prefetch_abort( void )
{
    uart_string( "prefetch_abort" );

    while(1);
}

void data_abort( void )
{
    uart_string(" data_abort " );

    while(1);
}



