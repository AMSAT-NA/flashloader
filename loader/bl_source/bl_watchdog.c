/*
 * bl_watchdog.c
 *
 *  Created on: Apr 10, 2023
 *      Author: bfisher
 */

#include "reg_spi.h"
#include "reg_rti.h"
#include "gio.h"
#include "bl_config.h"

void kickWatchdog(void){
    uint32_t currentValue;
    uint32_t pinMask = (1<<WATCHDOG_PIN);
#ifdef WATCHDOG_TIMED_WAIT
    static uint32_t currentCounter,lastCounterValue=0;
    int32_t diffCounter;
    currentCounter = rtiREG1->CNT[0].FRCx;
    diffCounter = (int32_t)currentCounter - (int32_t)lastCounterValue;
    if(diffCounter > MIN_WATCHDOG_TOGGLE){
        lastCounterValue = currentCounter;
#endif
        WATCHDOG_PORT->DIR |= pinMask; //Set the watchdog pin to allow input
        currentValue = (WATCHDOG_PORT->DIN) & pinMask;
        if (currentValue == 0){
            WATCHDOG_PORT->DSET = pinMask; //Invert the pin to kick the wd.
        } else {
            WATCHDOG_PORT->DCLR = pinMask;
        }
#ifdef WATCHDOG_TIMED_WAIT
    } else if (diffCounter<0){
        lastCounterValue = currentCounter;
    } //Here, it wrapped.
#endif
}
#ifdef WATCHDOG_TIMED_WAIT
static void rtiInit(void)
{
/* USER CODE BEGIN (3) */
/* USER CODE END */
    /** @b Initialize @b RTI1: */

    /** - Setup NTU source, debug options and disable both counter blocks */
    rtiREG1->GCTRL = (uint32)((uint32)0x0U << 16U) | 0x00000000U;

    /** - Setup timebase for free running counter 0 */
    rtiREG1->TBCTRL = 0x00000000U;

    /** - Enable/Disable capture event sources for both counter blocks */
    rtiREG1->CAPCTRL = 0U | 0U;

    /** - Setup input source compare 0-3 */
    rtiREG1->COMPCTRL = 0x00001000U | 0x00000100U | 0x00000000U | 0x00000000U;

    /** - Reset up counter 0 */
    rtiREG1->CNT[0U].UCx = 0x00000000U;

    /** - Reset free running counter 0 */
    rtiREG1->CNT[0U].FRCx = 0x00000000U;

    /** - Setup up counter 0 compare value
    *     - 0x00000000: Divide by 2^32
    *     - 0x00000001-0xFFFFFFFF: Divide by (CPUC0 + 1)
    */
    rtiREG1->CNT[0U].CPUCx = 7U;

    /** - Reset up counter 1 */
    rtiREG1->CNT[1U].UCx = 0x00000000U;

    /** - Reset free running counter 1 */
    rtiREG1->CNT[1U].FRCx  = 0x00000000U;

    /** - Setup up counter 1 compare value
    *     - 0x00000000: Divide by 2^32
    *     - 0x00000001-0xFFFFFFFF: Divide by (CPUC1 + 1)
    */
    rtiREG1->CNT[1U].CPUCx = 7U;

    /** - Setup compare 0 value. This value is compared with selected free running counter. */
    rtiREG1->CMP[0U].COMPx = 100000U;

    /** - Setup update compare 0 value. This value is added to the compare 0 value on each compare match. */
    rtiREG1->CMP[0U].UDCPx = 100000U;

    /** - Setup compare 1 value. This value is compared with selected free running counter. */
    rtiREG1->CMP[1U].COMPx = 570000U;

    /** - Setup update compare 1 value. This value is added to the compare 1 value on each compare match. */
    rtiREG1->CMP[1U].UDCPx = 570000U;

    /** - Setup compare 2 value. This value is compared with selected free running counter. */
    rtiREG1->CMP[2U].COMPx = 80000U;

    /** - Setup update compare 2 value. This value is added to the compare 2 value on each compare match. */
    rtiREG1->CMP[2U].UDCPx = 80000U;

    /** - Setup compare 3 value. This value is compared with selected free running counter. */
    rtiREG1->CMP[3U].COMPx = 100000U;

    /** - Setup update compare 3 value. This value is added to the compare 3 value on each compare match. */
    rtiREG1->CMP[3U].UDCPx = 100000U;

    /** - Clear all pending interrupts */
    rtiREG1->INTFLAG = 0x0007000FU;

    /** - Disable all interrupts */
    rtiREG1->CLEARINTENA = 0x00070F0FU;

    /**   @note This function has to be called before the driver can be used.\n
    *           This function has to be executed in privileged mode.\n
    *           This function does not start the counters.
    */

/* USER CODE BEGIN (4) */
/* USER CODE END */
}

/* USER CODE BEGIN (5) */
/* USER CODE END */


/** @fn void rtiStartCounter(uint32 counter)
*   @brief Starts RTI Counter block
*   @param[in] counter Select counter block to be started:
*              - rtiCOUNTER_BLOCK0: RTI counter block 0 will be started
*              - rtiCOUNTER_BLOCK1: RTI counter block 1 will be started
*
*   This function starts selected counter block of the selected RTI module.
*/

/* USER CODE BEGIN (6) */
/* USER CODE END */
/* SourceId : RTI_SourceId_002 */
/* DesignId : RTI_DesignId_002 */
/* Requirements : HL_SR77 */
static void rtiStartCounter(uint32 counter)
{
/* USER CODE BEGIN (7) */
/* USER CODE END */

    rtiREG1->GCTRL |= ((uint32)1U << (counter & 3U));

    /**   @note The function rtiInit has to be called before this function can be used.\n
    *           This function has to be executed in privileged mode.
    */

/* USER CODE BEGIN (8) */
/* USER CODE END */
}

/* USER CODE BEGIN (9) */
/* USER CODE END */


/** @fn void rtiStopCounter(uint32 counter)
*   @brief Stops RTI Counter block
*   @param[in] counter Select counter to be stopped:
*              - rtiCOUNTER_BLOCK0: RTI counter block 0 will be stopped
*              - rtiCOUNTER_BLOCK1: RTI counter block 1 will be stopped
*
*   This function stops selected counter block of the selected RTI module.
*/
#endif
void startCounter(void){
#ifdef WATCHDOG_TIMED_WAIT
    rtiInit();
    rtiStartCounter(0);
#endif
}

