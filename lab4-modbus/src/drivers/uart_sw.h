
#ifndef UART_SW_H_
#define UART_SW_H_



int8_t uartsw_char_put( uint8_t data );
int8_t uartsw_char_get( uint8_t *data );
int8_t uartsw_is_tx_done( void );

void uartsw_init( void );


#endif /* UART_SW_H_ */
