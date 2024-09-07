
#include <system.h>
#include <modbus-slave/umodbus_enron.h>
#include <drivers/storage.h>

#include "sys_param.h"



#define SPF_STORE                      ( 1 << 0 )
#define SPF_AUTO_SAVE                  ( 1 << 1 )



typedef struct
{
    uint16_t  param;
    int16_t  min;
    int16_t  max;
    int16_t  inc;
    int16_t  def;

    uint8_t  flags;

} setup_param_t;


static uint16_t sys_param_std[PARAM_COUNT];
static int8_t sys_param_is_changed;
static int8_t sys_param_save_all_signal;
setup_param_t setup_params[] =
{
#if ( DEV_TYPE == DEV_TYPE_DCTRL_PSU )
        {
            .param = OUTPUT_VOLTAGE_SETUP,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_CURRENT_LIMIT,
            .min = CURRENT_SET_MIN,
            .max = CURRENT_SET_MAX,
            .inc = CURRENT_SET_INC,

            .def = CURRENT_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_VOLTAGE_SETUP_1,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_1_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_VOLTAGE_SETUP_2,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_2_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_VOLTAGE_SETUP_3,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_3_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_VOLTAGE_SETUP_F,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_F_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

#elif ( DEV_TYPE == DEV_TYPE_CHARGER_FH )
        {
            .param = OUTPUT_VOLTAGE_SETUP_FAST,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_VOLTAGE_SETUP_HOLD,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_CURRENT_LIMIT,
            .min = CURRENT_SET_MIN,
            .max = CURRENT_SET_MAX,
            .inc = CURRENT_SET_INC,

            .def = CURRENT_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },
#else
        {
            .param = OUTPUT_VOLTAGE_SETUP,
            .min = VOLTAGE_SET_MIN,
            .max = VOLTAGE_SET_MAX,
            .inc = VOLTAGE_SET_INC,

            .def = VOLTAGE_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_CURRENT_LIMIT,
            .min = CURRENT_SET_MIN,
            .max = CURRENT_SET_MAX,
            .inc = CURRENT_SET_INC,

            .def = CURRENT_SET_DEF,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = OUTPUT_VOLTAGE_SETUP_CTRL,
            .min = VOLTAGE_SET_MIN_CTRL,
            .max = VOLTAGE_SET_MAX_CTRL,
            .inc = 1,

            .def = VOLTAGE_SET_MIN_CTRL,

            .flags  = 0,
        },

        {
            .param = OUTPUT_CURRENT_LIMIT_CTRL,
            .min = CURRENT_SET_MIN_CTRL,
            .max = CURRENT_SET_MAX_CTRL,
            .inc = 1,

            .def = CURRENT_SET_MIN_CTRL,

            .flags  = 0,
        },

        {
            .param = CONV_ENABLE_CTRL,
            .min = 0,
            .max = 1,
            .inc = 1,

            .def = 1,

            .flags  = 0,
        },
#endif

        {
            .param = CALIBRATION_COEFF_1,
            .min = 0,
            .max = INT16_MAX,
            .inc = 1,

            .def = 8192,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = CALIBRATION_COEFF_2,
            .min = 0,
            .max = INT16_MAX,
            .inc = 1,

            .def = 8192,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = CALIBRATION_COEFF_3,
            .min = INT16_MIN,
            .max = INT16_MAX,
            .inc = 1,

            .def = 0,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = CALIBRATION_COEFF_4,
            .min = 0,
            .max = INT16_MAX,
            .inc = 1,

            .def = 0,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = TCOMP_MAX_BOUNDARY,
            .min = 0,
            .max = 999,
            .inc = 1,

            .def = 0,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = TCOMP_VALUE,
            .min = 0,
            .max = 999,
            .inc = 1,

            .def = 0,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = MODBUS_ADDRESS,
            .min = 1,
            .max = 99,
            .inc = 1,

            .def = 1,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = MODBUS_BAUDRATE,
            .min = 0,
            .max = 7,
            .inc = 1,

            .def = 4,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },

        {
            .param = MODBUS_PARITY,
            .min = 0,
            .max = 2,
            .inc = 1,

            .def = 2,

            .flags  = SPF_AUTO_SAVE | SPF_STORE,
        },
};
#define SETUP_PARAMS_COUNT          ( sizeof(setup_params) / sizeof(setup_params[0]) )



uint16_t sys_param_set_via_protocol( uint8_t param, uint16_t value )
{
    switch( param )
    {
#if ( DEV_TYPE == DEV_TYPE_CHARGER_FH )
    case OUTPUT_VOLTAGE_SETUP_HOLD:
        if( value < VOLTAGE_SET_MIN )
        {
            value = VOLTAGE_SET_MIN;
        }
        else
        if( value > sys_param_std[OUTPUT_VOLTAGE_SETUP_FAST] )
        {
            value = sys_param_std[OUTPUT_VOLTAGE_SETUP_FAST];
        }
        break;

    case OUTPUT_VOLTAGE_SETUP_FAST:
        if( value < sys_param_std[OUTPUT_VOLTAGE_SETUP_HOLD] )
        {
            value = sys_param_std[OUTPUT_VOLTAGE_SETUP_HOLD];
        }
        else
        if( value > VOLTAGE_SET_MAX )
        {
            value = VOLTAGE_SET_MAX;
        }
        break;
#endif

    case OUTPUT_VOLTAGE_SETUP:
#if ( DEV_TYPE == DEV_TYPE_DCTRL_PSU )
    case OUTPUT_VOLTAGE_SETUP_1:
    case OUTPUT_VOLTAGE_SETUP_2:
    case OUTPUT_VOLTAGE_SETUP_3:
    case OUTPUT_VOLTAGE_SETUP_F:
#endif
        if( value < VOLTAGE_SET_MIN )
        {
            value = VOLTAGE_SET_MIN;
        }
        else
        if( value > VOLTAGE_SET_MAX )
        {
            value = VOLTAGE_SET_MAX;
        }
        break;
    case OUTPUT_CURRENT_LIMIT:
        if( value < CURRENT_SET_MIN )
        {
            value = CURRENT_SET_MIN;
        }
        else
        if( value > CURRENT_SET_MAX )
        {
            value = CURRENT_SET_MAX;
        }
        break;

#if ( DEV_EXTMODBUS == 1 )
#error check it...
    case CONV_ENABLE_CTRL:
        conv_enable(value ? 1 : 0);
        break;
#endif
    }

    return sys_param_set(param, value);
}


//static uint8_t sys_param_multiple_set( uint16_t index, uint16_t len, uint8_t* data )
//{
//    uint8_t exception = 0;
//    uint16_t temp;
//
//    while( len > 0 )
//    {
//        temp  = *data++ << 8;
//        temp |= *data++;
//
//        sys_param_set_via_protocol(index, temp);
//
//        index++;
//        len--;
//    }
//
//    return exception;
//}


void sys_params_force_save_all( void )
{
    uint16_t param;
    uint16_t value;

    for( uint32_t i = 0; i < SETUP_PARAMS_COUNT; i++ )
    {
        param = setup_params[i].param;

        if( setup_params[i].flags & SPF_STORE )
        {
            value = sys_param_std[param];

            storage_write(param, value);
        }
    }
}


void sys_param_save_all( void )
{
    sys_param_save_all_signal = true;
}


uint16_t sys_param_load( uint16_t param )
{
    return storage_read(param);
}


void sys_param_save( uint16_t param )
{
    uint16_t value = sys_param_std[param];

    if( storage_read(param) != value )
    {
        storage_write(param, value);
    }
}


void sys_param_setup_init( void )
{

}


void sys_param_default_set( void )
{

}

void sys_param_save_standart( void )
{
}


void sys_param_setup_callback( void )
{
    sys_param_save_standart();
}


uint16_t sys_param_set( uint16_t param, int16_t value )
{
    uint16_t  accepted_value;
    uint16_t  index;

    accepted_value = sys_param_get(param);


    if( value != accepted_value )
    {
        for( index = 0; index < SETUP_PARAMS_COUNT; index++ )
        {
            if( setup_params[index].param == param )
            {
                break;
            }
        }

        if( index < SETUP_PARAMS_COUNT )
        {
            // todo: !!!!!!!!!!!
//            if( (value >= setup_params[index].min) &&
//                (value <= setup_params[index].max) )
            {
                if( value < setup_params[index].min )
                {
                    value = setup_params[index].min;
                }
                else if( value > setup_params[index].max )
                {
                    value = setup_params[index].max;
                }

                if( setup_params[index].flags & SPF_AUTO_SAVE )
                {
                    if( accepted_value != value )
                    {
                        sys_param_is_changed = 1;
                    }
                }

                accepted_value = value;
            }

        }
        else
        {
            accepted_value = value;
        }

        sys_param_std[param] = accepted_value;
    }

    return accepted_value;
}


uint16_t sys_param_step_get( uint16_t param )
{
    uint8_t index;
    uint16_t step = 0;

    for( index = 0; index < SETUP_PARAMS_COUNT; index++ )
    {
        if( setup_params[index].param == param )
        {
            break;
        }
    }

    if( index < SETUP_PARAMS_COUNT )
    {
        step = setup_params[index].inc;
    }

    return step;
}


uint16_t sys_param_adjust( uint16_t param, int16_t ajust_value )
{
    return sys_param_set_via_protocol(param, sys_param_std[param] + ajust_value);
}


uint16_t sys_param_get( uint16_t param )
{
    uint16_t value;

    value = sys_param_std[param];

    return value;
}


void sys_param_inc( uint16_t param )
{
    sys_param_std[param] += 1;
}


static void sys_param_proc( void )
{
    static int8_t stage;
    static timer_t  timer;
    static uint16_t  index;


//    if( sys_param_get(SYS_PARAM_SAVE_SET) )
//    {
//        sys_param_set(SYS_PARAM_SAVE_SET, 0);
//        sys_param_save_all_signal = true;
//    }

    switch( stage )
    {
    case 0:
        if( sys_param_is_changed || sys_param_save_all_signal )
        {
            sys_param_is_changed = 0;
            timer = sys_timer_tick_get();
            stage++;
        }
        break;

    case 1:
        if( sys_param_is_changed )
        {
            sys_param_is_changed = 0;
            timer = sys_timer_tick_get();
        }

        if( sys_timer_tick_diff_get(timer) >= _MS(5000) )
        {
            stage++;
        }

        if( sys_param_save_all_signal )
        {
            sys_param_save_all_signal = false;

            stage = 3;
        }
        break;

    case 2:
        // auto save
        if( setup_params[index].flags & SPF_AUTO_SAVE )
        {
            sys_param_save(setup_params[index].param);
        }

        if( ++index >= SETUP_PARAMS_COUNT )
        {
            index = 0;
            stage = 0;
        }
        break;

    case 3:
        // force save
        if( setup_params[index].flags & SPF_STORE )
        {
            sys_param_save(setup_params[index].param);
        }

        if( ++index >= SETUP_PARAMS_COUNT )
        {
            index = 0;
            stage = 0;
        }
        break;
    }
}


void sys_param_init( void )
{
    storage_init(sys_params_force_save_all);

//    umodbus_address_map_register(1000, PARAM_COUNT, sys_param_std, &sys_param_multiple_set);

    for( uint32_t i = 0; i < SETUP_PARAMS_COUNT; i++ )
    {
        sys_param_std[setup_params[i].param] = setup_params[i].def;
        sys_param_set(setup_params[i].param, sys_param_load(setup_params[i].param));
    }

    sys_service_add(&sys_param_proc);
}


