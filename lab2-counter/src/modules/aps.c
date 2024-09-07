
#include <system.h>
#include <drivers/adc.h>
#include <system/sys_timer.h>

#include "aps.h"


#if ( BOARD_TYPE != 713144 )
#define APS_OUTPUT_VOLTAGE_COEFF    (   73 )
#else
#define APS_OUTPUT_VOLTAGE_COEFF    (   60 )
#endif


typedef struct {
    uint16_t  input_voltage;
    uint16_t  output_voltage;
    uint8_t  temperature;
    int8_t  power_good;
    uint8_t  flags;
} aps_t;


static aps_t  aps;



int8_t aps_is_power_good( void )
{
#if ( APS_VOLTAGE_CHECK != 0 )
    return aps.power_good;
#else
    return true;
#endif
}


uint16_t aps_output_voltage_get( void )
{
    return aps.output_voltage;
}


void aps_output_voltage_check( void )
{
#if ( APS_VOLTAGE_CHECK != 0 )
    if( aps.output_voltage > APS_STARTUP_VOLTAGE_HIGH )
    {
        sys_error_set(ERR_AUX_VOLTAGE_TOO_HIGH);
    }
    else
    if( aps.output_voltage < APS_STARTUP_VOLTAGE_LOW )
    {
        sys_error_set(ERR_AUX_VOLTAGE_TOO_LOW);
    }
#endif
}


static void aps_timeproc( void )
{
    static int8_t  power_good;
    static int16_t  voltage_acc;
    int16_t  voltage;

    //
    // todo: add median filter
    //
    voltage = adc_value_get(ADC_AUX_SUPPLY_CHANNEL) >> 2;
    //voltage = middle_of_3(prev_voltage, aps.output_voltage, voltage);

    if( voltage > 992 )
    {
        voltage = 992;
    }

    voltage = (voltage * APS_OUTPUT_VOLTAGE_COEFF) >> 8;
    voltage_acc += voltage - aps.output_voltage;
    voltage = voltage_acc >> 2;

    //
    // Check power good
    //
    if( voltage < APS_SHUTDOWN_VOLTAGE_LOW )
    {
        power_good = false;
    }
    else
    if( (voltage >= APS_STARTUP_VOLTAGE_LOW) && (voltage <= APS_STARTUP_VOLTAGE_HIGH) )
    {
        power_good = true;
    }
    else
    if( voltage > APS_SHUTDOWN_VOLTAGE_HIGH )
    {
        power_good = false;
    }

//    prev_voltage = aps.output_voltage;
    aps.output_voltage = voltage;
    aps.power_good = power_good;
}


void aps_init( void )
{
    sys_timer_timeproc_add(&aps_timeproc, 1);
}


