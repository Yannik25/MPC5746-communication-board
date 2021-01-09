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
#include "uart_pal1.h"
  volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <string.h>
#define welcomeMsg "This example is an simple echo using UART_PAL\r\n\
it will send back any character you send to it.\r\n\
The board will greet you if you send 'Hello!'\r\
\nNow you can begin typing:\r\n"
/* Error message displayed at the console, in case data is received erroneously */
#define errorMsg "An error occurred! The application will stop!\r\n"
/* Timeout in ms for blocking operations */
#define TIMEOUT         200UL
#define BUFFER_SIZE     256UL
/* Buffer used to receive data from the console */
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIdx;
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
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/
  /* Write your local variable definition here */
  status_t status;
  uint32_t bytesRemaining = 0;
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
  UART_Init(&uart_pal1_instance, &uart_pal1_Config0);
  /* Send a welcome message */
  UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)welcomeMsg, strlen(welcomeMsg), TIMEOUT);
  /* Infinite loop:
   *     - Receive data from user
   *     - Echo the received data back
   */
  /* Infinite loop */
  for( ;; )
  {
      /* Receive and store data byte by byte until new line character is received,
       * or the buffer becomes full (256 characters received)
       */
      UART_ReceiveData(&uart_pal1_instance, buffer, 1U);
      /* Wait for transfer to be completed */
      while(UART_GetReceiveStatus(&uart_pal1_instance, &bytesRemaining) == STATUS_BUSY);
      /* Check the status */
      status = UART_GetReceiveStatus(&uart_pal1_instance, &bytesRemaining);
      if (status != STATUS_SUCCESS)
      {
          /* If an error occurred, send the error message and exit the loop */
          UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)errorMsg, strlen(errorMsg), TIMEOUT);
          break;
      }
      /* Process end of line in Doc/Window(CRLF) or Unix(LF) */
      if (buffer[bufferIdx - 1] == '\r')
      {
          bufferIdx = bufferIdx - 1;
          /* Replace end of line character with null */
          buffer[bufferIdx] = 0U;
      }
      else
      {
          /* Replace end of line character with null */
          buffer[bufferIdx] = 0U;
      }
      /* If the received string is "Hello!", send back "Hello World!" */
      if(strcmp((char *)buffer, "Hello!") == 0)
      {
          strcpy((char *)buffer, "Hello World!\n");
      }
      else
      {
          buffer[bufferIdx] = '\n';
          bufferIdx++;
          /* Append string terminator to the received data */
          buffer[bufferIdx] = 0U;
      }
      /* Send the received data back */
      UART_SendDataBlocking(&uart_pal1_instance, buffer, strlen((char *) buffer), TIMEOUT);
      /* Reset the buffer index to start a new reception */
      bufferIdx = 0U;
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
