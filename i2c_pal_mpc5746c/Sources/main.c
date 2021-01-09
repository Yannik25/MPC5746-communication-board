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
/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "pin_mux.h"
#include "uart_pal1.h"
#include "dmaController1.h"
#include "clockMan1.h"
#include <devassert.h>
/* User includes (#include below this line is not maintained by Processor Expert) */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
/* Definition of the data transfer size */
#define TRANSFER_SIZE 8
uint8_t rxbuff[TRANSFER_SIZE];
uint8_t masterTxBuffer[TRANSFER_SIZE];

#define welcomeMsg "This example is an simple test for I2C\r\n"
#define sensorread "The I2C sensor values are:\r\n"
/* Error message displayed at the console, in case data is received erroneously */
#define errorMsg "An error occurred! The application will stop!\r\n"
//#define string// "sensorwerte"
/* Timeout in ms for blocking operations */
#define TIMEOUT         200UL
#define BUFFER_SIZE     256UL
/* Buffer used to receive data from the console */
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIdx;
uint16_t i;

void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while(cycles--);
}

uint8_t decToBcd(uint8_t val)
{
  return( (val/10*16) + (val%10) );
}

uint8_t bcdToDec(uint8_t val)
{
  return( (val/16*10) + (val%16) );
}

void setDS3231time(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t
  dayOfMonth, uint8_t month, uint8_t year)
  {
    masterTxBuffer[0]=0;
    masterTxBuffer[1]=decToBcd(second);
    masterTxBuffer[2]=decToBcd(minute);
	masterTxBuffer[3]=decToBcd(hour);
	masterTxBuffer[4]=decToBcd(dayOfWeek);
	masterTxBuffer[5]=decToBcd(dayOfMonth);
	masterTxBuffer[6]=decToBcd(month);
	masterTxBuffer[7]=decToBcd(year);

    (void) I2C_MasterSendDataBlocking(&i2cmaster5V_instance, masterTxBuffer, TRANSFER_SIZE, true, 0xFFFF);
    //(void) I2C_MasterSendDataBlocking(&i2cmaster33V_instance, masterTxBuffer, TRANSFER_SIZE, true, 0xFFFF);
  }

void readDS3231time(uint8_t *second, uint8_t *minute, uint8_t *hour, uint8_t *dayOfWeek, uint8_t *dayOfMonth, uint8_t *month, uint8_t *year){

  masterTxBuffer[0]=0;
  (void) I2C_MasterSendDataBlocking(&i2cmaster5V_instance, masterTxBuffer, 1, true, 0xFFFF);
  //(void) I2C_MasterSendDataBlocking(&i2cmaster33V_instance, masterTxBuffer, TRANSFER_SIZE, true, 0xFFFF);

  (void) I2C_MasterReceiveData(&i2cmaster5V_instance, rxbuff, TRANSFER_SIZE, true);
  //(void) I2C_MasterReceiveData(&i2cmaster33V_instance, rxbuff, TRANSFER_SIZE, true);

  *second = bcdToDec((rxbuff[1])& 0x7f);
  *minute = bcdToDec(rxbuff[2]);
  *hour = bcdToDec((rxbuff[3]) & 0x3f);
  *dayOfWeek = bcdToDec(rxbuff[4]);
  *dayOfMonth = bcdToDec(rxbuff[5]);
  *month = bcdToDec(rxbuff[6]);
  *year = bcdToDec(rxbuff[7]);

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
 *  @brief I2C Slave Callback
 *
 *  @param [in] instance   I2C instance number
 *  @param [in] slaveEvent Event received on the I2C bus
 *  @param [in] userData   User defined data that is passed to the callback
 *  @return None
 *
 *  @details This function will be called by I2C interrupt handler and it
 *  will assign the buffer for TX or RX events.
 *  If an error event occurs, it will abort the current transfer.
 */
void i2c1_SlaveCallback0(i2c_slave_event_t slaveEvent, void *userData)
{
	/* Get instance number from userData */
	i2c_instance_t * instance;
	instance = (i2c_instance_t *) userData;
	/* Check the event type:
	 *  - set RX or TX buffers depending on the master request type
	 */
	if (slaveEvent == I2C_SLAVE_EVENT_RX_REQ)
		I2C_SlaveSetRxBuffer(instance, rxbuff, TRANSFER_SIZE);
	if (slaveEvent == I2C_SLAVE_EVENT_TX_REQ)
		I2C_SlaveSetTxBuffer(instance, rxbuff, TRANSFER_SIZE);
}
volatile int exit_code = 0;
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

  char string[8]="";

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

    /* Initialize I2C Master configuration
     *  See I2C component for configuration details
     */
    I2C_MasterInit(&i2cmaster5V_instance, &i2cmaster5V_MasterConfig0);
    //I2C_MasterInit(&i2cmaster33V_instance, &i2cmaster5V_MasterConfig0);
    /* Initialize I2C Master configuration
     *  See I2C component for configuration details
     */

    setDS3231time(00,20,9,5,21,11,20); //09:20 Fr. 21.11.20

    uint8_t second, minute, hour, dayOfWeek, dayOfMonth, month, year;


    /* Infinite loop */
    for( ;; )
    {
        readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

        sprintf (string, "Time: %x:%x:%x Day:%x Date:%x.%x.%x\r\n", rxbuff[2],rxbuff[1],rxbuff[0],rxbuff[3],rxbuff[4],rxbuff[5],rxbuff[6]);

        UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)sensorread, strlen(sensorread), TIMEOUT);
        UART_SendDataBlocking(&uart_pal1_instance, (uint8_t *)string, strlen(string), TIMEOUT);

    	delay(7200000);

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
