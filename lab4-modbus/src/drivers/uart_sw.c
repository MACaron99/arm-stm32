
#include <system.h>

#include "uart_sw.h"


//
// Half-duplex operation
//

#define BAUDRATE                   19200
#define BAUDRATE_TIME_CONST        ( (uint32_t)((48000000 / BAUDRATE) - 1) )

#define PARITY_BIT                    1
#define DATA_BITS_COUNT             ( 8 )
#define PAYLOAD_BITS_COUNT          ( DATA_BITS_COUNT + PARITY_BIT )
#define FRAME_BITS_COUNT            ( 1 + PAYLOAD_BITS_COUNT + 1 )



// uartsw transmit status and data variables
static volatile bool  uartsw_tx_busy;
static volatile uint16_t  uartsw_tx_data;

// uartsw receive status and data variables
static volatile uint8_t  uartsw_rx_is_new_data;
static volatile uint16_t  uartsw_rx_data;
#if ( PARITY_BIT == 1 )
static volatile int8_t  uartsw_rx_parity;
#endif



int8_t uartsw_is_tx_done( void )
{
    return  uartsw_tx_busy ? 0 : 1;
}


int8_t uartsw_char_put( uint8_t data )
{
    uint16_t  xmit_word = data;

    // wait until uart is ready
    if( uartsw_tx_busy )
        return false;
    uartsw_tx_busy = true; // set busy flag

    // save data
    uartsw_tx_data = xmit_word;
    TIM17->CR1 |= TIM_CR1_CEN;

    return true;
}


int8_t uartsw_char_get( uint8_t *data )
{
    uint16_t temp;


    if( uartsw_rx_is_new_data )
    {
        uartsw_rx_is_new_data = 0;

        temp = uartsw_rx_data;
        *data = temp;

#if ( PARITY_BIT == 1 )
        // check parity
        if( !uartsw_rx_parity )
        {
            return false;
        }
#endif
        return true;
    }

    return false;
}


void  EXTI2_3_IRQHandler()
{
    // shedule t=0,5 of bit time
    TIM16->ARR = (BAUDRATE_TIME_CONST / 2);
    TIM16->CR1 |= TIM_CR1_CEN;

    // disable EXTI
    EXTI->PR |= EXTI_PR_PR3;
    EXTI->IMR &= ~EXTI_IMR_MR3;
}


//
// TIM16 - RX
//
void TIM16_IRQHandler()
{
    static int8_t bit_count;
    static uint16_t buffer;
#if ( PARITY_BIT != 0 )
    static int8_t parity_calc;
    int8_t parity_recv;
#endif

    TIM16->SR = 0;
    TIM16->ARR = BAUDRATE_TIME_CONST; // schedule the next bit

    // start bit has already been received
    // we're in the data bits
    if( bit_count == 0 )
    {
        // start bit
        bit_count ++;
    }
    else if( bit_count < (DATA_BITS_COUNT + 1)  )
    {
        // sample the data line
        if( PIN_STATE(UARTSW_RX_PIN) )
        {
            // serial line is marking
            // record '1' bit
            buffer |= (1 << (bit_count - 1));
            parity_calc ^= 1;
        }

        bit_count ++;
    }
#if ( PARITY_BIT != 0 )
    else if( bit_count < (PAYLOAD_BITS_COUNT + 1)  )
    {
        parity_recv = PIN_STATE(UARTSW_RX_PIN) ? 1 : 0;
        uartsw_rx_parity = (parity_calc == parity_recv);
        parity_calc = 0;

        bit_count ++;
    }
#endif
    else
    {
        // stop bit
        TIM16->CR1 &= ~TIM_CR1_CEN;
        EXTI->IMR |= EXTI_IMR_MR3;

        uartsw_rx_is_new_data = 1;
        bit_count = 0;

        uartsw_rx_data = buffer;
        buffer = 0;
    }
}


