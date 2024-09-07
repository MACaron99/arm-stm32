
#include <stdint.h>

#include "sys_assert.h"



void
sys_assert( char *file_name, uint16_t line_num )
{
    (void) file_name;
    (void) line_num;

    while(1) ;
}


