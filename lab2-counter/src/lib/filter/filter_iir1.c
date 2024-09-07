
#include <stdint.h>

#include "filter_iir1.h"



static int16_t  filter1_coefficients[10] =
{
// Scaled for a 16x16:32 Direct form 1 IIR filter with:
// Feedback shift = 14
// Output shift = 14
// Input  has a maximum value of 1, scaled by 2^15
// Output has a maximum value of 1.2493048565317384, scaled by 2^14
    632, 1265, 632, 17180, -4852,// b0 Q13(0.0772), b1 Q13(0.154), b2 Q13(0.0772), a1 Q14(1.05), a2 Q14(-0.296)
    1024, 2048, 1024, 21642, -10367// b0 Q14(0.0625), b1 Q14(0.125), b2 Q14(0.0625), a1 Q14(1.32), a2 Q14(-0.633)
};


int filter1_filterBlock( filter1Type * pThis, int16_t * pInput, int16_t * pOutput, int32_t count )
{
    filter1_executionState executionState;          // The executionState structure holds call data, minimizing stack reads and writes
    if( ! count ) return 0;                         // If there are no input samples, return immediately
    executionState.pInput = pInput;                 // Pointers to the input and output buffers that each call to filterBiquad() will use
    executionState.pOutput = pOutput;               // - pInput and pOutput can be equal, allowing reuse of the same memory.
    executionState.count = count;                   // The number of samples to be processed
    executionState.pState = pThis->state;                   // Pointer to the biquad's internal state and coefficients.
    executionState.pCoefficients = filter1_coefficients;    // Each call to filterBiquad() will advance pState and pCoefficients to the next biquad

    // The 1st call to filter1_filterBiquad() reads from the caller supplied input buffer and writes to the output buffer.
    // The remaining calls to filterBiquad() recycle the same output buffer, so that multiple intermediate buffers are not required.

    filter1_filterBiquad_14_14( &executionState );  // Run biquad #0
    executionState.pInput = executionState.pOutput; // The remaining biquads will now re-use the same output buffer.

    filter1_filterBiquad_14_14( &executionState );  // Run biquad #1

    // At this point, the caller-supplied output buffer will contain the filtered samples and the input buffer will contain the unmodified input samples.
    return count;    // Return the number of samples processed, the same as the number of input samples
}


void filter1_filterBiquad_14_14( filter1_executionState * pExecState )
{
    // Read state variables
    int16_t  x0;
    int16_t  x1 = pExecState->pState[0];
    int16_t  x2 = pExecState->pState[1];
    int16_t  y1 = pExecState->pState[2];
    int16_t  y2 = pExecState->pState[3];

    // Read coefficients into work registers
    int16_t  b0 = *(pExecState->pCoefficients++);
    int16_t  b1 = *(pExecState->pCoefficients++);
    int16_t  b2 = *(pExecState->pCoefficients++);
    int16_t  a1 = *(pExecState->pCoefficients++);
    int16_t  a2 = *(pExecState->pCoefficients++);

    // Read source and target pointers
    int16_t  *pInput  = pExecState->pInput;
    int16_t  *pOutput = pExecState->pOutput;
    int32_t  count = pExecState->count;
    int32_t  accumulator;

    while( count-- )
    {
        x0 = *(pInput++);

        accumulator  = (int32_t)x2 * b2;
        accumulator += (int32_t)x1 * b1;
        accumulator += (int32_t)x0 * b0;

        x2 = x1;        // Shuffle left history buffer
        x1 = x0;

        accumulator += (int32_t)y2 * a2;
        accumulator += (int32_t)y1 * a1;

        y2 = y1;        // Shuffle right history buffer
        y1 = accumulator  >> 14;

        *(pOutput++) = accumulator  >> 14;
    }

    *(pExecState->pState++) = x1;
    *(pExecState->pState++) = x2;
    *(pExecState->pState++) = y1;
    *(pExecState->pState++) = y2;
}