//
// TIM17 - TX
//
void TIM17_IRQHandler()
{
    static int8_t bit_count;
#if ( PARITY_BIT != 0 )
    static int8_t parity_calc;
#endif


    TIM17->SR = 0;

    if( bit_count == 0 )
    {
        // start bit
        PIN_CLR(UARTSW_TX_PIN);
        parity_calc = 0;

        bit_count++;
    }
    else if( bit_count < (DATA_BITS_COUNT + 1) )
    {
        // data bits (LSB first)
        if( uartsw_tx_data & 0x01 )
        {
            PIN_SET(UARTSW_TX_PIN);
            parity_calc ^= 1;
        }
        else
        {
            PIN_CLR(UARTSW_TX_PIN);
        }

        uartsw_tx_data = uartsw_tx_data >> 1;
        bit_count++;
    }
#if ( PARITY_BIT != 0 )
    else if( bit_count < (PAYLOAD_BITS_COUNT + 1)  )
    {
        if( parity_calc )
        {
            PIN_SET(UARTSW_TX_PIN);
        }
        else
        {
            PIN_CLR(UARTSW_TX_PIN);
        }

        parity_calc = 0;

        bit_count ++;
    }
#endif
    else if( bit_count < (PAYLOAD_BITS_COUNT + 2) )
    {
        // stop bit
        PIN_SET(UARTSW_TX_PIN);
        bit_count++;
    }
    else
    {
        // transmission is done
        bit_count = 0;
        uartsw_tx_busy = false;

        TIM17->CR1 &= ~TIM_CR1_CEN;
    }
}


void uartsw_init( void )
{
    GPIO_InitTypeDef  gpio;

    //
    // GPIO configuration
    //
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;


    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOB, &gpio);
    PIN_SET(UARTSW_TX_PIN);

    gpio.GPIO_Mode = GPIO_Mode_IN;;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &gpio);

    //
    // Timer Base configuration
    //

    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

    //
    // Timer 16 - RX
    //
    TIM16->ARR =  BAUDRATE_TIME_CONST;
    TIM16->EGR |= TIM_EGR_UG;
    TIM16->DIER |= TIM_DIER_UIE; // enable timer interrupt
    NVIC_SetPriority(TIM16_IRQn, 0);
    NVIC_EnableIRQ(TIM16_IRQn);
    //TIM16->CR1 |= TIM_CR1_CEN;

    //
    // Timer 17 - TX
    //
    TIM17->ARR =  BAUDRATE_TIME_CONST;
    TIM17->EGR |= TIM_EGR_UG;
    TIM17->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM17_IRQn, 0);
    NVIC_EnableIRQ(TIM17_IRQn);

    // EXTI configuration

    /* (1) Enable the peripheral clock of GPIOB */
    /* (2) Select Port A for pin 0 external interrupt by writing 0000 in
     EXTI0 (reset value)*/
    /* (3) Configure the corresponding mask bit in the EXTI_IMR register */
    /* (4) Configure the Trigger Selection bits of the Interrupt line on
     rising edge*/
    /* (5) Configure the Trigger Selection bits of the Interrupt line on
     falling edge*/

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; /* (1) */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    // SYSCFG->EXTICR[1] &=  ~(0xFFFF);  // сброс битов, соответствующих EXTI7
    SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI3_PB; // установка битов, соответствующих порту B
    //EXTI->RTSR = 0x80; /* (5) */
    EXTI->FTSR = EXTI_FTSR_TR3; /* (5) */
    /* Configure NVIC for External Interrupt */
    /* (1) Enable Interrupt on EXTI0_1 */
    /* (2) Set priority for EXTI0_1 */
    NVIC_SetPriority(EXTI2_3_IRQn, 0); /* (2) */
    NVIC_EnableIRQ(EXTI2_3_IRQn); /* (1) */
    EXTI->IMR = EXTI_IMR_MR3; /* (3) */

    uartsw_tx_busy = false;
}


