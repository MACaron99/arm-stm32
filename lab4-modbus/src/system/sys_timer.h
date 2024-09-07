
#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_

// only for sys_ticks timer counter
#define _MS( x_100usValue )                   ( (x_100usValue)*10U )

// only for seconds timer counter
#define _SEC( x_1secValue )                   ( x_1secValue )
#define _MN(  x_1secValue )                   ( (x_1secValue)*60U )
#define _HR(  x_1secValue )                   ( (x_1secValue)*3600U )

#define sys_timer_seconds_diff_get( timer )   ( sys_timer_seconds_get() - timer )
#define sys_timer_seconds_update( timer )     timer = sys_timer_seconds_get()

#define sys_timer_tick_diff_get( timer )      ( sys_timer_tick_get() - timer )
#define sys_timer_tick_update( timer )        timer = sys_timer_tick_get()

#define sys_timer_ms_diff_get( timer )        ( sys_timer_ms_get() - timer )

#define sys_timer_sleep(timer, time)          do { \
                                              sys_timer_tick_update(timer); \
                                              PT_WAIT_UNTIL(sys_timer_tick_diff_get(timer) >= time); \
                                              } while(0)


typedef uint32_t  timer_sec_t;
typedef uint32_t  timer_t;


void  sys_timer_timeproc_add( void (*func)(void), int8_t period );

timer_sec_t  sys_timer_seconds_get( void );
timer_t  sys_timer_ms_get( void );
timer_t  sys_timer_tick_get( void );

void  sys_timer_init( void );


#endif /* SYS_TIMER_H_ */
