
#ifndef ADC_H_
#define ADC_H_


#define ADC_SAMPLE_FREQUENCY_HZ        ( 10000 )


int16_t  adc_value_get( uint16_t ch );
void  adc_callback_set( void (*callback_func)(void) );

void  adc_deinit( void );

void  adc_init( void );


#endif /* ADC_H_ */


