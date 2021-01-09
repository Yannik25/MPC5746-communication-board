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
 **     Project     : flexcan_mpc5746c
 **     Processor   : MPC5746C_256
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2017-04-06, 11:12, # CodeGen: 2
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
#include "canCom1.h"
#include "pin_mux.h"
#include "FreeRTOS.h"
#include "clockMan1.h"
#include "dmaController1.h"
#include <stdint.h>
#include <stdbool.h>
extern void rtos_start(void);
#define PEX_RTOS_START rtos_start
volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */
#include "BoardDefines.h"
/**
 * Functions prototypes
 */
void BoardInit(void);
void FlexCANInit(void);
void GPIOInit(void);
void SendCANData(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len);
/**
 * Functions
 */
void BoardInit(void){
	/* Initialize and configure clocks
	 *  -   Setup system clocks, dividers
	 *  -   Configure FlexCAN clock, GPIO, LPSPI
	 *  -   see clock manager component for more details
	 */
	CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
			g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
	CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_FORCIBLE);
	/* Initialize LED1 and FlexCAN RX and TX pins */
	PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
}
void FlexCANInit(void){
	/*
	 * Initialize FlexCAN driver
	 *  - 8 byte payload size
	 *  - FD enabled
	 *  - 500 kbit/s bitrate
	 */
	FLEXCAN_DRV_Init(INST_CANCOM1, &canCom1_State, &canCom1_InitConfig0);
}
void GPIOInit(void){
	/* Turn off LED1 and LED2 */
	PINS_DRV_SetPins(LED_PORT, 1 << LED1);
	PINS_DRV_SetPins(LED_PORT, 1 << LED2);
}
void SendCANData(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len){
	/* Set information about the data to be sent
	 *  - 1 byte in length
	 *  - Standard message ID
	 *  - Bit rate switch enabled to use a different bitrate for the data segment
	 *  - Flexible data rate enabled
	 *  - Use zeros for FD padding
	 */
	flexcan_data_info_t dataInfo =
	{
			.data_length = len,
			.msg_id_type = FLEXCAN_MSG_ID_STD,
			.enable_brs  = true,
			.fd_enable   = true,
			.fd_padding  = 0U
	};
	/* Configure TX message buffer with index TX_MSG_ID and TX_MAILBOX*/
	FLEXCAN_DRV_ConfigTxMb(INST_CANCOM1, mailbox, &dataInfo, messageId);
	/* Execute send non-blocking */
	FLEXCAN_DRV_Send(INST_CANCOM1, mailbox, &dataInfo, messageId, data);
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
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
#ifdef PEX_RTOS_INIT
	PEX_RTOS_INIT();                   /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
#endif
	/*** End of Processor Expert internal initialization.                    ***/
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
