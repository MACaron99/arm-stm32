
#ifndef CONFIG_H_
#define CONFIG_H_


#include <config/dev_types.h>

//
// Board configuration
//
//#define BOARD_TYPE                      713144
#define BOARD_TYPE                      713168

#define INDICATOR_TYPE                  713161
#define INDICATOR_REV

//
// Device configuration
//
#define DEV_TYPE                        DEV_TYPE_GENERIC_PSU

#define RELAY_ALARM                     (   1  )

#define CONV_DEDICATED_ENABLE           (   0  )
#define CONV_THERMAL_CURRENT_LIMIT      (   0  )

//#define LED_ONLINE                      IND_LED_3
#define MENU_TYPE                       MT_SYSTEM
//#define SYS_UI_INC_MESSAGE_OFF

//
// Power converter configuration
//
#define VOLTAGE_INC_EXTRA_RES           (   1 )
#define VOLTAGE_PWM_EXTRA_RES           (  10 )  // VOLTAGE_MAX * VOLTAGE_PWM_EXTRA_RES
#define CURRENT_PWM_EXTRA_RES           (  20 )

#define VOLTAGE_SET_INC                 (   1 * 1 )
#define VOLTAGE_SET_MIN                 ( 100 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_SET_MAX                 ( 420 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_SET_DEF                 ( 420 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_SET_MIN_CTRL            VOLTAGE_SET_MIN
#define VOLTAGE_SET_MAX_CTRL            VOLTAGE_SET_MAX
//#define VOLTAGE_MIN                     VOLTAGE_SET_MIN
#define VOLTAGE_ABS_MAX                 ( 430 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_MEASUREMENT_FORMAT      Fmt_F1
#define VOLTAGE_SETUP_FORMAT            Fmt_F1

#define CURRENT_SET_INC                 (    5 )
#define CURRENT_SET_MIN                 (   50 )
#define CURRENT_SET_MAX                 (  300 )
#define CURRENT_SET_MIN_CTRL            ( CURRENT_SET_MIN )
#define CURRENT_SET_MAX_CTRL            ( CURRENT_SET_MAX )
#define CURRENT_SET_DEF                 CURRENT_SET_MIN
#define CURRENT_ABS_MIN                 ( CURRENT_SET_MIN / 10 + 1 )
#define CURRENT_ABS_MAX                 ( CURRENT_SET_MAX )
#define CURRENT_MEASUREMENT_FORMAT      Fmt_F1
#define CURRENT_SETUP_FORMAT            Fmt_F1

#define OUTPUT_POWER_COEFF              (  100 )

//
// Protection configuration
//
#define PROTECTION_VOLTAGE_MIN          (   50 )
#define PROTECTION_VOLTAGE_MAX          (  450 )
#define PROTECTION_CURRENT_MAX          (  320 )
#define PROTECTION_TIMEOUT              ( 1500 ) // ms

#define PROTECTION_OVERHEAT_ON          ( +75 )
#define PROTECTION_OVERHEAT_OFF         ( +60 )
#define PROTECTION_UNDERCOOL_ON         (  -5 )
#define PROTECTION_UNDERCOOL_OFF        (   0 )

//
// Calibration section
//
#define CALIBRATION_OUTPUT_VOLTAGE      ( VOLTAGE_SET_MAX / VOLTAGE_INC_EXTRA_RES )
#define CALIBRATION_OUTPUT_CURRENT      ( CURRENT_SET_MAX )

// Service mode section
#define SERVICE_VOLTAGE_MAX             VOLTAGE_SET_MAX
#define SERVICE_CURRENT_MAX             CURRENT_SET_MAX


#include <config/config-internal.h>
#include <config/config-templates.h>


#endif /* CONFIG_H_ */

