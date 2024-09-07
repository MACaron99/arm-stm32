
#include <system.h>
#include <modbus-slave/umodbus_enron.h>

#include "sys_param_ext.h"


static  uint16_t  sys_param_ext[PARAM_EXTENDED_COUNT];



uint16_t sys_param_ext_get( uint8_t param )
{
    int16_t value;

    value = sys_param_ext[param];

    return value;
}


uint16_t sys_param_ext_set( uint8_t param, uint16_t value )
{
    uint16_t accepted_value;

    accepted_value = sys_param_ext[param];

    switch( param )
    {
//  case xxx:
//      if( value >= xxx && value <= xxx )
//          accepted_value = value;
//      break;

    default:
        accepted_value = value;
    }

    sys_param_ext[param] = accepted_value;

    return accepted_value;
}


//static uint8_t sys_param_ext_multiple_set( uint16_t index, uint16_t len, uint8_t* data )
//{
//    uint8_t exception = UMODBUS_EXCEPTION_NONE;
//    uint16_t temp;
//
//    while( len > 0 )
//    {
//        temp = *data++ << 8;
//        temp |= *data++;
//
//        sys_param_ext_set(index, temp);
//
//        index++;
//        len--;
//    }
//
//    return exception;
//}


void sys_param_ext_init( void )
{
//    umodbus_address_map_register(2000, PARAM_EXTENDED_COUNT, sys_param_ext, &sys_param_ext_multiple_set);
}

