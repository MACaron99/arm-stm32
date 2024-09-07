
#include <stdint.h>

#include "imath.h"



int32_t abs32( int32_t x )
{
    if( x < 0 )
    {
        x = -x;
    }

    return x;
}


int16_t abs16( int16_t x )
{
    if( x < 0 )
    {
        x = -x;
    }

    return x;
}


int32_t clamp( int32_t *value, int32_t min, int32_t  max )
{
    int32_t  is_clamped = 0;

    if( *value < min )
    {
        *value = min;
        is_clamped = 1;
    }
    else
    if( *value > max )
    {
        *value = max;
        is_clamped = 1;
    }

    return is_clamped;
}


int32_t clamp16( int16_t *value, int16_t min, int16_t  max )
{
    int32_t  is_clamped = 0;

    if( *value < min )
    {
        *value = min;
        is_clamped = 1;
    }
    else
    if( *value > max )
    {
        *value = max;
        is_clamped = 1;
    }

    return is_clamped;
}


int32_t ipow( int32_t x, int32_t step )
{
    int32_t y;

    if( step < 0 )
        return 0;
    if( step == 0 )
        return 1;

    y = x;
    for( int32_t i = 0; i < step; i++ )
    {
        y = y * x;
    }

    return y;
}


uint32_t isqrt( uint32_t a_nInput )
{
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }

    /* Do arithmetic rounding to nearest integer */
    if (op > res)
    {
        res++;
    }

    return res;
}


int32_t coeff_calc( int32_t value, int32_t coeff, int8_t coeff_resolution )
{
    int32_t  result;

    result = value * coeff;
    result = result + (1 << (coeff_resolution - 1)); // rounding
    result = result >> coeff_resolution;

    return result;
}


