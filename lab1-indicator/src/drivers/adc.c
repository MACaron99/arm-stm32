
#include <system.h>

#include "adc.h"


//
// ADCclock = 14Mhz = 71,4nS
// Tconv = (SampleTime + 12,5) * ADCclock
//
// ADC_SampleTime_71_5Cycles  => ( 71,5 + 12,5) * clk =  6,0 uS
// ADC_SampleTime_239_5Cycles => (239,5 + 12,5) * clk = 18,0 uS
//
#define ADC_SAMPLE_TIME             ( ADC_SampleTime_71_5Cycles )


typedef struct
{
    GPIO_TypeDef*  gpio;
    uint16_t  pin;
} adc_io_t;


// must be 16
const static adc_io_t adc_config[] =
{
    { GPIOA, 0 },  // 0
    { GPIOA, 1 },  // 1
    { GPIOA, 2 },  // 2
    { GPIOA, 3 },  // 3
    { GPIOA, 4 },  // 4
    { GPIOA, 5 },  // 5
    { GPIOA, 6 },  // 6
    { GPIOA, 7 },  // 7

    { GPIOB, 0 },  // 8
    { GPIOB, 1 },  // 9

    { GPIOC, 0 },  // 10
    { GPIOC, 1 },  // 11
    { GPIOC, 2 },  // 12
    { GPIOC, 3 },  // 13
    { GPIOC, 4 },  // 14
    { GPIOC, 5 },  // 15
};
#define ADC_CHANNELS_COUNT          ( sizeof(adc_config) / sizeof(adc_config[0]) )


const static uint16_t  adc_seq[] =
{
    0,
    1,
    2,
    8,
    9,
};
#define ADC_SEQ_COUNT               ( sizeof(adc_seq) / sizeof(adc_seq[0]) )

static volatile int16_t  adc_value[ADC_SEQ_COUNT];
static int8_t  adc_channel_lut[ADC_CHANNELS_COUNT];
static void (*adc_dma_callback) (void);


void
TIM1_BRK_UP_TRG_COM_IRQHandler( void )
{
    TIM1->SR &= ~TIM_SR_UIF;
}


void
ADC1_IRQHandler( void )
{
//    ADC1->ISR |= ADC_ISR_EOSEQ;
//    ADC1->ISR |= ADC_ISR_EOC;
}


void
DMA1_Channel1_IRQHandler( void )
{
    DMA1->IFCR = DMA_IFCR_CTCIF1;


    if( adc_dma_callback )
    {
        adc_dma_callback();
    }
}


int16_t
adc_value_get( uint16_t ch )
{
    int8_t  index;

    index = adc_channel_lut[ch];

    return ( index >= 0 ) ? adc_value[index] : -1;
}


void
adc_callback_set( void (*callback_func)(void) )
{
    adc_dma_callback = callback_func;
}


void
adc_deinit( void )
{
    TIM1->CR1 &= ~TIM_CR1_CEN;
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;


    /* (1) Stop any ongoing conversion */
    /* (2) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
    /* (3) Disable the ADC */
    /* (4) Wait until the ADC is fully disabled */
    ADC1->CR |= ADC_CR_ADSTP; /* (1) */
    while ((ADC1->CR & ADC_CR_ADSTP) != 0) /* (2) */
    {
    /* For robust implementation, add here time-out management */
    }

    ADC1->CR |= ADC_CR_ADDIS; /* (3) */
}


void
adc_init( void )
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;


    // for debug or error handling only
    // must be other adc_value_get function with error handling
    for( uint32_t i = 0; i < ADC_CHANNELS_COUNT; i++ )
    {
        adc_channel_lut[i] = -1;
    }

    for( uint32_t i = 0; i < ADC_SEQ_COUNT; i++ )
    {
        adc_channel_lut[adc_seq[i]] = i;
    }

    //
    // Configure DMA
    //
    //
    // - MEM2MEM    -                 - Disable memory to memory mode
    // - PL[1:0]    - DMA_CCR_PL_1    - A high priority DMA stream
    // - MSIZE[1:0] - DMA_CCR_MSIZE_0 - Memory size 16bits
    // - PSIZE[1:0] - DMA_CCR_PSIZE_0 - Peripheral size 16bits
    // - MINC       - DMA_CCR_MINC    - Enable memory increment
    // - PINC       -                 - Disable peripheral increment
    // - CIRC       - DMA_CCR_CIRC    - Circular mode enabled
    // - DIR        -                 - Read from peripheral
    // - TEIE       -
    // - HTIE       -
    // - TCIE       - DMA_CCR_TCIE    - Transfer complete interrupt enable
    // - EN         -
    //
    DMA1_Channel1->CCR = DMA_CCR_PL_1 | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | \
                         DMA_CCR_MINC | DMA_CCR_CIRC    | DMA_CCR_TCIE;

    DMA1_Channel1->CNDTR = ADC_SEQ_COUNT;
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)adc_value;

    // Configure NVIC
    NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    DMA1_Channel1->CCR |= DMA_CCR_EN;


    //
    // Configure ADC
    //
    ADC1->CHSELR = 0;
    for( uint32_t i = 0, ch; i < ADC_SEQ_COUNT; i++ )
    {
        ch = adc_seq[i];

        adc_config[ch].gpio->MODER |= GPIO_MODER_MODER0 << (adc_config[ch].pin * 2);

        ADC1->CHSELR |= (1 << adc_seq[i]);
    }

    ADC1->SMPR = ADC_SAMPLE_TIME;
    ADC1->CFGR1  = ADC_CFGR1_DMACFG;
    ADC1->CFGR1 |= ADC_CFGR1_EXTEN_0;         // ext trigger, 0b01 - rising edge
    ADC1->CFGR1 &= ~ADC_CFGR1_EXTSEL;         // source, 0b000 - TRG0 - TIM1_TRGO
//    ADC1->CFGR2 = ADC_CFGR2_CKMODE_1;
    ADC1->CFGR2 &= ~(ADC_CFGR2_CKMODE);     // CKMODE 0 - asynchronous, dedicated 14MHz clock

    // Calibrate ADC before enabling
    ADC1->CR |= ADC_CR_ADCAL;
    while( ADC1->CR & ADC_CR_ADCAL ) ;

    // Activate ADC
    ADC1->CFGR1 |= ADC_CFGR1_DMAEN;
    ADC1->CR |= ADC_CR_ADEN;
    while( !(ADC1->ISR & ADC_ISR_ADRDY) ) ;

    // Adc Interrupt enable
//    NVIC_SetPriority(ADC1_IRQn, 0);
//    NVIC_EnableIRQ(ADC1_IRQn);
//    ADC1->IER = ADC_IER_EOSEQIE;
//    ADC1->IER = ADC_IER_EOCIE;

    // Start first conversion
    ADC1->CR |= ADC_CR_ADSTART;

    //
    // Timer 1 config for sync operation
    //
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->PSC = 0;
    TIM1->ARR = sys_core_clock / ADC_SAMPLE_FREQUENCY_HZ;

    /** Timer Interrupt enable */
//    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0);
//    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
//    TIM1->DIER = TIM_DIER_UIE;

    TIM1->CR2 = TIM_CR2_MMS_1;  // The update event is selected as trigger output
    TIM1->CR1 = TIM_CR1_CEN;
}


