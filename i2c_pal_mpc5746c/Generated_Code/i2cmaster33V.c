/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : i2cmaster33V.c
**     Project     : i2c_pal_mpc5746c
**     Processor   : MPC5746C_256
**     Component   : i2c_pal
**     Version     : Component SDK_S32_PA_11, Driver 01.00, CPU db: 3.00.000
**     Repository  : SDK_S32_PA_11
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-11-20, 16:20, # CodeGen: 2
**
**     Copyright 1997 - 2015 Freescale Semiconductor, Inc. 
**     Copyright 2016-2017 NXP 
**     All Rights Reserved.
**     
**     THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
**     IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
**     OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**     IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
**     INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
**     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
**     STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
**     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
**     THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file i2cmaster33V.c
** @version 01.00
*/         
/*!
**  @addtogroup i2cmaster33V_module i2cmaster33V module documentation
**  @{
*/         
#include "i2cmaster33V.h"

/*!
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.9, Could define variable at block scope
 * The variable is used by user so it must remain global.
 
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, Conversion between a pointer and
 * integer type.
 * The cast is required to initialize a pointer with an unsigned long define,
 * representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, Cast from unsigned int to pointer.
 * The cast is required to initialize a pointer with an unsigned long define,
 * representing an address.
 */

/*! @brief PAL instance information */
i2c_instance_t i2cmaster33V_instance = {I2C_INST_TYPE_I2C, 0U};

i2c_master_t i2cmaster33V_MasterConfig0 =
{
    .slaveAddress        = 104,
    .is10bitAddr         = false,
    .baudRate            = 100000,
    .transferType        = I2C_PAL_USING_INTERRUPTS,
    .operatingMode       = I2C_PAL_STANDARD_MODE,
    .dmaChannel1         = 255,
    .dmaChannel2         = 255,
    .callback            = NULL,
    .callbackParam       = NULL,
    .extension           = NULL 
};            
 
i2c_slave_t i2cmaster33V_SlaveConfig0 =
{
    .slaveAddress       = 0,
    .is10bitAddr        = false,
    .slaveListening     = true,
    .transferType       = I2C_PAL_USING_INTERRUPTS,
    .dmaChannel         = 255,
    .callback           = NULL,
    .callbackParam      = NULL
};
        
/* END i2cmaster33V. */

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
