
#ifndef SYSTEM_H_
#define SYSTEM_H_


#include <config.h>
#include <board.h>

#include <lib/types.h>
#include <lib/ustdlib.h>
#include <drivers/core.h>
#include <system/pt.h>
#include <system/sys_assert.h>
#include <system/sys_process.h>
#include <system/sys_error.h>
#include <system/sys_timer.h>
#include <system/sys_param.h>
#include <system/sys_service.h>


enum
{
    SYSTEM_STATUS_UNKNOWN,
    SYSTEM_STATUS_MALFUNCTION,
    SYSTEM_STATUS_OUTPUT_FAULT,
    SYSTEM_STATUS_INPUT_FAULT,
    SYSTEM_STATUS_UNDERCOOL,
    SYSTEM_STATUS_OVERHEAT,
    SYSTEM_STATUS_OFFLINE,
    SYSTEM_STATUS_ONLINE
};


int32_t sys_run_state_get( void );

int32_t sys_status_get( void );
int32_t sys_alarm_state_get( void );

int32_t  sys_service_mode_get( void );
void  sys_service_mode_set( int32_t state );



#endif /* SYSTEM_H_ */
