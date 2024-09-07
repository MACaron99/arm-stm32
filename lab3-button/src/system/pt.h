
#ifndef _PT_H_
#define _PT_H_

#include <system/sys_timer.h>

//*****************************************************************************
//
// Make label
//
//*****************************************************************************
#define CONCAT(s1, s2) s1##s2
#define LABEL(s) CONCAT(L, s)

//*****************************************************************************
//
// Protothread start
//
//*****************************************************************************
#define PT_ENTER()                  \
static void *_stage;                \
char _need_yield = 0;               \
(void) _need_yield;                 \
do                                  \
{                                   \
    if(_stage)                      \
    {                               \
        goto *_stage;               \
    }                               \
}                                   \
while(0)

//*****************************************************************************
//
// Protothread exit
//
//*****************************************************************************
#define PT_EXIT()                   \
do                                  \
{                                   \
    _stage = 0;                     \
    return PT_STATUS_DONE;          \
}                                   \
while(0)

//*****************************************************************************
//
// Protothread wait for condition
//
//*****************************************************************************
#define PT_WAIT_UNTIL(cond)         \
 do                                 \
 {                                  \
    _stage = &&LABEL(__LINE__);     \
LABEL(__LINE__):                    \
    if(!(cond))                     \
    {                               \
        return PT_STATUS_YIELD;     \
    }                               \
}                                   \
while(0)

//*****************************************************************************
//
// Protothread yield
//
//*****************************************************************************
#define PT_YIELD()                  \
 do                                 \
 {                                  \
     _need_yield = 1;               \
     _stage = &&LABEL(__LINE__);    \
LABEL(__LINE__):                    \
    if(_need_yield)                 \
    {                               \
        return PT_STATUS_YIELD;     \
    }                               \
}                                   \
while(0)

//*****************************************************************************
//
// Protothread return status
//
//*****************************************************************************
typedef enum
{
    PT_STATUS_YIELD = 0,
    PT_STATUS_DONE  = 1
} pt_status_t;

//*****************************************************************************
//
// Protothread stage
//
//*****************************************************************************
typedef enum
{
    PT_STAGE_READY   = 0,
    PT_STAGE_RUNNING = 1,
    PT_STAGE_DONE    = 2
} pt_stage_t;

//*****************************************************************************
//
// Protothread control structure
//
//*****************************************************************************
typedef struct _pt
{
    struct _pt *next;
    pt_status_t (*function)();
    pt_stage_t stage;

} pt_t;

//*****************************************************************************
//
// Protothread mutex
//
//*****************************************************************************
typedef struct
{
    pt_t *owner;
} pt_mutex_t;

#define pt_task_delay(p_timer, time)   do { \
                                       *(p_timer) = sys_timer_tick_get(); \
                                       PT_WAIT_UNTIL(sys_timer_tick_diff_get(*(p_timer)) >= time); \
                                       } while(0)

#define pt_task_sleep(timer, time)         do { \
                                              sys_timer_tick_update(timer); \
                                              PT_WAIT_UNTIL(sys_timer_tick_diff_get(timer) >= time); \
                                              } while(0)


int8_t  pt_mutex_acquire( pt_mutex_t *mutex );
void  pt_mutex_release( pt_mutex_t *mutex );
void  pt_schedule( void );
void  pt_create( pt_t *pt, pt_status_t (*func)() );


#endif /* _PT_H_ */


