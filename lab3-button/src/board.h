
#ifndef BOARD_H_
#define BOARD_H_

//
// Alarm relay
//
#define RELAY_ALARM_PIN             B, 7
#if ( RELAY_ALARM == 1 )
#define RELAY_ALARM_ON              PIN_CLR(RELAY_ALARM_PIN)
#define RELAY_ALARM_OFF             PIN_SET(RELAY_ALARM_PIN)
#else
#define RELAY_ALARM_ON
#define RELAY_ALARM_OFF
#endif

//
//
//
#define INDICATOR_IRQ               A, 13

//
// Digital signal input
//
#define DP_MB_ADDR_BIT_0            A, 8
#define DP_MB_ADDR_BIT_1            A, 9
#define DP_MB_ADDR_BIT_2            A, 5
#define DP_MB_ADDR_BIT_3            A, 4


#if ( DEV_DCTRL == 1 )
#define DSIG_PIN_1                  B, 4
//#define DSIG_PIN_2                  D, 7
#define DSIG_PIN_3                  B, 3
#define DSIG_PIN_F                  B, 6
#endif

//
// RS-485 config section
//
#define RS485_DIR_PIN               A, 3
#define RS485_1_TRANSMITTER_ENABLE  PIN_SET(RS485_DIR_PIN)
#define RS485_1_RECEIVER_ENABLE     PIN_CLR(RS485_DIR_PIN)


#define UARTSW_TX_PIN               B, 4
#define UARTSW_RX_PIN               B, 3

//
// Indicator config section
//
#define EXT_LEDS                    (  0  )
#define INT_CHARGER_LEDS            (  0  )
#define EXT_CHARGER_LEDS            (  0  )

#define ADC_AUX_SUPPLY_CHANNEL      (  0  )
#define ADC_OUTPUT_VOLTAGE_CHANNEL  (  1  )
#define ADC_OUTPUT_CURRENT_CHANNEL  (  2  )
#define ADC_TEMPERATURE_CHANNEL_1   (  8  )
#define ADC_TEMPERATURE_CHANNEL_2   (  9  )

// external led configuration
#define EXT_LED_PIN                 A, 13

#if ( DEV_TYPE == DEV_TYPE_CHARGER ) || ( DEV_TYPE == DEV_TYPE_CHARGER_FH )
#define LED_CHARGE_IN_PROGRESS      IND_LED_2
#define LED_CHARGE_DONE             IND_LED_4
#define LED_CHARGE_MASK             (LED_CHARGE_IN_PROGRESS | LED_CHARGE_DONE)
#endif

// Debug pins
#define DEBUG_X4_PIN                B, 1



#endif /* BOARD_H_ */


