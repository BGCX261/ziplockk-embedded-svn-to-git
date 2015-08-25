/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO(R)
 **
 **  Distribution: The file is distributed �as is,� without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. Distribution of this file (unmodified or modified) is not
 **  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
 **  rights to distribute the assembled, compiled & linked contents of this
 **  file as part of an application binary file, provided that it is built
 **  using the Atollic TrueSTUDIO(R) toolchain.
 **
 *****************************************************************************
 */

/* Includes */
#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "common.h"

#define DELAY	(100)

__IO float Result;
float Data0, Data1=1.0f, Data2=2.0f ;

/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void)
{
	/* Example use SysTick timer and read System core clock */
	SysTick_Config(72);  /* 1 us if clock frequency 72 MHz */

	SystemCoreClockUpdate();
	uint32_t clock = SystemCoreClock;   /* This is a way to read the System core clock */

	/* Initialize LEDs and User Button available on STM32F3-Discovery board */
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED7);
	STM_EVAL_LEDInit(LED8);
	STM_EVAL_LEDInit(LED9);
	STM_EVAL_LEDInit(LED10);

	//STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

    /* Initialze float variables */
    Data0 = 1.0 ;
    Data1 = 2.0 ;	
    
    /* Start a set of mathematical operations */ 
    Result = fabsf(Data0) ;
  
    Result = -Data0 ;
  
    Result = Data1 + Data2 ;
  
    Result = Data1 - Data2 ;
  
    Result = Data0 + Data1 * Data2 ;
    Result = Data0 - Data1 * Data2 ;	
    Result = -Data0 + Data1 * Data2 ;
    Result = -Data0 - Data1 * Data2 ;	
  
    Result = fmaf(Data1,Data2,Data0) ;
    Result = fmaf(-Data1,Data2,Data0) ;	
    Result = fmaf(Data1,Data2,-Data0) ;
    Result = fmaf(-Data1,Data2,-Data0) ;
  
    Result = Data1 / Data2 ;
    Result = sqrtf(Data1) ;	
    Result = sqrt(Data1) ;		
    Result = fabs(Data1) ;

	while (1)
	{
		STM_EVAL_LEDToggle(LED3);
		_delay_ms(DELAY);
	}

	/* Program will never run to this line */
	return 0;
}
