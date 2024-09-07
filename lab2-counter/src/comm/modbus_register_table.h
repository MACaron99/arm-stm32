
#ifndef MODBUS_REGISTER_TABLE_H_
#define MODBUS_REGISTER_TABLE_H_


enum Parameters
{
// status registers
    INPUT_VOLTAGE,              // 1000
    INPUT_CURRENT,              // 1001
    OUTPUT_VOLTAGE,             // 1002
    OUTPUT_CURRENT,             // 1003
    OUTPUT_POWER,               // 1004
    TEMPERATURE_REG,            // 1005
    CONV_VOLTAGE,               // 1006
    SYS_STATUS,                 // 1007

// control registers
    OUTPUT_VOLTAGE_SETUP,       // 1008
    OUTPUT_CURRENT_LIMIT,       // 1009
#if ( DEV_TYPE == DEV_TYPE_DCTRL_PSU )
    OUTPUT_VOLTAGE_SETUP_1,     // 1010
    OUTPUT_VOLTAGE_SETUP_2,     // 1011
    OUTPUT_VOLTAGE_SETUP_3,     // 1012
    OUTPUT_VOLTAGE_SETUP_F,     // 1013
#elif ( DEV_TYPE == DEV_TYPE_CHARGER_FH )
    OUTPUT_VOLTAGE_SETUP_FAST,  // 1010
    OUTPUT_VOLTAGE_SETUP_HOLD , // 1011
    PARAM_1010,                 // 1012
    PARAM_1011,                 // 1013
#else
    OUTPUT_VOLTAGE_SETUP_CTRL,  // 1010
    OUTPUT_CURRENT_LIMIT_CTRL,  // 1011
    CONV_ENABLE_CTRL,           // 1012
    PARAM_1013,                 // 1013
#endif
    REMOTE_CTRL_CMD,            // 1014
    REMOTE_CTRL_ARG,            // 1015

    CALIBRATION_COEFF_1,        // 1016  (VOLTAGE)
    CALIBRATION_COEFF_2,        // 1017  (CURRENT)
    CALIBRATION_COEFF_3,        // 1018  (CURRENT PWM CURVE)
    CALIBRATION_COEFF_4,        // 1019

    TCOMP_MAX_BOUNDARY,         // 1020
    TCOMP_VALUE,                // 1021

    CURRENT_TRACKING_ENABLE,    // 1022
    CURRENT_TRACKING_MASTER_ID, // 1023
    CURRENT_TRACKING_VALUE,     // 1024
    CURRENT_TRACKING_PID_KP,    // 1025
    CURRENT_TRACKING_PID_KI,    // 1026

    MODBUS_ADDRESS,             // 1027
    MODBUS_BAUDRATE,            // 1028
    MODBUS_PARITY,              // 1029

    DEBUG_1,                    // 1030
    DEBUG_2,                    // 1031
    DEBUG_3,                    // 1032

    PARAM_COUNT
};

enum ParametersEx
{
    MULTICAST_START_ADDR,       // 2000
    MULTICAST_END_ADDR,         // 2001

    MULTICAST_REMOTE_CTRL_CMD,  // 2002
    MULTICAST_REMOTE_CTRL_ARG1, // 2003
    MULTICAST_REMOTE_CTRL_ARG2, // 2004
    MULTICAST_REMOTE_CTRL_ARG3, // 2005
    MULTICAST_REMOTE_CTRL_ARG4, // 2006
    MULTICAST_REMOTE_CTRL_ARG5, // 2007
    MULTICAST_REMOTE_CTRL_ARG6, // 2008
    MULTICAST_REMOTE_CTRL_ARG7, // 2009
    MULTICAST_REMOTE_CTRL_ARG8, // 2010
    MULTICAST_REMOTE_CTRL_ARG9, // 2011

    PARAM_EXTENDED_COUNT
};

#endif /* MODBUS_REGISTER_TABLE_H_ */
