/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ###################################################################
**     Filename    : main.c
**     Project     : hello_world_mpc5746c
**     Processor   : MPC5746C_324
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-14, 14:08, # CodeGen: 1
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */
/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "pin_mux.h"
  volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define PORT1	PTA	//LED on pin:  PA[2] GPIO[2]	SIUL2	I/O	PAD_SR_HV	F3	17
#define PORT2	PTE
#define LED1	2
#define LED2	0
#define LED3	1
#define LED4	8
#define LED5	9
#define LED6	10
#define LED7	0
#define LED8	11

#define PORT3 	PTF
#define RXPIN 	0
#define TXPIN 	1
#define BAUDRATE 1200
#define BITTIME 100000/BAUDRATE
#define HALFBITTIME 50000/BAUDRATE


void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while(cycles--);
}
void delaymicrosec(volatile int cycles)
{
	cycles*=14;
	while(cycles--);
}
/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/



char serialread()
{
	char rxdata=0;
	while(!(PINS_DRV_ReadPins(PORT3)&0x1));
	while((PINS_DRV_ReadPins(PORT3)&0x1));
	delaymicrosec(HALFBITTIME);
		for(uint8_t i=0;i<8;i++)
		{
			delaymicrosec(BITTIME);
			rxdata = rxdata | ((PINS_DRV_ReadPins(PORT3)&0x1)<<i);
		}
	delaymicrosec(HALFBITTIME);
	return rxdata;
}

void serialwrite(char data)
{
	PINS_DRV_SetPins(PORT3, (1 << TXPIN));
	for(uint8_t i=0;i<8;i++)
	{
		delaymicrosec(BITTIME);
		if((data>>i)&1){
			PINS_DRV_ClearPins(PORT3, (1 << TXPIN));
		}
		else{
			PINS_DRV_SetPins(PORT3, (1 << TXPIN));
		}
	}
	delaymicrosec(BITTIME);
	PINS_DRV_ClearPins(PORT3, (1 << TXPIN));
	delaymicrosec(BITTIME);
}

void writeln(char* buffer, int length)
{
	int pos=0;
	while(pos<length)
	{
		if(buffer[pos]==0)
		break;
		serialwrite(buffer[pos]);
		pos++;
	}
}

void readln(char* buffer, int length)
{
	int pos = 0;
	char c;
	while(1)
	{
		c = serialread();
		if(c==13) //enter or carriage return
		{
			buffer[pos]=0; //add null termination
			break;
		}
	buffer[pos] = c;
	pos++;
	if(pos>=length) pos = 0; //overflow, circle to the start
	}
}




int main(void)
{

  /* Write your local variable definition here */
  char serialbuffer[20] = { "Hello\n" };
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
  /* Initialize LED */
  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);

  while(1)
  {
      /* Toggle output value LED1 */
	  PINS_DRV_SetPins(PORT1, (1 << LED1));
      delay(72000);
      PINS_DRV_SetPins(PORT2, (1 << LED2));
      delay(72000);
      PINS_DRV_SetPins(PORT1, (1 << LED3));
	  delay(72000);
      PINS_DRV_SetPins(PORT2, (1 << LED4));
	  delay(72000);
      PINS_DRV_SetPins(PORT2, (1 << LED5));
	  delay(72000);
      PINS_DRV_SetPins(PORT2, (1 << LED6));
	  delay(72000);
      PINS_DRV_SetPins(PORT1, (1 << LED7));
	  delay(72000);
      PINS_DRV_SetPins(PORT2, (1 << LED8));
	  delay(72000);
	  writeln(serialbuffer,20);

	  PINS_DRV_ClearPins(PORT1, (1 << LED1));
	  PINS_DRV_ClearPins(PORT2, (1 << LED2));
	  PINS_DRV_ClearPins(PORT1, (1 << LED3));
	  PINS_DRV_ClearPins(PORT2, (1 << LED4));
	  PINS_DRV_ClearPins(PORT2, (1 << LED5));
	  PINS_DRV_ClearPins(PORT2, (1 << LED6));
	  PINS_DRV_ClearPins(PORT1, (1 << LED7));
	  PINS_DRV_ClearPins(PORT2, (1 << LED8));
	  delay(720000);

  }
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the NXP C55 series of microcontrollers.
**
** ###################################################################
*/
