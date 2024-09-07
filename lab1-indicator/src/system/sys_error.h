//*****************************************************************************
//
// File Name    : 'sys_error.c'
// Title        :
// Author       : I.Otrokha
// Created      : 02.04.2011
// Revised      :
// Version      :
// Target MCU   :
// Editor Tabs  :
//
// Description  : provide exception mechanism.
//
//
// License      : Impuls ltd.
//
//
//*****************************************************************************

#ifndef SYS_ERROR_H_
#define SYS_ERROR_H_

#define ERR_ILLEGAL_COMMNAD           129
#define ERR_ACCESS_DENIED             128

#define ERR_MATH_CRC                  41
#define ERR_MATH_GENERIC              40

#define ERR_PROCESS_LOCKED            33
#define ERR_PROCESS_PENDING           32

#define ERR_EEPROM_WRONG_ADDR         28
#define ERR_EEPROM_BUSY               27
#define ERR_EEPROM_RW_COLLISION       26
#define ERR_EEPROM_QUEUE_FULL         25
#define ERR_EEPROM                    24

#define ERR_TEMP_SENSOR_3_SHORTEN     22
#define ERR_TEMP_SENSOR_2_SHORTEN     21
#define ERR_TEMP_SENSOR_1_SHORTEN     20
#define ERR_TEMP_SENSOR_SHORTEN       20
#define ERR_TEMP_SENSOR_3_BROKEN      19
#define ERR_TEMP_SENSOR_2_BROKEN      18
#define ERR_TEMP_SENSOR_1_BROKEN      17
#define ERR_TEMP_SENSOR_BROKEN        17
#define ERR_TEMP_SENSOR_ERR           16

#define ERR_AUX_VOLTAGE_TOO_HIGH       11
#define ERR_AUX_VOLTAGE_TOO_LOW        10
#define ERR_AUX_VOLTAGE_OUT_OF_RANGE   9
#define ERR_AUX_SUPPLY                 8

#define ERR_CRITICAL                   8    /* Non clearable errors starts here */

#define ERR_CALIBRATION_OK             4
#define ERR_CALIBRATION_PARAM          3
#define ERR_CALIBRATION_PROCEDURE      2
#define ERR_CALIBRATION_OUT_OF_RANGE   1
#define ERR_OK                         0


typedef unsigned char err_t;

err_t sys_error_get( void );
void  sys_error_set( err_t error );

void  sys_error_clear_warning( void );
void  sys_error_clear( void );

#endif /* SYS_ERROR_H_ */
