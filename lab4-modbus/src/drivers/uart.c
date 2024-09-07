
#include <system.h>

#include "uart.h"


#define RX_QUEUE_SIZE   ( 32 )

static unsigned char    rx_queue[RX_QUEUE_SIZE];
static unsigned short   rx_queue_read_idx;
static unsigned short   rx_queue_write_idx;
static unsigned char    rx_queue_full;

#define TX_QUEUE_SIZE   ( 32 )

static unsigned char    tx_queue[TX_QUEUE_SIZE];
static unsigned short   tx_queue_read_idx;
static unsigned short   tx_queue_write_idx;
static unsigned char    tx_queue_full;

static unsigned char    transmission_complete;



static
int32_t rx_enqueue( uint8_t byte )
{
    int32_t ret = ERROR;

    if( !rx_queue_full )
    {
        rx_queue[rx_queue_write_idx] = byte;

        rx_queue_write_idx = (rx_queue_write_idx + 1) % RX_QUEUE_SIZE;

        if( rx_queue_write_idx == rx_queue_read_idx )
        {
            rx_queue_full = 1;
        }

        ret = SUCCESS;
    }

    return ret;
}


static
int32_t rx_dequeue( uint8_t *byte )
{
    int32_t ret = ERROR;

    if( (rx_queue_read_idx != rx_queue_write_idx) || rx_queue_full )
    {
        *byte = rx_queue[rx_queue_read_idx];

        rx_queue_read_idx = (rx_queue_read_idx + 1) % RX_QUEUE_SIZE;

        rx_queue_full = 0;

        ret = SUCCESS;
    }

    return ret;
}


static
int32_t tx_enqueue( uint8_t byte )
{
    int32_t ret = ERROR;

    if( !tx_queue_full )
    {
        tx_queue[tx_queue_write_idx] = byte;

        tx_queue_write_idx = (tx_queue_write_idx + 1) % TX_QUEUE_SIZE;

        if( tx_queue_write_idx == tx_queue_read_idx )
        {
            tx_queue_full = 1;
        }

        ret = SUCCESS;
    }

    return ret;
}


static
int32_t tx_dequeue( uint8_t *byte )
{
    int32_t ret = ERROR;

    if( (tx_queue_read_idx != tx_queue_write_idx) || tx_queue_full )
    {
        *byte = tx_queue[tx_queue_read_idx];

        tx_queue_read_idx = (tx_queue_read_idx + 1) % TX_QUEUE_SIZE;

        tx_queue_full = 0;

        ret = SUCCESS;
    }

    return ret;
}


void USART1_IRQHandler( void )
{
    uint8_t  byte;

    if( USART_GetITStatus(USART1, USART_IT_RXNE) )
    {
        USART_ClearFlag(USART1, USART_FLAG_RXNE |
                                USART_FLAG_NE   |
                                USART_FLAG_FE   |
                                USART_FLAG_PE   |
                                USART_FLAG_ORE);

        byte = USART_ReceiveData(USART1);
        rx_enqueue(byte);
    }
    else if( USART_GetITStatus(USART1, USART_IT_TXE) )
    {
        if( tx_dequeue(&byte) )
        {
            USART_SendData(USART1, byte);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TC, ENABLE);
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
    else if( USART_GetITStatus(USART1, USART_IT_TC) )
    {
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
        transmission_complete = SUCCESS;

        USART_ClearITPendingBit(USART1, USART_IT_TC);
    }
}


int32_t uart_recv( uint8_t *byte )
{
    int32_t ret;

    NVIC_DisableIRQ(USART1_IRQn);

    ret = rx_dequeue(byte);

    NVIC_EnableIRQ(USART1_IRQn);

    return ret;
}


int32_t uart_send( uint8_t byte )
{
    int32_t ret;

    NVIC_DisableIRQ(USART1_IRQn);

    ret = tx_enqueue(byte);

    if( ret == SUCCESS )
    {
        transmission_complete = 0;

        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }

    NVIC_EnableIRQ(USART1_IRQn);

    return ret;
}


int32_t uart_is_transmission_complete( void )
{
    return transmission_complete;
}


void uart_flush( void )
{
    uint8_t  byte;

    while( uart_recv(&byte) );

    USART_ReceiveData(USART1);
}


static void
usart_debug_proc( void )
{
}


void uart_init( uint32_t baudrate, uint32_t parity )
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(USART1_TX_GPIO_CLK, ENABLE);
    RCC_AHBPeriphClockCmd(USART1_RX_GPIO_CLK, ENABLE);

    /* Enable USART clock */
    USART1_CLK_INIT;

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(USART1_TX_GPIO_PORT, USART1_TX_SOURCE, USART1_TX_AF);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(USART1_RX_GPIO_PORT, USART1_RX_SOURCE, USART1_RX_AF);

    /* Configure USART Tx and Rx as alternate function push-pull */
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
    GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
    GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);

    /* USARTx configuration */
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = ( parity != 0 ) ?
            USART_WordLength_9b : USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = ( parity ) ? USART_Parity_Even : USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* Configure USART interrupts */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_SetPriority(USART1_IRQn, 3);
    NVIC_EnableIRQ(USART1_IRQn);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);

    sys_service_add(&usart_debug_proc);
}
