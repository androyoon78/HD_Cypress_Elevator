/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#define _CONFIG_DEBUGMSG_

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "project.h"

// Functions
void Calibration();
uint16 CalibationSensor(uint tryCnt, int minValue, int MaxValue);
int GetDistanceTouchedSensor();
void NontouchedSensorDetection(uint16 result0);
void DetectButton();
void ToggleLED();
uint16 GetSensorValue();

void DamoaIntPrintf(int value)
{
#ifdef _CONFIG_DEBUGMSG_
    DEBUG_UART_UartPutString("\r\n");
    char buf[10] = {0, };
    sprintf(buf, "%d", value);
    DEBUG_UART_UartPutString(buf);
    DEBUG_UART_UartPutString("\r\n");
#else
    value = 0;
#endif
}

/* [] END OF FILE */
