
#ifndef FILTER1_H_ // Include guards
#define FILTER1_H_


typedef struct
{
    int16_t  state[8];
    int16_t  output;
} filter1Type;

typedef struct
{
    int16_t  *pInput;
    int16_t  *pOutput;
    int16_t  *pState;
    int16_t  *pCoefficients;
    int16_t  count;
} filter1_executionState;



int filter1_filterBlock( filter1Type * pThis, int16_t * pInput, int16_t * pOutput, int32_t count );

void filter1_filterBiquad_14_14( filter1_executionState * pExecState );


#endif // FILTER1_H_

