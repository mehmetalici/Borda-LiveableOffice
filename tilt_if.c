/*
 * tilt_if.c
 *
 *  Created on: Aug 15, 2018
 *      Author: mehmet.alici
 */
// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_apps_rcm.h"
#include "interrupt.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"

// OS includes
#if defined(USE_TIRTOS) || defined(USE_FREERTOS) || defined(SL_PLATFORM_MULTI_THREADED)
#include <stdlib.h>
#include "osi.h"
#endif

// Common interface include
#include "gpio_if.h"
#include "tilt_if.h"


/**********GLOBAL VARIABLES*******************/


void Tilt_IF_IntClearNDisable(Tilt_Struct tilt){
    GPIOIntClear  (tilt.uiPort, tilt.uiIntFlag);
    GPIOIntDisable(tilt.uiPort, tilt.uiIntFlag);
}

void Tilt_IF_IntClearNEnable(Tilt_Struct *tilt){
    tilt->intFound = 0;
    GPIOIntClear  (tilt->uiPort, tilt->uiIntFlag);
    GPIOIntEnable(tilt->uiPort, tilt->uiIntFlag);
}

unsigned short getGPIOPin(unsigned char ucPin){
    unsigned short count = 0;
    while(ucPin > 0) {
        ucPin /= 2;
        count++;
    }
    return count;
}

char Tilt_IF_ReadValueChar(Tilt_Struct tilt) {
    if(GPIOPinRead(tilt.uiPort, tilt.ucPin) >> (getGPIOPin(tilt.ucPin) % 8) - 1) {
        return '1';
    }
    else {
        return '0';
    }
}

int Tilt_IF_ReadValueInt(Tilt_Struct tilt) {
    return (GPIOPinRead(tilt.uiPort, tilt.ucPin) >> (getGPIOPin(tilt.ucPin) % 8) - 1);
}

void Tilt_IF_SetValue(Tilt_Struct *tilt, int value) {

    if(value)
        tilt->s_data.val[0] = '1';
    else
        tilt->s_data.val[0] = '0';
    tilt->s_data.val[1] = '\0';
}

long Tilt_IF_IntStatus(unsigned long ulPort, tBoolean bMasked) {
    return GPIOIntStatus  ( ulPort,  bMasked);
}

void Tilt_IF_SetIntFlag(Tilt_Struct *tilt, unsigned int gpioNum){
    tilt->uiIntFlag = 1 << (gpioNum % 8);
}

float Tilt_IF_Moving_Avg(int raw_adc) {
    static uint32_t buffer[10]; // N = number of your samples.
    static uint32_t oldest = 0;
    static unsigned long sum = 0;
    static int count = 0;
    count++;
    sum -= buffer[oldest]; //Subtract the oldest entry from the sum.
    sum += raw_adc; // Add the newest one to it.
    buffer[oldest] = raw_adc; // Substitute them in the buffer.
    /*Cycle the variable oldest in the buffer.*/
    oldest += 1;
    if (oldest >= 10) oldest = 0;
    if(count >= 10){
        count = 0;
        return sum / 10.0;
    }
    else
        return -1;
}

void Tilt_IF_ConfigureNIntEnable(Tilt_Struct *tilt, unsigned int gpioNum, const char* id, void (*pfnIntHandler)(void))
{
    strcpy(tilt->s_data.id_key, TILTS_ID_KEY);
    strcpy(tilt->s_data.val_key, TILTS_VAL_KEY);
    strcpy(tilt->s_data.id, id);


    Tilt_IF_SetIntFlag(tilt, gpioNum);

    GPIO_IF_SetPortNPin(gpioNum,
                        &tilt->uiPort,
                        &tilt->ucPin);

    GPIO_IF_ConfigureNIntEnable(tilt->uiPort,
                                tilt->ucPin,
                                TILTS_INTERRUPT_TYPE,
                                pfnIntHandler);
}


















