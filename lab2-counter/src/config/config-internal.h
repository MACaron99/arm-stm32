
#ifndef CONFIG_INTERNAL_H_
#define CONFIG_INTERNAL_H_

//
// Measurement and coefficient
//
#ifndef COEFF_RESOLUTION
#define COEFF_RESOLUTION                (  13 ) // bits
#endif
#define COEFF_UNITY_VALUE               ( 1 << COEFF_RESOLUTION )

//
// Auxiliary Power Supply configuration
//
#ifndef APS_VOLTAGE_CHECK
#define APS_VOLTAGE_CHECK               (   0 )
#endif

#define APS_STARTUP_VOLTAGE_HIGH        ( 160 )
#define APS_STARTUP_VOLTAGE_LOW         ( 115 )

#define APS_SHUTDOWN_VOLTAGE_HIGH       ( 165 )
#define APS_SHUTDOWN_VOLTAGE_LOW        ( 112 )

//
// UI configuration
//
#define MENU_TIMEOUT                    (  6500 )
#define MENU_UPDATE_PERIOD              (   250 )

#define MENU_DISPLAY_MODBUS_ADDR        0
#define MENU_DISPLAY_OUTPUT_POWER       0     // 1 - Yes, 0 - No
#define MENU_DISPLAY_INT_TEMPERATURE    0
#define MENU_DISPLAY_INPUT_VOLTAGE      0

#if ( DEV_TYPE == DEV_TYPE_CHARGER )
#ifndef LED_PROTECTION
#define LED_PROTECTION                  IND_LED_3
#endif
#else
#ifndef LED_PROTECTION
#define LED_PROTECTION                  IND_LED_1
#endif
#endif

#ifndef MENU_TYPE
#define MENU_TYPE                       MT_SYSTEM
#endif

//
// Modbus configuration
//
#define REG_INPUT_START                 (  1000 )
#define REG_INPUT_NREGS                 ( PARAM_COUNT )

#define MB_FIXED_ADDRESS                (   0  )    // if left 0, than dynamic recognition
#define MB_SLAVE_ADDRESS_BASE           (   1  )


//
// Cooling system (FAN) configuration
//
#ifndef CONV_TEMPERATURE_SENSOR_COUNT
#define CONV_TEMPERATURE_SENSOR_COUNT   ( 2 )
#endif

#define FAN_PWM_FREQ                    ( 60000 )

#define FAN_TEST_TIME                   _MS(3500)

#define FAN_ON_TEMPERATURE              (    +5 )
#define FAN_OFF_TEMPERATURE             (     0 )


//
// Power converter configuration
//
#ifndef CONV_DEDICATED_ENABLE
#define CONV_DEDICATED_ENABLE          (  0 )
#endif

#if ( CONV_THERMAL_CURRENT_LIMIT != 0 )
#ifndef CONV_TCL_TEMPERATURE_LEVEL
#define CONV_TCL_TEMPERATURE_LEVEL     (  65 )
#endif
#ifndef CONV_TCL_COEFF
#define CONV_TCL_COEFF                 (   5 ) // Amps per degree
#endif
#endif

#define VOLTAGE_PWM_MAX                ( VOLTAGE_ABS_MAX * VOLTAGE_PWM_EXTRA_RES - 1 )
#define CURRENT_PWM_MAX                ( CURRENT_ABS_MAX * CURRENT_PWM_EXTRA_RES - 1 )


#define CONV_SS_RATIO_1                ( 0.3 )
#define CONV_SS_RATIO_2                ( 1.0 - CONV_SS_RATIO_1 )
//#define SOFT_START_TIME_STEP_1         ( (uint32_t)((_MS(1000) / (VOLTAGE_PWM_MAX*SOFT_START_RATIO_1))+0.5) )
//#define SOFT_START_TIME_STEP_2         ( (uint32_t)((_MS(500) / (VOLTAGE_PWM_MAX*SOFT_START_RATIO_2))+0.5) )

#define CONV_SS_TIME_1                 (  750 )
#define CONV_SS_TIME_2                 (  750 )

//#define CONV_SS_STEP_1                 ( CONV_SS_TIME_1 / (VOLTAGE_PWM_MAX * CONV_SS_RATIO_1) + 0.5 )
//#define CONV_SS_STEP_2                 ( CONV_SS_TIME_2 / (VOLTAGE_PWM_MAX * CONV_SS_RATIO_2) + 0.5 )
#define CONV_SS_STEP_1                 ( (VOLTAGE_PWM_MAX * CONV_SS_RATIO_1) / CONV_SS_TIME_1 + 0.5 )
#define CONV_SS_STEP_2                 ( (VOLTAGE_PWM_MAX * CONV_SS_RATIO_2) / CONV_SS_TIME_2 + 0.5 )

#if ( VOLTAGE_PWM_MAX/4/CONV_SS_TIME_1*10 < 5 )
#error <config-internal.h> - VOLTAGE_PWM_MAX - impossible to withstand the step of a soft start, too small step
#endif

//#if ( CONV_SS_STEP_1 < 100 )
//#error "<100"
//#endif

#define CURRENT_TRACKING_VSHIFT_MAX    ( 100 )
#define CURRENT_TRACKING_HYSTERESIS    ( CURRENT_ABS_MAX / 100 )
#define CONV_TRACKING_PERIOD           _MS(200)

#define CURRENT_LIMIT_TEMPERATURE      (  +69  )


#endif /* CONFIG_INTERNAL_H_ */
