/*
 * tilt_if.h
 *
 *  Created on: Aug 15, 2018
 *      Author: mehmet.alici
 */
#include "gpio.h"
#include "http_if.h"

#ifndef TILT_IF_H_
#define TILT_IF_H_

#define TILT1GPIO 14
#define TILT2GPIO 10
#define TILT3GPIO 11


#define TILTS_ID_KEY "id"
#define TILTS_VAL_KEY "value"
#define TILTS_INTERRUPT_TYPE GPIO_BOTH_EDGES

#endif /* TILT_IF_H_ */


typedef struct {
    unsigned int uiPort;
    unsigned char ucPin;
    unsigned int uiIntFlag;
    s_PostData s_data;
    unsigned int  uiIntType;
    volatile unsigned int intFound;
    float val_Stabilized;
    int val_InInterrupt;
    int val_MovingAvg;
    void (*pfnIntHandler)(void);
}Tilt_Struct;



void Tilt_IF_IntClearNEnable(Tilt_Struct *tilt);
void Tilt_IF_IntClearNDisable(Tilt_Struct tilt);
float Tilt_IF_Moving_Avg(int raw_adc);
int Tilt_IF_ReadValueInt(Tilt_Struct tilt);
long Tilt_IF_IntStatus(unsigned long ulPort, tBoolean bMasked);
void Tilt_IF_SetValue(Tilt_Struct *tilt,  int cValue);
char Tilt_IF_ReadValue(Tilt_Struct tilt);
unsigned short getGPIOPin(unsigned char ucPin);
void Tilt_IF_InterruptClear(unsigned long ucPort, unsigned long ulInts);
void Tilt_IF_ConfigureNIntEnable(Tilt_Struct *tilt, unsigned int gpioNum, const char* id, void (*pfnIntHandler)(void));
