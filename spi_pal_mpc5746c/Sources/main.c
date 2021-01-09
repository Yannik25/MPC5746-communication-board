/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ###################################################################
**     Filename    : main.c
**     Project     : spi_pal_mpc5746c
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

#define LEDMATRIX
//#define MAX6675

/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "spi.h"
#include "dmaController1.h"
#include "uart_pal1.h"
#include <string.h>

 volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdint.h>
#include <stdbool.h>
#define TIMEOUT 1000U
#define NUMBER_OF_FRAMES_TO_SEND 2U
#define NUMBER_OF_FRAMES_TO_RECEIVE 1U
#define BUFFER_SIZE 10U

/* Definition of the data transfer size */
#define TRANSFER_SIZE 8
uint8_t rxbuff[TRANSFER_SIZE];
uint8_t masterTxBuffer[TRANSFER_SIZE];

#define welcomeMsg "This example is an simple test for SPI\r\n"
#define sensorread "The SPI sensor values are:\r\n"
#define displayon "Turn the SPI LED Pixel on\r\n"
#define displayoff "Turn the SPI LED Pixel off\r\n"
/* Error message displayed at the console, in case data is received erroneously */
#define errorMsg "An error occurred! The application will stop!\r\n"

/* Buffer used to receive data from the console */
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIdx;


void delay(volatile int cycles)
{
  /* Delay function - do nothing for a number of cycles */
  while(cycles--);
}

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
            UART_SetRxBuffer(&uart_pal1_instance, &buffer[bufferIdx], 1U);
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
  /* Write your local variable definition here */
  uint8_t master_sendon[BUFFER_SIZE] = {0xF, 0x1};
  uint8_t master_sendoff[BUFFER_SIZE] = {0xF, 0x0};
  uint16_t master_send_to_receive[BUFFER_SIZE] = {0xFFFF,0xFF};
  uint16_t master_receive[BUFFER_SIZE];
  uint8_t slave_send[BUFFER_SIZE] = {};
  uint8_t slave_receive[BUFFER_SIZE];
  char string[8]="";
  int temp = 0;
  float tempC;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

  CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT, g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

  /* Initialize pins
  *	See PinSettings component for more info
  */
  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);


  /* Initialize UART PAL over LINFlexD */
  UART_Init(&uart_pal1_instance, &uart_pal1_Config0);
  /* Send a welcome message */
  UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)welcomeMsg, strlen(welcomeMsg), TIMEOUT);

  /* Initialize SPI0 */
  SPI_MasterInit(&spiInstance, &MasterConfig0);

  /* Infinite loop */
  while(1)
  {

	  #ifdef LEDMATRIX
	  SPI_MasterTransfer(&spiInstance, master_sendon, master_receive, NUMBER_OF_FRAMES_TO_SEND);
	  UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)displayon, strlen(displayon), TIMEOUT);
	  delay(100000);
	  SPI_MasterTransfer(&spiInstance, master_sendoff, master_receive, NUMBER_OF_FRAMES_TO_SEND);
	  UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)displayoff, strlen(displayoff), TIMEOUT);
	  delay(100000);
	  #endif

	  #ifdef MAX6675
	  SPI_MasterTransfer(&spiInstance, master_send_to_receive, master_receive, NUMBER_OF_FRAMES_TO_RECEIVE);
	  UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)sensorread, strlen(sensorread), TIMEOUT);
	  temp = master_receive[0] >> 2;
	  tempC = temp * 0.25;
	  sprintf (string, "TempC %f\r\n", tempC);
	  UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)string, strlen(string), TIMEOUT);
	  delay(1000000);
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
