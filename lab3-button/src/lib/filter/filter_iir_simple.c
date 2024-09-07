
#include <stdint.h>

#include "filter_iir_simple.h"



int32_t iir32_calc( iir32_t *filter, int16_t value )
{
    int32_t  output;

    filter->acc += value - filter->prev;
    output = filter->acc / filter->k_const;
    filter->prev = output;

    return output;
}


void iir32_init( iir32_t *filter, int16_t k_const )
{
    filter->acc = 0;
    filter->prev = 0;
    filter->k_const = k_const;
}
