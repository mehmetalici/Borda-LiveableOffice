//#include "headers/a.h"
#include <string.h>

// SimpleLink includes
#include "simplelink.h"
#include "utils.h"
// driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "utils.h"
#include "interrupt.h"
#include "jsmn.h"
// common interface includes
#include "uart_if.h"
#include "common.h"
#include "pinmux.h"
#include "timer_if.h"
#include "gpio.h"
#include "string_if.h"
#include "tilt_if.h"
#include "ldr.h"

// HTTP Client lib
#include <http/client/httpcli.h>
#include <http/client/common.h>
#include <http_if.h>
#include "timer.h"
// JSON Parser
#include "jsmn.h"

//ADC Includes
#include "adc_userinput.h"
// Standard includes
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Driverlib includes
#include "utils.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_adc.h"
#include "hw_gprcm.h"
#include "prcm.h"
#include "uart.h"
#include "pinmux.h"
#include "pin.h"
#include "adc.h"
#include "hw_apps_rcm.h"
#include "gpio_if.h"
#include "ldr.h"



#define APPLICATION_VERSION "1.0.0"
#define APP_NAME            "Coffee & Restroom Sensor App"




//*****************************************************************************
//
// Globals used by the coffee measurement interrupt handler.
//
//*****************************************************************************
static volatile unsigned int coffeeIntFound = 0;




//****************************************************************************
//
// Globals used by tilt sensor interrupt handler
//
//*****************************************************************************

volatile unsigned long ulTiltInts;
unsigned long g_ulTiltsBase = GPIOA1_BASE;




//*****************************************************************************
//Globals used by the program
//
//
//*****************************************************************************

float mov_avgResult;
const char* TILT1_ID = "1";
const char* TILT2_ID = "2";
const char* TILT3_ID = "3";
Tilt_Struct tilt1, tilt2, tilt3;
LDR_Struct LDR1, LDR2, LDR3;
HTTPCli_Struct httpClient;
s_PostData NO_LDR;





//*****************************************************************************
//
//! The interrupt handler for the LDR interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************

void
LDRIntHandler(void)
{
    LDR_IF_InterruptClear();
    coffeeIntFound = true;
}



//*****************************************************************************
//
//! The interrupt handler for the Tilt interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void TiltIntHandler(void) {

    ulTiltInts  = Tilt_IF_IntStatus(g_ulTiltsBase, false);

    if(ulTiltInts & tilt1.uiIntFlag){
        Tilt_IF_IntClearNDisable(tilt1);
        tilt1.val_InInterrupt = Tilt_IF_ReadValueInt(tilt1);
        tilt1.intFound = true;
    }
    if(ulTiltInts & tilt2.uiIntFlag){
        Tilt_IF_IntClearNDisable(tilt2);
        tilt2.val_InInterrupt = Tilt_IF_ReadValueInt(tilt2);
        tilt2.intFound = true;
    }
    if(ulTiltInts & tilt3.uiIntFlag){
        Tilt_IF_IntClearNDisable(tilt3);
        tilt3.val_InInterrupt = Tilt_IF_ReadValueInt(tilt3);
        tilt3.intFound = true;
    }
}




