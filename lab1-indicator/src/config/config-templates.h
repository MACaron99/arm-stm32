
#ifndef SRC_CONFIG_TEMPLATES_H_
#define SRC_CONFIG_TEMPLATES_H_


//
// Charger configuration - template
//
#if ( DEV_TYPE == DEV_TYPE_CHARGER ) || ( DEV_TYPE == DEV_TYPE_CHARGER_FH )

#ifndef LED_PROTECTION
#define LED_PROTECTION                 IND_LED_3
#endif

//#define CHARGER_INC_CHARGE_TIMEOUT
//#define CHARGER_FIXED_CURRENT_THRESHOLD

#define VOLTAGE_SET_DEF_FAST           ( 290 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_SET_DEF_HOLD           ( 272 * VOLTAGE_INC_EXTRA_RES )

#ifndef CHARGER_FIXED_CURRENT_THRESHOLD
#define CHARGER_FAST_MODE_CHANGE_COEFF (  2  ) // Iout >= Ilim / x
#define CHARGER_HOLD_MODE_CHANGE_COEFF (  3  ) // Iout <  Ilim / x
#else
#define CHARGER_FAST_MODE_CHANGE_LEVEL (   6 ) // => 0,6A
#define CHARGER_HOLD_MODE_CHANGE_LEVEL (   3 ) // =< 0,3A
#endif

#define CHARGER_HOLD_TRANSITION_TIME   (  3 * 60 )
#define CHARGER_FAST_TRANSITION_TIME   (      30 )

#endif

//
// Discrete control configuration - template
//
#if 0

#define VOLTAGE_1_SET_DEF              ( 690 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_2_SET_DEF              ( 720 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_3_SET_DEF              ( 710 * VOLTAGE_INC_EXTRA_RES )
#define VOLTAGE_F_SET_DEF              ( 680 * VOLTAGE_INC_EXTRA_RES )

#endif


#endif /* SRC_CONFIG_TEMPLATES_H_ */
