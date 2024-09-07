
#include <system.h>

#include "sys_service.h"



typedef struct
{
	void (*pfunc) (void);
} service_t;


static service_t  service[SERVICE_COUNT_MAX];
static int8_t  service_count;



void sys_service_add( void (*func)(void) )
{
    service[service_count].pfunc = func;

    service_count += 1;
}


void sys_service_run( void )
{
    for( int32_t i = 0; i < service_count; i++ )
    {
        service[i].pfunc();
    }
}

