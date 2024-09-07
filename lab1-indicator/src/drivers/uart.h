
#ifndef UART_H_
#define UART_H_


#define USART1_CLK_INIT                  do { \
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); \
} while(0);


#define USART1_CLK                       RCC_APB2Periph_USART1

#define USART1_TX_PIN                    GPIO_Pin_14
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define USART1_TX_SOURCE                 GPIO_PinSource14
#define USART1_TX_AF                     GPIO_AF_1

#define USART1_RX_PIN                    GPIO_Pin_15
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define USART1_RX_SOURCE                 GPIO_PinSource15
#define USART1_RX_AF                     GPIO_AF_1


static const uint32_t    baud_t[] = {
        1200, 2400, 4800, 9600, 19200,
        38400, 57600, 115200, 230400
};

enum
{
    BAUDRATE_1200,
    BAUDRATE_2400,
    BAUDRATE_4800,
    BAUDRATE_9600,
    BAUDRATE_19200,
    BAUDRATE_38400,
    BAUDRATE_57600,
    BAUDRATE_115200,
    BAUDRATE_230400,
};


static const uint32_t    parity_t[] = {
        USART_Parity_No, USART_Parity_Odd, USART_Parity_Even
};

enum
{
    PARITY_NONE,
    PARITY_ODD,
    PARITY_EVEN,
};


void uart_flush( void );

long uart_is_transmission_complete( void );
long uart_send( unsigned char byte );
long uart_recv( unsigned char *byte );

void uart_init( uint32_t baudrate, uint32_t parity );


#endif /* UART_H_ */
