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

#define __NUMBER_FIVE__

#include "Custom.h"

#ifdef __NUMBER_FIVE__
#define NEAR_THRESOLD       8
#else
#define NEAR_THRESOLD       16             //
#endif
#define TOUCHED_THRESOLD    200
#define PUSHED_THRESOLD     300

#define Wait_Time 100
#define CALIBRATION_COUNT 300

uint8 ADC = 0;
uint16 result0 = 0;
uint16 calibatedTouchValue = 0;
uint32 noTouchedCount = 0;

bool StatusButton = 0;
bool Nothing = 0;
bool Touched = 0;
bool Closed  = 0;
bool Pushed  = 0;

uint pushedButtonCount = 0;

unsigned int stayLEDcount = 0;

void DamoaIntPrintf(int value);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    
    // DEBUG UART START
    DEBUG_UART_Start();
    
    // ADC START & INIT
    ADC_Start();
    
    // ALL PIN DEFAULT LOW
    //LED_Write(0);
    //IMA09_CE_Write(0);
    //IMA09_ENV_Write(0);
    //SW_OUT_Write(0);
        
    //TEST MESSAGE OUT
    DEBUG_UART_UartPutString(" HW VERSION : V1.0 " );
    DEBUG_UART_UartPutString("2021.03.28\n");
    
    //IMA09 SETTING 

    IMA09_CE_Write(1);
    IMA09_ENV_Write(1);
    CyDelay(500);
    
    LED_Write(1);
    CyDelay(500);
    Calibration();
    LED_Write(0);

    LED_Write(1);
    CyDelay(500);
    Calibration();
    LED_Write(0);
    
    CyDelay(500);
    
    for(;;)
    {
        // Get Button Status
        DetectButton();
        
        if (Pushed == false)
        {
            // Get Non-Touched Sensor Status
            result0 = GetDistanceTouchedSensor();
            if (result0 <= NEAR_THRESOLD)
            {
               // DEBUG_UART_UartPutString(" Non Action\r\n" );
                if (noTouchedCount > 100000)
                {
                    Calibration();
                    noTouchedCount = 0;
                }
                    
                noTouchedCount++;
                Nothing = true;
                Touched = false;
                Pushed = false;
                stayLEDcount = 0;
            }
            
            if (result0 >  TOUCHED_THRESOLD)
            {
               // DEBUG_UART_UartPutString(" Touched Action\r\n" );
                //ToggleLED();
                Nothing = false;
                Touched = true;
                Pushed = false;
                stayLEDcount = 0;
            }

            if ((result0 > NEAR_THRESOLD) && ( result0 <= TOUCHED_THRESOLD))
            {
                //DEBUG_UART_UartPutString(" Closed Action\r\n" );
                //DamoaIntPrintf(stayLEDcount);
                //DamoaIntPrintf(result0);

                if ((stayLEDcount > 300) && (stayLEDcount < 600))
                {
                    if (StatusButton == true)
                    {
                        DEBUG_UART_UartPutString(" ::: \r\n");
                        LED_Write(0);
                        StatusButton = false;
                        //DamoaIntPrintf(result0);
                        //DEBUG_UART_UartPutString(" : LED Off \r\n " );
                        Calibration();

                    }
                    else
                    {
                        DEBUG_UART_UartPutString(" ;;; \r\n");
                        LED_Write(1);
                        StatusButton = true;
                        //DamoaIntPrintf(result0);
                        //DEBUG_UART_UartPutString(" : LED On \r\n " );
                    }

                    stayLEDcount = 0;
                }
                           
                stayLEDcount++;
                
                Nothing = false;
                Touched = false;
                Pushed = false;
            }
            
            CyDelay(1);
            //NontouchedSensorDetection(result0);
        }

        
    }
}

void Calibration()
{
    int minValue = 2000;
    int MaxValue = 0;
    uint calibCount = CALIBRATION_COUNT;
 
    //DEBUG_UART_UartPutString(" === Calibration ===\r\n" );
    
    calibatedTouchValue = CalibationSensor(calibCount, minValue, MaxValue);
    
    if ((calibatedTouchValue < MaxValue) && (calibatedTouchValue > minValue)) {
        // Good path
        return;
    } else {
        // try calibration again with half 
        calibCount = CALIBRATION_COUNT / 2;
        calibatedTouchValue = CalibationSensor(calibCount, minValue, MaxValue);
    }
    
    //DamoaIntPrintf(calibatedTouchValue);
    //DEBUG_UART_UartPutString("\r\n === Finished Calibration ===\r\n" );
    DEBUG_UART_UartPutString("+++");
}

uint16 CalibationSensor(uint tryCnt, int minValue, int MaxValue)
{  
    while(tryCnt < 1)
    {
        if (GetSensorValue() > MaxValue)
            MaxValue = GetSensorValue();
        
        if (GetSensorValue() < minValue)
            minValue = GetSensorValue();
        tryCnt--;
    }
    
    return GetSensorValue();
}

uint16 GetSensorValue()
{
    uint16 value;
    value = ADC_ReadResult_mVolts(0) * 2;    
    return value;
}

int GetDistanceTouchedSensor()
{
    // Reference value : calibatedTouchValue
   
    int distance = calibatedTouchValue - GetSensorValue();    
    distance = distance > 0? distance:distance*(-1);   
    //DamoaIntPrintf(distance);
    
    return distance;
}

void NontouchedSensorDetection(uint16 result0)
{
    
        result0 = GetDistanceTouchedSensor();
        if ((result0 <= NEAR_THRESOLD ) || (result0 > TOUCHED_THRESOLD))
        {
            stayLEDcount = 0;
        } else {
            stayLEDcount++;
//            DamoaIntPrintf(stayLEDcount);
        }

        if (stayLEDcount > 20)
        {   
           
            if (StatusButton == true)
            {
                StatusButton = false;
            }
            else
            {
                StatusButton = true;
            }
            stayLEDcount = 0;
        }
        
        if (StatusButton == true)
        {
            LED_Write(1);
        }
        
        if (StatusButton == false)
        {
            LED_Write(0);
        }

        CyDelay(20);
}

void DetectButton()
{
    //READ EXT IO PIN STAT
    if (SW_IN_Read() == 0)
    {
        if (pushedButtonCount < 1)
        {
            //DEBUG_UART_UartPutString(" Pushed Button\r\n" );
            ToggleLED();
        }
        pushedButtonCount++;
        Pushed = true;
    }
    else
    {
        if (pushedButtonCount > 1)
        {
            //DEBUG_UART_UartPutString(" Released Button\r\n" );
        }
        Pushed = false;
        pushedButtonCount = 0;
    }
}

void ToggleLED()
{
    LED_Write(!LED_Read());
}

/* [] END OF FILE */
