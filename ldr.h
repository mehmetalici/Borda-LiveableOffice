/*
 * ldr.h
 *
 *  Created on: Aug 16, 2018
 *      Author: mehmet.alici
 */
#include "gpio.h"
#include "http_if.h"
#ifndef LDR_H_
#define LDR_H_

typedef struct {
    unsigned int uiChannel;
    float value;
    float tresholdVal;
    s_PostData s_data;
}LDR_Struct;


#define INDICATE_HIGH_LEVEL '3'
#define INDICATE_MID_LEVEL '2'
#define INDICATE_LOW_LEVEL '1'
#define INDICATE_EMPTY_LEVEL '0'

#define LDR0_COFFEE_POT "0"
#define LDR1_COFFEE_POT "0"
#define LDR2_COFFEE_POT "0"
#define LDR3_COFFEE_POT "0"

#define LDR0_TRESHOLD_LEVEL 0.5
#define LDR1_TRESHOLD_LEVEL 0.5
#define LDR2_TRESHOLD_LEVEL 0.5
#define LDR3_TRESHOLD_LEVEL 0.02

#define ID_KEY_LDR "id"
#define VAL_KEY_LDR "value"

#endif /* LDR_H_ */

#define LDR0PIN PIN_57
#define LDR1PIN PIN_58
#define LDR2PIN PIN_59
#define LDR3PIN PIN_60

#define LDR_ID "LDR"
#define SAMPLING_INTERVAL 10000

void LDR_IF_SetFields(s_PostData *sensorData, const char* coffee_pot, int ucLevel);
void LDR_IF_SetValue(LDR_Struct *ldr, float value);
float LDR_IF_ReadValue(unsigned int uiChannel);
void LDR_IF_InterruptClear();
void LDR_IF_ConfigureNEnable(LDR_Struct *ldr, unsigned int pinNum,
                             const char* coffee_pot, int ucLevel,
                              float tresholdLevel, void (*LDRIntHandler) (void));
