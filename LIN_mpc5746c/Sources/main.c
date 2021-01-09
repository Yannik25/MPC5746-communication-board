/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
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
**     Project     : uart_pal_mpc5746c
**     Processor   : MPC5746C_256
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-07-13, 13:47, # CodeGen: 1
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
#include "dmaController1.h"
#include "pin_mux.h"
#include "clockMan1.h"
#include "uart.h"
  volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <string.h>
#define frametransmitted "'LIN_Frame Transmitted'\n"
#define framereceived "'LIN_Frame Received'\n"
/* Error message displayed at the console, in case data is received erroneously */
#define errorMsg "An error occurred! The application will stop!\r\n"
/* Timeout in ms for blocking operations */
#define TIMEOUT         200UL
#define BUFFER_SIZE     256UL
/* Buffer used to receive data from the console */
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIdx;

#define LINSlave 	//config LIN module as Slave
#define LINSlavesend	//config Slave to send data to Master
//#define LINSlavereceive 	//config Slave to receive data to Master //TODO
//#define LINMaster	//config LIN module as Master
//#define LINMastersend	//config Master to send data to Slave
//#define LINMasterreceive 	//config Master to receive data to Slave

void initLINFlexD_0 (void);
void transmitLINframe (void);
void receiveLINframe(void);

void delay(volatile int cycles)
{
  /* Delay function - do nothing for a number of cycles */
  while(cycles--);
}

void initLINFlexD_0 (void) {     	/* Master at 10.417K baud with 80MHz uart_CLK */
  #ifdef LINMaster
	LINFlexD_0->LINCR1 = 0x00000311; 	/* Configure module as uart master & header */
  #endif
  #ifdef LINSlave
	LINFlexD_0->LINCR1 = 0x00000301; 	/* Configure module as uart slave & header */
  #endif
  LINFlexD_0->LINIBRR = 0xd; 		/* Mantissa baud rate divider component */
  	 	  	  	  	  	  	  	  	/* Baud rate divider = 4 MHz uart_CLK input / (16*19200K bps) ~=13 = 0xd */
  LINFlexD_0->LINFBRR = 0x0; 		/* Fraction baud rate divider component */
  #ifdef LINMaster
  	LINFlexD_0->LINCR1 = 0x00000310; 	/* Change module mode from init to normal */
  #endif
  #ifdef LINSlave
  	LINFlexD_0->LINCR1 = 0x00000300; 	/* Change module mode from init to normal */
  #endif
}

void transmitLINframe (void) {   	/* Transmit one frame 'hello    ' to ID 0x35*/
	#ifndef LINSlave
		LINFlexD_0->BIDR = 0x00001E35;
	#endif
	#ifdef LINSlave
		LINFlexD_0->BIDR = 0x00001E35;
	#endif
	LINFlexD_0->LINCR2 = LINFlexD_LINCR2_HTRQ_MASK;	//1<<8;
	#if defined(LINMastersend) || defined(LINSlavesend)	//If Mastersend or Slavesend then send data, else only Header
		LINFlexD_0->BDRL = 0x6C6C6568;
		LINFlexD_0->BDRM = 0x2020206F;
	#endif
	#ifdef LINSlavesend
		LINFlexD_0->LINCR2 = LINFlexD_LINCR2_DTRQ_MASK;
	#endif
	#ifndef LINSlavesend
		while (!(LINFlexD_0->LINSR & LINFlexD_LINSR_DTF_MASK));
	#endif
	LINFlexD_0->LINSR &= ~1<<1;
}

