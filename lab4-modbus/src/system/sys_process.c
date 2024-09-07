/**
 *  Note: Crit section fixed
 */
#include "sys_process.h"


__attribute__((naked)) uint32_t  sys_irq_disable( void )
{
    __asm volatile
    (
        "mrs    r0, PRIMASK   \n\t"
        "cpsid  i             \n\t"
        "bx     lr            \n\t"
        :
        :
        : "r0"
    );
}
__attribute__((naked)) uint32_t  sys_irq_restore( uint32_t mask )
{
    __asm volatile
    (
        "msr    PRIMASK, r0   \n\t"
        "bx     lr            \n\t"
        :
        :
        : "r0"
    );
    return 0;
}
