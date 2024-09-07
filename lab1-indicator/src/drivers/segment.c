#include <stdint.h>
#include "stm32f0xx.h"
#include "segment.h"


#define DIGIT_NUM                                 4

#define CLK_SET                        GPIOA->ODR |= GPIO_ODR_10
#define CLK_CLR                        GPIOA->ODR &= ~GPIO_ODR_10

#define DAT_SET                        GPIOA->ODR |= GPIO_ODR_11
#define DAT_CLR                        GPIOA->ODR &= ~GPIO_ODR_11

#define WRT_SET                        GPIOA->ODR |= GPIO_ODR_12
#define WRT_CLR                        GPIOA->ODR &= ~GPIO_ODR_12



static uint8_t digit_numbers[DIGIT_NUM];
static uint8_t seg_numbers[10] = {
		0b11000000,
		0b11111001,
		0b10100100,
		0b10110000,
		0b10011001,
		0b10010010,
		0b10000010,
		0b11111000,
		0b10000000,
		0b10010000
};
static uint8_t seg_digits[4] = {
		0b11111110,
		0b11111101,
		0b11111011,
		0b11110111
};


static void display_digit( void )
{
	static volatile uint8_t stage;
	static volatile uint8_t bit_stage = 15;
	static volatile uint8_t seg_stage;

	uint8_t high_byte = seg_digits[seg_stage];
	uint8_t low_byte = seg_numbers[digit_numbers[seg_stage]];

	uint16_t number = (high_byte << 8) | low_byte;

	switch( stage )
	{
	case 0:
		if(number & (1 << bit_stage))
		{
			DAT_SET;
		}
		else
		{
			DAT_CLR;
		}

		stage++;
		break;

	case 1:
		CLK_SET;

		stage++;
		break;

	case 2:
		CLK_CLR;

		if( bit_stage == 0 )
		{
			bit_stage = 15;
			stage++;
		}
		else
		{
			bit_stage--;
			stage = 0;
		}
		break;

	case 3:
		WRT_SET;

		stage++;
		break;

	case 4:
		WRT_CLR;

		if( seg_stage >= (DIGIT_NUM - 1) )
		{
			seg_stage = 0;
		}
		else
		{
			seg_stage++;
		}

		stage = 0;
		break;
	}
}


static void split_number( const char* numbers )
{
	for( int8_t i = 0; i < DIGIT_NUM; i++ )
	{
		if( numbers[i] )
		{
			digit_numbers[i] = numbers[i] - '0';
		}
		else
		{
			digit_numbers[i] = 0;
		}
	}
}


void TIM14_IRQHandler(void)
{
	TIM14->SR &= ~TIM_SR_UIF;

	display_digit();
}


void timer_init( void )
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

	TIM14->ARR = 4799;
	TIM14->EGR = TIM_EGR_UG;
	TIM14->CR1 = TIM_CR1_CEN;

	TIM14->DIER |= TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM14_IRQn);
}


void segment_init( const char* numbers )
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER |= (GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0);

	split_number( numbers );

	timer_init();
}
