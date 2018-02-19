/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65
        Device            :  PIC16F1619
        Driver Version    :  2.00
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <stdio.h>
#include <stdint.h>
#include "mcc_generated_files/mcc.h"
#include "./I2C_user.h"
#include "./BME280_driver/bme280.h"

//#define FORCED_MODE
#define MAINLOOP_DELAY_MS    1000   // mSec

void I2C_UserInit(void);
void Bme280_UserInit(void);
void user_delay_ms(uint32_t period);

#ifdef FORCED_MODE
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev);
int8_t get_data_forced_mode(struct bme280_dev *dev);
#else
int8_t stream_sensor_data_normal_mode(struct bme280_dev *dev);
int8_t get_data_normal_mode(struct bme280_dev *dev);
#endif

#define LED_D4_ON()     {D4_LAT=1;}
#define LED_D4_OFF()    {D4_LAT=0;}
struct bme280_dev BME280Dev;

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
//    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
//    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    
    printf("Program Start...\r\n");
    LED_D4_ON();
    
    I2C_UserInit();
    Bme280_UserInit();
 
    LED_D4_OFF();

#ifdef FORCED_MODE    
    stream_sensor_data_forced_mode(&BME280Dev);
#else
    stream_sensor_data_normal_mode(&BME280Dev);
#endif
    
    while (1)
    {
#ifdef FORCED_MODE    
        get_data_forced_mode(&BME280Dev);
#else
        get_data_normal_mode(&BME280Dev);
#endif
        user_delay_ms(MAINLOOP_DELAY_MS);
        D4_Toggle();
    }
}
void debug_error(void);


void Bme280_UserInit(void)
{
    int8_t rslt = BME280_OK;

    BME280Dev.dev_id = BME280_I2C_ADDR_PRIM;
    BME280Dev.intf = BME280_I2C_INTF;
    BME280Dev.read = user_i2c_read;
    BME280Dev.write = user_i2c_write;
    BME280Dev.delay_ms = user_delay_ms;

    rslt = bme280_init(&BME280Dev);  
    
    if(rslt!=BME280_OK){
        debug_error();
    }
}

void user_delay_ms(uint32_t period)
{
//    D7_LAT=1; // for check
    for(uint32_t i=0;i<period;i++){
        TMR1_Reload(); // 1msec value reload and start timer
        PIR1bits.TMR1IF = 0;
        TMR1_StartTimer();
        while(!TMR1_HasOverflowOccured());
        TMR1_StopTimer();
    }
//    D7_LAT=0; // for check    
}


void debug_error(void)
{
    while(1){
        LED_D4_ON();
        __delay_ms(100);
        LED_D4_OFF();
        __delay_ms(100);
    }
}

void print_sensor_data(struct bme280_data *comp_data);
#ifdef FORCED_MODE
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t settings_sel;
 
    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
    dev->settings.filter = BME280_FILTER_COEFF_16;
 
    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
 
    rslt = bme280_set_sensor_settings(settings_sel, dev);
 
    printf("Temperature, Pressure, Humidity by forced mode.\r\n");
    
    return rslt;

}

int8_t get_data_forced_mode(struct bme280_dev *dev)
{
    int8_t rslt;
    struct bme280_data comp_data;
    
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
    /* Wait for the measurement to complete and print data @25Hz */
    dev->delay_ms(40);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
    print_sensor_data(&comp_data);    
    
    return rslt;
}

#else

int8_t stream_sensor_data_normal_mode(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t settings_sel;


    /* Recommended mode of operation: Indoor navigation */
    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
    dev->settings.filter = BME280_FILTER_COEFF_16;
    dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

    settings_sel = BME280_OSR_PRESS_SEL;
    settings_sel |= BME280_OSR_TEMP_SEL;
    settings_sel |= BME280_OSR_HUM_SEL;
    settings_sel |= BME280_STANDBY_SEL;
    settings_sel |= BME280_FILTER_SEL;
    rslt = bme280_set_sensor_settings(settings_sel, dev);
    rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

    printf("Temperature, Pressure, Humidity by normal mode.\r\n");

    return rslt;
}
int8_t get_data_normal_mode(struct bme280_dev *dev)
{
    int8_t rslt;
    struct bme280_data comp_data;
    
    /* Delay while the sensor completes a measurement */
    //        dev->delay_ms(70);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
    print_sensor_data(&comp_data);
    
    return rslt;
}
#endif

void print_sensor_data(struct bme280_data *comp_data)
{
        printf("%ld.%02ld[C], %ld.%02ld[hPa], %ld.%03ld[%%]\r\n",   comp_data->temperature/100,comp_data->temperature%100,
                                                        comp_data->pressure/100,comp_data->pressure%100,
                                                        comp_data->humidity/1000,comp_data->humidity%1000);
}

/**
 End of File
*/