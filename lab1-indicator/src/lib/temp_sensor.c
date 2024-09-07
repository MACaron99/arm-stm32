
#include <system.h>

#include "temp_sensor.h"


#if ( BOARD_TYPE != 713144 )
// --- WARNING --- WARNING --- WARNING ---
// This table is for 10kOhm upper resistor
// from 0 to 100 with dt = 5*C (for 10k resistor in upper and lower leg)
static const uint16_t  vishay_ntc10k_lookuptable[] =
{
     26 /* -50 */,  34 /* -45 */,  46 /* -40 */,  60 /* -35 */,
     77 /* -30 */,  98 /* -25 */, 124 /* -20 */, 153 /* -15 */,
    187 /* -10 */, 226 /*  -5 */, 268 /*   0 */, 314 /*   5 */,
    362 /*  10 */, 412 /*  15 */, 462 /*  20 */, 512 /*  25 */,
    560 /*  30 */, 606 /*  35 */, 650 /*  40 */, 690 /*  45 */,
    727 /*  50 */, 760 /*  55 */, 790 /*  60 */, 817 /*  65 */,
    841 /*  70 */, 862 /*  75 */, 881 /*  80 */, 897 /*  85 */,
    912 /*  90 */, 925 /*  95 */, 936 /* 100 */, 945 /* 105 */,
    954 /* 110 */, 961 /* 115 */, 968 /* 120 */, 974 /* 125 */
};
#else
// This table is for 6.2kOhm upper resistor
// from 0 to 100 with dt = 5*C
static const uint16_t vishay_ntc10k_lookuptable[] =
{
     16 /* -50 */,  22 /* -45 */,  29 /* -40 */,  38 /* -35 */,
     49 /* -30 */,  63 /* -25 */,  80 /* -20 */, 101 /* -15 */,
    125 /* -10 */, 153 /*  -5 */, 185 /*   0 */, 220 /*   5 */,
    259 /*  10 */, 302 /*  15 */, 346 /*  20 */, 392 /*  25 */,
    439 /*  30 */, 485 /*  35 */, 531 /*  40 */, 575 /*  45 */,
    617 /*  50 */, 657 /*  55 */, 694 /*  60 */, 727 /*  65 */,
    758 /*  70 */, 786 /*  75 */, 812 /*  80 */, 834 /*  85 */,
    855 /*  90 */, 873 /*  95 */, 889 /* 100 */, 903 /* 105 */,
    915 /* 110 */, 927 /* 115 */, 937 /* 120 */, 945 /* 125 */
};
#endif
#define LOOCKUPTABLE_FIRST_VALUE    -50
#define LOOCKUPTABLE_LAST_VALUE     125
#if 1
#define LOOCKUPTABLE_VALUES         36
#else
#define LOOCKUPTABLE_VALUES         (sizeof(vishay_ntc10k_lookuptable) /     \
                                     sizeof(vishay_ntc10k_lookuptable[0]))
#endif


bool is_temperature_valid( s08 temperature )
{
    bool  is_valid = false;

    if( temperature >= TEMP_SENSOR_ERROR_RANGE )
        is_valid = true;

    return is_valid;
}


static inline
uint16_t  approx_linear( uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t x )
{
    u16 y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);

    return y;
}

int8_t  temperature_interpolation( uint16_t adc_value )
{
    uint16_t  x = vishay_ntc10k_lookuptable[0];
    uint16_t  y = vishay_ntc10k_lookuptable[LOOCKUPTABLE_VALUES-1];
    int8_t    temperature = TEMP_SENSOR_ERR;

    if( adc_value < 8 )
    {
        return TEMP_SENSOR_SHORTEN;
    }
    else
    if( adc_value > 1016 )
    {
        return TEMP_SENSOR_BROKEN;
    }

    adc_value = 1023 - adc_value;

    if( adc_value < x )
    {
        temperature = (LOOCKUPTABLE_FIRST_VALUE - 1);
    }
    else
    if( adc_value >= y )
    {
        temperature = (LOOCKUPTABLE_LAST_VALUE + 1);
    }
    else
    {
        for( int8_t i = 0; i < LOOCKUPTABLE_VALUES-1; i++ )
        {
            x  = vishay_ntc10k_lookuptable[i];
            y  = vishay_ntc10k_lookuptable[i+1];

            if( adc_value >= x && adc_value <  y )
            {
                temperature  = approx_linear(x, y, i*5, i*5+5, adc_value);
                temperature += LOOCKUPTABLE_FIRST_VALUE;
                break;
            }
        }
    }

    return temperature;
}


