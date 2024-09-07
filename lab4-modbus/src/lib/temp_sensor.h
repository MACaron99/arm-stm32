
#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_


#define TEMP_SENSOR_OUT             -128
#define TEMP_SENSOR_ERR             TEMP_SENSOR_OUT

#define TEMP_SENSOR_BROKEN          -127
#define TEMP_SENSOR_SHORTEN         -126

#define TEMP_SENSOR_ERROR_RANGE     -80

bool is_temperature_valid( s08 temperature );

s08  temperature_interpolation( u16 adc_value );

#endif /* TEMP_SENSOR_H_ */

