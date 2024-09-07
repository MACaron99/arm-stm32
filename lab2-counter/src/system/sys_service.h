

#ifndef SYS_SERVICE_H_
#define SYS_SERVICE_H_


#ifndef SERVICE_COUNT_MAX
#define SERVICE_COUNT_MAX           (  32  )
#endif


void sys_service_add( void (*func)(void) );
void sys_service_run( void );


#endif /* SYS_SERVICE_H_ */
