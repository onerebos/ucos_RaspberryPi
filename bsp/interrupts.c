#include "interrupts.h"
#include "uart.h"
#include "regs.h"

INTERRUPT_VECTOR g_VectorTable[BCM2835_INTC_TOTAL_IRQ];

/**
 *	Enables all IRQ's in the CPU's CPSR register.
 **/
static void irqEnable() {
	__asm volatile("mrs 	r0,cpsr");		// Read in the cpsr register.
	__asm volatile("bic		r0,r0,#0x80");	// Clear bit 8, (0x80) -- Causes IRQs to be enabled.
	__asm volatile("msr		cpsr_c, r0");	// Write it back to the CPSR register
}

static void irqDisable() {
	__asm volatile("mrs		r0,cpsr");		// Read in the cpsr register.
	__asm volatile("orr		r0,r0,#0x80");	// Set bit 8, (0x80) -- Causes IRQs to be disabled.
	__asm volatile("msr		cpsr_c, r0");	// Write it back to the CPSR register.

}

#define clz(a) \
 ({ unsigned long __value, __arg = (a); \
     asm ("clz\t%0, %1": "=r" (__value): "r" (__arg)); \
     __value; })


/**
 *	This is the global IRQ handler  on this platform!
 *	It is based on the assembler code found in the Broadcom datasheet.
 *
 **/
void OS_CPU_IRQ_ISR_Handler() {
	/**
	 *	WARNING:
	 *		Compiler optimisations will likely prevent this code from working!
	 *		
	 *	This will be fixed later!
	 **/
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
#endif    
 
    OS_ENTER_CRITICAL();
    OSIntEnter();
    OS_ENTER_CRITICAL();

	static volatile unsigned long ulMaskedStatus;
	register unsigned long irqNumber;
	static volatile unsigned long tmp;
    register INT32U IntrPendingBits;
    //uart_send('.');

    /*
    if ( intcRegs->Pending1 & (1<<29) )
    {
        //uart_send('#');
        irqNumber = 29;
    	if(g_VectorTable[irqNumber].pfnHandler) 
        {
            //g_VectorTable[irqNumber].pfnHandler = aux_uart_isr;
            //DisableInterrupt(29);
            //uart_string("!");
		    (g_VectorTable[irqNumber].pfnHandler)(irqNumber, g_VectorTable[irqNumber].pParam);
            //aux_uart_isr(0,0);
        }
        OSIntExit();
        return;
    }
*/
    ulMaskedStatus = intcRegs->IRQBasic;
	tmp = ulMaskedStatus & 0x00000300;			// Check if anything pending in pr1/pr2.   

	if(ulMaskedStatus & ~0xFFFFF300) {			// Note how we mask out the GPU interrupt Aliases.
	    IntrPendingBits = ulMaskedStatus;       // Make emit_interrupt has the same logic	
        irqNumber = 64 + 31;						// Shifting the basic ARM IRQs to be IRQ# 64 +
		goto emit_interrupt;
	}

	if(tmp & 0x100) {
		IntrPendingBits = intcRegs->Pending1;
		irqNumber = 0 + 31;
		// Clear the interrupts also available in basic IRQ pending reg.
		//ulMaskedStatus &= ~((1 << 7) | (1 << 9) | (1 << 10) | (1 << 18) | (1 << 19));
		if( IntrPendingBits ) {
			goto emit_interrupt;
		}
	}

	if(tmp & 0x200) {
		IntrPendingBits = intcRegs->Pending2;
		irqNumber = 32 + 31;
		// Don't clear the interrupts in the basic pending, simply allow them to processed here!
		if( IntrPendingBits ) {
			goto emit_interrupt;
		}				
	}

    if ( ~0xFFA00000 & ulMaskedStatus )
    {
        uartTransmit( 0, "GPU IRQ\n\r", sizeof("GPU IRQ\n\r"), 0 );
    }
    else if ( 0xFFA00000 & ulMaskedStatus )
    {
        uartTransmit( 0 , "Unused IRQ\n\r", sizeof("Unused IRQ\n\r" ) , 0 );
    }
    else
    {
        uartTransmit( 0 , "Empty IRQ\n\r", sizeof("Empty IRQ\n\r"), 0 );
    }
/*
    DisableInterrupts();// For uart print
    hexstring_showstop( ulMaskedStatus );
    hexstring_showstop( tmp ); 
    showstop();
    uart_send('?');
*/ 
    OSIntExit();
	return;

emit_interrupt:

    //hexstring( GET_SP() ); 
    //hexstring( GET_PC() );
	tmp = IntrPendingBits- 1;
	IntrPendingBits = IntrPendingBits ^ tmp;

	unsigned long lz = clz(IntrPendingBits);

	//irqNumber = irqNumber - 

	//__asm volatile("clz	r7,r5");				// r5 is the ulMaskedStatus register. Leaving result in r6!
	//__asm volatile("sub r6,r7");

/*
	if(g_VectorTable[irqNumber-lz].pfnHandler) {
		g_VectorTable[irqNumber-lz].pfnHandler(irqNumber, g_VectorTable[irqNumber].pParam);
	}
*/
    if(g_VectorTable[irqNumber-lz].pfnHandler) {
		g_VectorTable[irqNumber-lz].pfnHandler(irqNumber, g_VectorTable[irqNumber].pParam);
	}
    else
    {
        uartTransmit( 0 , "Undefined IRQ Func" , sizeof("Undefined IRQ Func"), 0 );
    }
    OSIntExit();
}


static void stubHandler(int nIRQ, void *pParam) {
	/**
	 *	Actually if we get here, we should probably disable the IRQ,
	 *	otherwise we could lock up this system, as there is nothing to 
	 *	ackknowledge the interrupt.
	 **/  
    uart_string("StubHandle");
}

int InitInterruptController() {
	int i;
	for(i = 0; i < BCM2835_INTC_TOTAL_IRQ; i++) {
		g_VectorTable[i].pfnHandler 	= stubHandler;
		g_VectorTable[i].pParam			= (void *) 0;
	}
	return 0;
}



int RegisterInterrupt(int nIRQ, int(*pfnHandler)(int, void*), void *pParam) {

	irqDisable();
	{
        uart_string("ISR Registed\n");
		g_VectorTable[nIRQ].pfnHandler = pfnHandler;
		g_VectorTable[nIRQ].pParam		= pParam;
	}
	irqEnable();
	return 0;
}

int EnableInterrupt(int nIRQ) {

	unsigned long	ulTMP;

	ulTMP = intcRegs->EnableBasic;

	if(nIRQ >= 64 && nIRQ <= 72) {	// Basic IRQ enables
		intcRegs->EnableBasic = 1 << (nIRQ - 64);
	}
    if(nIRQ >=0 && nIRQ <= 31 ) 
    {
        intcRegs->Enable1 = (1 << nIRQ);
    }

	ulTMP = intcRegs->EnableBasic;

	// Otherwise its a GPU interrupt, and we're not supporting those...yet!

	return 0;
}

int DisableInterrupt(int nIRQ) {
	if(nIRQ >= 64 && nIRQ <= 72) {
		intcRegs->DisableBasic = 1 << (nIRQ - 64);
	}
    if(nIRQ >=0 && nIRQ <= 31 ) 
    {
        intcRegs->Disable1 = (1 << nIRQ);
    }


	// I'm currently only supporting the basic IRQs.

	return 0;
}

int EnableInterrupts() {
	irqEnable();
	return 0;
}

int DisableInterrupts() {
	irqDisable();
	return 0;
}
