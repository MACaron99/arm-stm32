
#include <system.h>

#include "sys_timer.h"

#ifndef TIMEPROC_COUNT_MAX
#define TIMEPROC_COUNT_MAX                    (  8  )
#endif

#define incSysTick( sysTick )              sysTick++
#define incLowTick( lowTick, cnt, presc )  do { if( ++cnt >= presc ) {cnt = 0; \
                                           lowTick++;}} while(0)

typedef struct timeproc_tag
{
    void (*pfunc) (void);

    int8_t period;
    int8_t timer;
} timeproc_t;


static volatile timer_t     sys_timer_100us_counter;
static volatile timer_t     sys_timer_ms_counter;
static volatile timer_sec_t sys_timer_sec_counter;
static          timeproc_t  timeproc[TIMEPROC_COUNT_MAX];
static volatile int8_t      timeproc_count;



timer_sec_t
sys_timer_seconds_get( void )
{
    timer_sec_t ticks;

    ticks = sys_timer_sec_counter;
    while( ticks != sys_timer_sec_counter )
    {
        ticks = sys_timer_sec_counter;
    }

    return ticks;
}


timer_t
sys_timer_ms_get( void )
{
    timer_t  ticks;

    ticks = sys_timer_ms_counter;

    return ticks;
}


timer_t
sys_timer_tick_get( void )
{
    timer_t  ticks;

    ticks = sys_timer_100us_counter;
    while( ticks != sys_timer_100us_counter )
    {
        ticks = sys_timer_100us_counter;
    }

    return ticks;
}


void
sys_timer_timeproc_add( void (*func)(void), int8_t period )
{
    sys_barrier( )
    {
        if( timeproc_count >= TIMEPROC_COUNT_MAX )
            while( 1 ) ;

        timeproc[timeproc_count].pfunc = func;
        timeproc[timeproc_count].period = period;
        timeproc[timeproc_count].timer = 0;

        timeproc_count ++;
    }
}


void SysTick_Handler( void )
{
    static uint16_t  presc_sec;
    static uint16_t  presc_ms;

    if( ++presc_ms >= 10 )
    {
        presc_ms = 0;
        sys_timer_ms_counter++;

        if( ++presc_sec >= 1000 )
        {
            presc_sec = 0;
            sys_timer_sec_counter++;
        }
    }

    incSysTick(sys_timer_100us_counter);


    for( int32_t  i = 0; i < timeproc_count; i++ )
    {
        timeproc[i].timer += 1;

        if( timeproc[i].timer >= timeproc[i].period )
        {
            timeproc[i].timer = 0;

            timeproc[i].pfunc();
        }
    }
}


void
sys_timer_init( void )
{
    //
    // Configure SysTick for 100us interrupts
    //
    SysTick_Config(sys_core_clock / 10000);
}


