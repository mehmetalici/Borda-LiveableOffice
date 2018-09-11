/*
 * ldr.c
 *
 *  Created on: Aug 16, 2018
 *      Author: mehmet.alici
 */

#include "ldr.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Driverlib includes
#include "utils.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_adc.h"
#include "hw_ints.h"
#include "hw_gprcm.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "uart.h"
#include "pinmux.h"
#include "pin.h"
#include "adc.h"
#include "timer_if.h"
#include "adc_userinput.h"
#include "uart_if.h"
#include "timer.h"

static volatile unsigned long g_ulBase;
unsigned int  uiChannel;
unsigned int  uiIndex=0;
unsigned long ulSample;
unsigned long pulAdcSamples[4096];
const int NO_OF_SAMPLES = 128;


void LDR_IF_SetFields(s_PostData *sensorData, const char* coffee_pot, int ucLevel) {
    strcpy(sensorData->id_key, ID_KEY_LDR);
    strcpy(sensorData->id, coffee_pot);
    strcpy(sensorData->val_key, VAL_KEY_LDR);
    sensorData->val[0] = ucLevel;
    sensorData->val[1] = '\0';
}


void LDR_IF_ConfigureNEnable(LDR_Struct *ldr, unsigned int pinNum,
                             const char* coffee_pot, int ucLevel,
                              float tresholdLevel, void (*LDRIntHandler) (void))
    {

    #ifdef CC3200_ES_1_2_1
    //
    // Enable ADC clocks.###IMPORTANT###Need to be removed for PG 1.32
    //
    HWREG(GPRCM_BASE + GPRCM_O_ADC_CLK_CONFIG) = 0x00000043;
    HWREG(ADC_BASE + ADC_O_ADC_CTRL) = 0x00000004;
    HWREG(ADC_BASE + ADC_O_ADC_SPARE0) = 0x00000100;
    HWREG(ADC_BASE + ADC_O_ADC_SPARE1) = 0x0355AA00;
    #endif

    LDR_IF_SetFields(&(ldr->s_data), coffee_pot, ucLevel);
    ldr->tresholdVal = tresholdLevel;
    //
    // Convert pin number to channel number
    //
    switch(pinNum)
    {
        case PIN_57:
            ldr->uiChannel = ADC_CH_0;
            break;
        case PIN_58:
            ldr->uiChannel = ADC_CH_1;
            break;
        case PIN_59:
            ldr->uiChannel = ADC_CH_2;
            break;
        case PIN_60:
            ldr->uiChannel = ADC_CH_3;
            break;
        default:
            break;
    }

    //
    // Configure ADC timer which is used to timestamp the ADC data samples
    //
    MAP_ADCTimerConfig(ADC_BASE,2^17);

    //
    // Enable ADC timer which is used to timestamp the ADC data samples
    //
    MAP_ADCTimerEnable(ADC_BASE);

    //
    // Enable ADC module
    //
    MAP_ADCEnable(ADC_BASE);


    //
    // Base address for first timer
    //
    g_ulBase = TIMERA0_BASE;


    //
    // Configuring the timers
    //
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);

    //
    // Setup the interrupts for the timer timeouts.
    //
    Timer_IF_IntSetup(g_ulBase, TIMER_A, LDRIntHandler);

    //
    // Turn on the timers feeding values in mSec
    //
    Timer_IF_Start(g_ulBase, TIMER_A, SAMPLING_INTERVAL);

}



void LDR_IF_InterruptClear() {

        unsigned long ulInts;
        ulInts = MAP_TimerIntStatus(g_ulBase, true);
        //
        // Clear the timer interrupt.
        //
        MAP_TimerIntClear(g_ulBase, ulInts);

}


float LDR_IF_ReadValue(unsigned int uiChannel) {
    //
    // Initialize Array index for multiple execution
    //
        uiIndex=0;


    //
    // Enable ADC channel
    //

    MAP_ADCChannelEnable(ADC_BASE, uiChannel);

    while(uiIndex < NO_OF_SAMPLES + 4)
    {
        if(MAP_ADCFIFOLvlGet(ADC_BASE, uiChannel))
        {
            ulSample = MAP_ADCFIFORead(ADC_BASE, uiChannel);
            pulAdcSamples[uiIndex++] = ulSample;
        }


    }

    MAP_ADCChannelDisable(ADC_BASE, uiChannel);

    uiIndex = 0;


    //
    // Print out ADC samples
    //

    return ((pulAdcSamples[4] >> 2 ) & 0x0FFF)*1.4/4096;
}

void LDR_IF_SetValue(LDR_Struct *ldr, float value) {
    ldr->value = value;
}








