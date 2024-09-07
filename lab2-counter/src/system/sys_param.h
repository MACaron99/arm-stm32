
#ifndef SYS_PARAM_H_
#define SYS_PARAM_H_

#include <modbus_register_table.h>


uint16_t sys_param_set_via_protocol( uint8_t param, uint16_t value );


void sys_param_setup_init( void );
void sys_param_setup_callback( void );
void sys_param_save( uint16_t param );
void sys_param_save_standart( void );
void sys_param_default_set( void );
uint16_t sys_param_set( uint16_t param, int16_t value );
uint16_t sys_param_step_get( uint16_t param );
uint16_t sys_param_adjust( uint16_t param, int16_t ajust_value );
uint16_t sys_param_get( uint16_t param );
void sys_param_inc( uint16_t param );

void  sys_param_init( void );


#endif /* SYS_PARAM_H_ */


