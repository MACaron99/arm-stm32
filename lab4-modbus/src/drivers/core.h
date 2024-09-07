
#ifndef CORE_H_
#define CORE_H_


#include <stm32f0xx.h>
#include <stm32f0xx_conf.h>


#define PIN_SET(ARGS)                  __PIN_SET(ARGS)
#define PIN_CLR(ARGS)                  __PIN_CLR(ARGS)
#define PIN_TOGGLE(ARGS)               __PIN_TOGGLE(ARGS)
#define PIN_STATE(ARGS)                __PIN_STATE(ARGS)
#define PIN_OUTPUT(ARGS)               __PIN_OUTPUT(ARGS)
#define PIN_INPUT(ARGS)                __PIN_INPUT(ARGS)

#define __PIN_SET(PORT_LETTER, N)      GPIO ## PORT_LETTER->BSRR = GPIO_Pin_ ## N
#define __PIN_CLR(PORT_LETTER, N)      GPIO ## PORT_LETTER->BRR = GPIO_Pin_ ## N
#define __PIN_TOGGLE(PORT_LETTER, N)   GPIO ## PORT_LETTER->ODR ^= GPIO_Pin_ ## N
#define __PIN_STATE(PORT_LETTER, N)    ( GPIO ## PORT_LETTER->IDR & GPIO_Pin_ ## N )
#define __PIN_OUTPUT(PORT_LETTER, N)   GPIO ## PORT_LETTER->MODER &= ~(GPIO_MODER_MODER0 << (N * 2)); \
                                       GPIO ## PORT_LETTER->MODER |=  (0x01UL << (N * 2))
#define __PIN_INPUT(PORT_LETTER, N)    GPIO ## PORT_LETTER->MODER &= ~(GPIO_MODER_MODER0 << (N * 2)); \
                                       GPIO ## PORT_LETTER->MODER |=  (0x00UL << (N * 2))

#define PIN_GPIO_GET(ARGS)             _PIN_GPIO_GET(ARGS)
#define _PIN_GPIO_GET(LETTER, N)       GPIO ## LETTER
#define PIN_NUMBER_GET(ARGS)           _PIN_NUMBER_GET(ARGS)
#define _PIN_NUMBER_GET(LETTER, N)     N


extern uint32_t  sys_core_clock;


void  core_powerdown( void );
void  core_iwdt_reset( void );
void  core_iwdt_init( void );
void  core_init( void );


#endif /* CORE_H_ */