void receiveLINframe (void) {      	/* Request data from ID 0x35 */
	uint8_t __attribute__ ((unused)) RxBuffer[8] = {0};
	uint8_t i;

	LINFlexD_0->BIDR = 0x00001E35; 	/* Init header: ID=0x35, 8 B, Rx, enh cksum */
    LINFlexD_0->LINCR2 = LINFlexD_LINCR2_HTRQ_MASK;	//.B.HTRQ = 1;   	/* Request header transmission */
    while (!(LINFlexD_0->LINSR & LINFlexD_LINSR_DTF_MASK)); /* Wait for data receive complete flag */
									/* Code waits here if no slave response */
    for (i=0; i<4;i++){         		/* If received less than or equal 4 data bytes */
	  RxBuffer[i]= (LINFlexD_0->BDRL>>(i*8)); /* Fill buffer in reverse order */
    }
    for (i=4; i<8;i++){         		/* If received more than 4 data bytes: */
	  RxBuffer[i]= (LINFlexD_0->BDRM>>((i-4)*8)); /* Fill rest in reverse order */
    }
    LINFlexD_0->LINSR &= ~1<<2;	//.R = 0x00000004;  /* Clear DRF flag */
}

//void receiveLINframe (void) {      	/* Request data from ID 0x15 */
//	uint8_t __attribute__ ((unused)) RxBuffer[8] = {0};
//	uint8_t i;
//
//  LINFlexD_0.BIDR.R = 0x00001C15; 	/* Init header: ID=0x15, 8 B, Rx, enh cksum */
//  LINFlexD_0.LINCR2.B.HTRQ = 1;   	/* Request header transmission */
//  while (!LINFlexD_LINSR_DRF_MASK); /* Wait for data receive complete flag */
//                              	  	/* Code waits here if no slave response */
//  for (i=0; i<4;i++){         		/* If received less than or equal 4 data bytes */
//	RxBuffer[i]= (LINFlexD_0.BDRL.R>>(i*8)); /* Fill buffer in reverse order */
//  }
//  for (i=4; i<8;i++){         		/* If received more than 4 data bytes: */
//	RxBuffer[i]= (LINFlexD_0.BDRM>>((i-4)*8)); /* Fill rest in reverse order */
//  }
//  LINFlexD_0.LINSR.R = 0x00000004;  /* Clear DRF flag */
//
//}

/* UART rx callback for continuous reception, byte by byte */
void rxCallback(void *driverState, uart_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;
    /* Check the event type */
    if (event == UART_EVENT_RX_FULL)
    {
        /* The reception stops when newline is received or the buffer is full */
        if ((buffer[bufferIdx] != '\n') && (bufferIdx != (BUFFER_SIZE - 2U)))
        {
            /* Update the buffer index and the rx buffer */
            bufferIdx++;
            UART_SetRxBuffer(&uart_instance, &buffer[bufferIdx], 1U);
        }
    }
}
/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
  /* Write your local variable definition here */
  status_t status;
  uint32_t bytesRemaining = 0;

  /*new_request = false;
  Rx_Response = false;
*/

  /* Initialize and configure clocks
   *  -   Setup system clocks, dividers
   *  -   see clock manager component for more details
   */
  CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                 g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_FORCIBLE);
  /* Initialize pins */
  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
  /* Initialize UART PAL over LINFlexD */
  UART_Init(&uart_instance, &uart_Config0);
  /* Infinite loop:
   *     - Receive data from user
   *     - Echo the received data back
   */
  initLINFlexD_0();

  while(1)
  {
	#ifdef LINMastersend
	  UART_SendDataBlocking(&uart_instance, (uint8_t *)frametransmitted, strlen(frametransmitted), TIMEOUT);
	  transmitLINframe();
	  delay(1000000);
	#endif
	#ifdef LINMasterreceive
	  receiveLINframe();
	  UART_SendDataBlocking(&uart_instance, (uint8_t *)framereceived, strlen(framereceived), TIMEOUT);
	  delay(1000000);
	#endif
	#ifdef LINSlavesend
	  if((LINFlexD_0->BIDR & LINFlexD_BIDR_ID_MASK) == 0x35){
		  LINFlexD_0->LINSR = LINFlexD_LINSR_HRF_MASK;
		 transmitLINframe();
	  }
	#endif
	#ifdef LINSlavereceive
	  if((LINFlexD_0->BIDR & LINFlexD_BIDR_ID_MASK) == 0x35){
		  receiveLINframe();
	  }
	#endif
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
