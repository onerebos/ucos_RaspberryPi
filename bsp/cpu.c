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
    uart_string_showstop("unused");
    showstop();
    while(1);
}

void fiq( void )
{
    uart_string_showstop( "fiq" );
    showstop();
    while(1);
}

void undefined_instruction( void )
{
    uart_string_showstop("undefined_instruction");
    showstop();
    while(1);
}

void swi( void )
{
    uart_string_showstop( "swi" );
    showstop();
    while(1);
}

void prefetch_abort( void )
{
    uart_string_showstop( "prefetch_abort" );
    showstop();
    while(1);
}

void data_abort( void )
{
    uart_string_showstop(" data_abort " );
    showstop();
    while(1);
}



