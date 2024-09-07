#include "indicator.h"

#include <stdint.h>
#include "stm32f0xx.h"


#define DIGIT_NUM                                 4

#define BTN_PRESSED                    !(GPIOA->IDR & GPIO_IDR_11)

#define PULL_UP_RESIST                 GPIOA->PUPDR |= GPIO_PUPDR_PUPDR11_0

#define DAT_IN                         GPIOA->MODER &= ~(GPIO_MODER_MODER11)
#define DAT_OUT                        GPIOA->MODER |= GPIO_MODER_MODER11_0

#define CLK_OUT                        GPIOA->MODER |= GPIO_MODER_MODER10_0
#define WRT_OUT                        GPIOA->MODER |= GPIO_MODER_MODER12_0

#define CLK_SET                        GPIOA->ODR |= GPIO_ODR_10
#define CLK_CLR                        GPIOA->ODR &= ~GPIO_ODR_10

#define DAT_SET                        GPIOA->ODR |= GPIO_ODR_11
#define DAT_CLR                        GPIOA->ODR &= ~GPIO_ODR_11

#define WRT_SET                        GPIOA->ODR |= GPIO_ODR_12
#define WRT_CLR                        GPIOA->ODR &= ~GPIO_ODR_12


uint16_t number;
static uint16_t factor = 1;
static uint8_t digit_numbers[DIGIT_NUM] = {0};
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
		0b11110111,
		0b11111011,
		0b11111101,
		0b11111110
};


static void button_pressed( uint8_t btn )
{
	switch( btn )
	{
	case 0:
		break;

	case 1:
		number -= factor;
		break;

	case 2:
		number += factor;
		break;

	case 3:
		factor = ( factor >= 1000 ) ? 1 : factor * 10;
		break;
	}
}


static void button_check( uint8_t btn )
{
	static uint8_t btn_states[4][2];
	uint8_t btn_state;

	btn_state = !(GPIOA->IDR & GPIO_IDR_11);

	if( (btn_states[btn][0] == btn_state) && btn_state )
	{
		btn_states[btn][1]++;

		if( btn_states[btn][1] == 3 )
		{
			button_pressed(btn);
		}
		if( btn_states[btn][1] > 50 )
		{
			button_pressed(btn);
		}
	}
	else
	{
		btn_states[btn][1] = 0;
	}

	btn_states[btn][0] = btn_state;
}


static void indicator_proc( void )
{
	static volatile uint8_t stage;
	static volatile uint8_t bit_stage = 15;
	static volatile uint8_t cur_digit;

	uint8_t high_byte = seg_digits[cur_digit];
	uint8_t low_byte = seg_numbers[digit_numbers[cur_digit]];

	uint16_t hex_num = (high_byte << 8) | low_byte;

	switch( stage )
	{
	case 0:
		if(hex_num & (1 << bit_stage))
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

		stage++;
		break;

	case 5:
		DAT_IN;
		PULL_UP_RESIST;

		stage++;
		break;

	case 6:
		button_check(cur_digit);

		DAT_OUT;

		if( cur_digit >= (DIGIT_NUM - 1) )
		{
			cur_digit = 0;
		}
		else
		{
			cur_digit++;
		}

		stage = 0;
		break;
	}
}


static void split_number( void ) {

    for (int i = 0; i < DIGIT_NUM; i++) {
        digit_numbers[i] = 0;
    }

    uint8_t div = 10;
    uint16_t num = number;

    for( uint8_t i = 0; i < DIGIT_NUM; i++ )
    {
    	uint16_t count = 0;

    	for( ; num >= div; num -= div )
		{
			count++;
		}

    	digit_numbers[i] = num;
    	num = count;
    }
}


void TIM14_IRQHandler(void)
{
	static uint16_t timer;

	TIM14->SR &= ~TIM_SR_UIF;

	indicator_proc();

	if( timer >= 1000 )
	{
		if( number < 0 || number > 9999 )
		{
			number = 0;
		}

		split_number();
		timer = 0;
	}
	else
	{
		timer++;
	}
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


void indicator_init( void )
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	DAT_OUT;
	CLK_OUT;
	WRT_OUT;

	timer_init();
}
