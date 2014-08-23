#include "gpio.h"
#include "uart.h"
#include "regs.h"
#include "timer.h"
#include "ucos/includes.h"

OS_STK  userAppTaskStk1[4000];
OS_STK  userAppTaskStk2[4000];

extern void userApp1(void *);
extern void userApp2(void *);

void main()
{

    initJTAG( );
	InitInterruptController();

	//uart_init();
	
    DisableInterrupts();

	timer_init();

	OSInit();

	uart_init();

	OSTaskCreate(userApp1, (void *) 0, &userAppTaskStk1[4000-1],5);

	OSTaskCreate(userApp2, (void *) 0, &userAppTaskStk2[4000-1],6);

	OSStart();

	while(1);
}
