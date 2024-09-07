
#ifndef IMATH_H_
#define IMATH_H_


int32_t  abs32( int32_t x );
int16_t  abs16( int16_t x );
int32_t  clamp( int32_t *value, int32_t min, int32_t  max );
int32_t  clamp16( int16_t *value, int16_t min, int16_t  max );

int32_t ipow( int32_t x, int32_t step );
uint32_t  isqrt( uint32_t a_nInput );

int32_t  coeff_calc( int32_t value, int32_t coeff, int8_t coeff_resolution );


#endif /* IMATH_H_ */

