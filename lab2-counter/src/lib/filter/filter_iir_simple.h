
#ifndef FILTER_IIR_H_
#define FILTER_IIR_H_


typedef struct
{
    int32_t  acc;
    int16_t  prev;
    int16_t  k_const;
} iir32_t;


int32_t  iir32_calc( iir32_t *filter, int16_t value );
void  iir32_init( iir32_t *filter, int16_t k_const );


#endif /* FILTER_IIR_H_ */
