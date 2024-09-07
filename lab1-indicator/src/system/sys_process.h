
#ifndef SYS_PROCESS_H_
#define SYS_PROCESS_H_


#include <system.h>

uint32_t  sys_irq_disable( void );
uint32_t  sys_irq_restore( uint32_t mask );


#define sys_barrier()              for( uint32_t mask = sys_irq_disable(), f=1;\
                                        f; f = sys_irq_restore(mask) )

#define task_init()                static uint8_t _task_ptr;                  \
                                   void ( *task_func[] )( void )
#define task_dispatch()            do {                                       \
                                   task_func[_task_ptr]();                    \
                                   if( ++_task_ptr >=                         \
                                   (sizeof(task_func)/sizeof(task_func[0])) ) \
                                   _task_ptr = 0;                             \
                                   } while(0)
#define task_sched(ptr)            do {                                       \
                                   (void) _task_ptr;                          \
                                   task_func[ptr]();                          \
                                   } while(0)
#define task_wait(t, period)       do {                                       \
                                   if( sys_timer_tick_diff_get(t) < period )  \
                                       return;                                \
                                   sys_timer_tick_update(t);                  \
                                   } while(0)

enum { Disable = 0, Enable };

enum MutexState{ Unlock, Lock };
enum TaskState { Init, Process, Shutdown };

#endif /* SYS_PROCESS_H_ */


