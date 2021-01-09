/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/* SDK includes. */
#include "interrupt_manager.h"
#include "clock_manager.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "Cpu.h"
#include "canCom1.h"
#include "dmaController1.h"
#include <stdbool.h>
#include <stdint.h>
#include "BoardDefines.h"
/* Use  this macro to set one of the boards as the master and the other one as the slave */
#define MASTER //Board 2 (Press Button S6 or S7 on Board 2 or LED 4&5 will light if message from Board 1 arrives)
//#define SLAVE //Board 1 (Press Button S6 or S7 on Board 1 or LED 4&5 will light if message from Board 2 arrives)
/* Definition of the TX and RX message buffers depending on the bus role */
#if defined(MASTER)
#define TX_MAILBOX  (1UL)
#define TX_MSG_ID   (1UL)
#define RX_MAILBOX  (0UL)
#define RX_MSG_ID   (2UL)
#elif defined(SLAVE)
#define TX_MAILBOX  (0UL)
#define TX_MSG_ID   (2UL)
#define RX_MAILBOX  (1UL)
#define RX_MSG_ID   (1UL)
#endif
/* Definition of task priorities */
#define mainRECEIVE_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define mainTRANSMIT_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )
/* Definition of task frequency */
#define mainRECEIVE_FREQUENCY_MS	( 50 / portTICK_PERIOD_MS )
#define mainTRANSMIT_FREQUENCY_MS	( 50 / portTICK_PERIOD_MS )
/*
 * CAN commands type
 */
typedef enum
{
	LED1_CHANGE_REQUESTED = 0x00U,
	LED2_CHANGE_REQUESTED = 0x01U
} can_commands_list;
/*
 * Function prototypes
 */
static void prvSetupHardware( void );
static void prvTransmitTask( void *pvParameters );
static void prvReceiveTask( void *pvParameters );
/*
 * Functions found in main
 */
extern void BoardInit(void);
extern void FlexCANInit(void);
extern void GPIOInit(void);
extern void SendCANData(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len);
/*
 * Function implementations
 */
static void prvSetupHardware( void ){
	BoardInit();
	GPIOInit();
	FlexCANInit();
	flexcan_data_info_t dataInfo =
	{
			.data_length = 1U,
			.msg_id_type = FLEXCAN_MSG_ID_STD,
			.enable_brs  = true,
			.fd_enable   = true,
			.fd_padding  = 0U
	};
	/* Configure RX message buffer with index RX_MSG_ID and RX_MAILBOX */
	FLEXCAN_DRV_ConfigRxMb(INST_CANCOM1, RX_MAILBOX, &dataInfo, RX_MSG_ID);
}
static void prvTransmitTask( void *pvParameters ){
	TickType_t xNextWakeTime;
	/* Cast to void because parameter isn't used */
	(void)pvParameters;
	bool prevButton1 = false;
	bool prevButton2 = false;
	xNextWakeTime = xTaskGetTickCount();
	for ( ;; )
	{
		uint8_t ledRequested;
		bool button1Pressed = PINS_DRV_ReadPins(BTN1_PORT) & (1 << BTN1);
		bool button2Pressed = PINS_DRV_ReadPins(BTN2_PORT) & (1 << BTN2);
		if (button1Pressed && !prevButton1){
			ledRequested = LED1_CHANGE_REQUESTED;
			SendCANData(TX_MAILBOX, TX_MSG_ID, &ledRequested, 1UL);
		}
		else if (button2Pressed && !prevButton2){
			ledRequested = LED2_CHANGE_REQUESTED;
			SendCANData(TX_MAILBOX, TX_MSG_ID, &ledRequested, 1UL);
		}
		prevButton1 = button1Pressed;
		prevButton2 = button2Pressed;
		vTaskDelayUntil( &xNextWakeTime, mainTRANSMIT_FREQUENCY_MS );
	}
}
static void prvReceiveTask( void *pvParameters ){
	TickType_t xNextWakeTime;
	/* Cast to void because parameter isn't used */
	(void)pvParameters;
	xNextWakeTime = xTaskGetTickCount();
	for ( ;; )
	{
		/* Define receive buffer */
		flexcan_msgbuff_t recvBuff;
		/* Start receiving data in RX_MAILBOX. */
		FLEXCAN_DRV_Receive(INST_CANCOM1, RX_MAILBOX, &recvBuff);
		/* Wait until the previous FlexCAN receive is completed */
		while(FLEXCAN_DRV_GetTransferStatus(INST_CANCOM1, RX_MAILBOX) == STATUS_BUSY);
		if((recvBuff.data[0] == LED1_CHANGE_REQUESTED) &&
				recvBuff.msgId == RX_MSG_ID)
		{
			/* Toggle output value LED1 */
			PINS_DRV_TogglePins(LED_PORT, (1 << LED1));
		}
		else if((recvBuff.data[0] == LED2_CHANGE_REQUESTED) &&
				recvBuff.msgId == RX_MSG_ID)
		{
			/* Toggle output value LED2 */
			PINS_DRV_TogglePins(LED_PORT, (1 << LED2));
		}
		vTaskDelayUntil( &xNextWakeTime, mainRECEIVE_FREQUENCY_MS );
	}
}
void rtos_start( void ){
	/* Setup board hardware */
	prvSetupHardware();
	/* Create FreeRTOS task to receive CAN packets */
	xTaskCreate( prvTransmitTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainTRANSMIT_TASK_PRIORITY, NULL);
	/* Create FreeRTOS task to transmit CAN packets */
	xTaskCreate( prvReceiveTask, "RX", configMINIMAL_STACK_SIZE, NULL, mainRECEIVE_TASK_PRIORITY, NULL);
	/* Start FreeRTOS task scheduler */
	vTaskStartScheduler();
}