int main()
{
    long lRetVal = -1;
    //
    // Board Initialization
    //
    BoardInit();

    // Configure the pinmux settings for the peripherals exercised
    //
    PinMuxConfig();

    //
    // Configuring UART
    //
    InitTerm();

    //InitUart();


    //
    // Display banner
    //
    DisplayBanner(APP_NAME);

    InitializeAppVariables();


    HTTP_IF_ConfigureNEnable(&httpClient);


    Tilt_IF_ConfigureNIntEnable(&tilt1, TILT1GPIO, TILT1_ID, TiltIntHandler);
    Tilt_IF_ConfigureNIntEnable(&tilt2, TILT2GPIO, TILT2_ID, TiltIntHandler);
    Tilt_IF_ConfigureNIntEnable(&tilt3, TILT3GPIO, TILT3_ID, TiltIntHandler);


    LDR_IF_ConfigureNEnable(&LDR1, LDR1PIN, LDR1_COFFEE_POT,
                            INDICATE_HIGH_LEVEL, LDR1_TRESHOLD_LEVEL, LDRIntHandler);

    LDR_IF_ConfigureNEnable(&LDR2, LDR2PIN, LDR2_COFFEE_POT,
                            INDICATE_MID_LEVEL, LDR2_TRESHOLD_LEVEL, LDRIntHandler);

    LDR_IF_ConfigureNEnable(&LDR3, LDR3PIN, LDR3_COFFEE_POT,
                            INDICATE_LOW_LEVEL, LDR3_TRESHOLD_LEVEL, LDRIntHandler);

    LDR_IF_SetFields(&NO_LDR, LDR1_COFFEE_POT, INDICATE_EMPTY_LEVEL);


    while(1)
    {
        if(coffeeIntFound == 1) {


            /* OPTION I */

           // LDR_IF_SetValue(&LDR1, LDR_IF_ReadValue(LDR1.uiChannel));
           // LDR_IF_SetValue(&LDR2, LDR_IF_ReadValue(LDR2.uiChannel));
            LDR_IF_SetValue(&LDR3, LDR_IF_ReadValue(LDR3.uiChannel));


            /* OPTION II */

            //UART_PRINT("\n\r");
            //char gg[5];
            //MessageUART("1");
            //GetCmdUart(gg, 5 );



            if(LDR3.value <= LDR3.tresholdVal) {
                UART_PRINT("\n\r\n\r\n\r\n\rCoffee is high. \n\r\n\r\n\r");
                UART_PRINT("LDR3 Voltage level: %f\n\r" , LDR3.value);
                lRetVal = HTTP_IF_LDRSendPost(LDR3, &httpClient);

                if(lRetVal < 0)
                {
                    UART_PRINT("HTTP Post failed.\n\r");
                }
                if(lRetVal == HTTP_UNAUTHORIZED) {
                    SetAuthentication(&httpClient);
                    HTTP_IF_LDRSendPost(LDR3, &httpClient);
                }
            }

//            else if(LDR2.value == LDR2.tresholdVal) {
//                UART_PRINT("Coffee Level is between %%68 - %%35\n\r");
//                UART_PRINT("LDR2 Voltage level: %f\n\r" , LDR2.value);
//                HTTP_URISet(POST_COFFEE_URI);
//                HTTP_PostDataSet(LDR2.s_data);
//                UART_PRINT("Data to send is\n\r %s\n\r", POST_DATA);
//                lRetVal = HTTPPostMethod(&httpClient);
//                UART_PRINT("HTTP Post Begin:\n\r");
//                if(lRetVal < 0)
//                {
//                    UART_PRINT("HTTP Post failed.\n\r");
//                }
//            }
//
//            else if(LDR1.value == LDR1.tresholdVal) {
//                UART_PRINT("Coffee Level is between %%5 - %%35\n\r");
//                UART_PRINT("LDR Voltage level: %f\n\r" , LDR1.value);
//                HTTP_URISet(POST_COFFEE_URI);
//                HTTP_PostDataSet(LDR1.s_data);
//                UART_PRINT("Data to send is\n\r %s\n\r", POST_DATA);
//                lRetVal = HTTPPostMethod(&httpClient);
//                UART_PRINT("HTTP Post Begin:\n\r");
//                if(lRetVal < 0)
//                {
//                    UART_PRINT("HTTP Post failed.\n\r");
//                }
//            }

            else {
                UART_PRINT("\n\r\n\rCoffee is low.\n\n\r\r");
                HTTP_URISet(POST_COFFEE_URI);
                HTTP_PostDataSet(NO_LDR);
                UART_PRINT("Data to send is\n\r %s\n\r", POST_DATA);
                UART_PRINT("HTTP Post Begin:\n\r");
                lRetVal = HTTPPostMethod(&httpClient);
                if(lRetVal < 0)
                {
                    UART_PRINT("HTTP Post failed.\n\r");
                }
            }
            coffeeIntFound = 0;
    }


        if(tilt1.intFound == true) {
            while(1) {
                UtilsDelay(1000000);
                tilt1.val_Stabilized = Tilt_IF_Moving_Avg(Tilt_IF_ReadValueInt(tilt1));

                if(tilt1.val_Stabilized == 0 || tilt1.val_Stabilized == 1){
                    break;
                }
            }

            if(tilt1.val_Stabilized == tilt1.val_InInterrupt) {
                UART_PRINT("Tilt2 Edge is stabilized to %d.\n\r", tilt1.val_Stabilized);
                Tilt_IF_SetValue(&tilt1, tilt1.val_Stabilized);
                HTTP_IF_SendTiltPost(tilt1, &httpClient);
            }

            Tilt_IF_IntClearNEnable(&tilt1);
        }

        if(tilt2.intFound == true) {
            while(1) {
                UtilsDelay(1000000);
                tilt1.val_Stabilized = Tilt_IF_Moving_Avg(Tilt_IF_ReadValueInt(tilt1));

                if(tilt2.val_Stabilized == 0 || tilt2.val_Stabilized == 1){
                    break;
                }
            }

            if(tilt2.val_Stabilized == tilt2.val_InInterrupt) {
                UART_PRINT("Tilt2 Edge is stabilized to %d.\n\r", tilt2.val_Stabilized);
                Tilt_IF_SetValue(&tilt2, tilt2.val_Stabilized);
                HTTP_IF_SendTiltPost(tilt2, &httpClient);
            }

            Tilt_IF_IntClearNEnable(&tilt2);
        }



        if(tilt3.intFound == true) {
            while(1) {
                UtilsDelay(1000000);
                tilt3.val_Stabilized = Tilt_IF_Moving_Avg(Tilt_IF_ReadValueInt(tilt1));

                if(tilt3.val_Stabilized == 0 || tilt3.val_Stabilized == 1){
                    break;
                }
            }

            if(tilt3.val_Stabilized == tilt3.val_InInterrupt) {
                UART_PRINT("Tilt2 Edge is stabilized to %d.\n\r", tilt3.val_Stabilized);
                Tilt_IF_SetValue(&tilt3, tilt3.val_Stabilized);
                HTTP_IF_SendTiltPost(tilt3, &httpClient);
            }

            Tilt_IF_IntClearNEnable(&tilt3);
        }

    }
}


