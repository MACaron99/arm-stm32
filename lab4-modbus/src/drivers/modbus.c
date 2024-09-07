#include <modbus.h>
#include <system.h>
#include <uart.h>
#include <crc.h>
#include <indicator.h>


#define ADDRESS                                           0x01
#define REGS_START_ADDR                                   1000
#define REGS_NUM                                            10
#define REQ_RESP_SIZE                                       32


static uint16_t registers[REGS_NUM];



static
void generate_crc( uint8_t *response, uint16_t resp_len )
{
	uint16_t crc = crc_16_ibm(response, resp_len);
	response[resp_len] = crc & 0xFF;
	response[resp_len + 1] = crc >> 8;
}


static
uint8_t req_is_valid( uint8_t *request, uint8_t byte_num )
{
	if( byte_num < 4 || request[0] != ADDRESS)
	{
		return 0;
	}

	uint16_t crc_recv = (request[byte_num - 1] << 8) | request[byte_num - 2];
	uint16_t crc_calc = crc_16_ibm(request, byte_num - 2);

	if( crc_recv != crc_calc )
	{
		return 0;
	}

	return 1;
}


static
void handle_error( uint8_t *request, uint8_t *response, uint16_t *resp_len )
{
	response[0] = request[0];
	response[1] = request[1] | 0x80;
	response[2] = 0x02;
	*resp_len = 3;

	generate_crc(response, *resp_len);
	*resp_len += 2;
}


static
void modbus_read( uint8_t *request, uint8_t *response, uint16_t *resp_len )
{
	uint16_t start_address = (request[2] << 8) | request[3];
	uint16_t reg_number = (request[4] << 8) | request[5];
    
    if( start_address < REGS_START_ADDR || (start_address - REGS_START_ADDR + reg_number) > REGS_NUM )
    {
    	handle_error(request, response, resp_len);
        return;
    }
    
    response[0] = request[0];
    response[1] = request[1];
    response[2] = reg_number * 2;
    *resp_len = (3 + reg_number * 2);
    
    for( uint16_t i = 0; i < reg_number; i++ )
    {
        response[3 + i * 2] = registers[start_address - REGS_START_ADDR + i] >> 8;
        response[4 + i * 2] = registers[start_address - REGS_START_ADDR + i] & 0xFF;
    }
    
    generate_crc(response, *resp_len);
    *resp_len += 2;
}


static
void modbus_write( uint8_t *request, uint8_t *response, uint16_t *resp_len )
{
    uint16_t start_address = (request[2] << 8) | request[3];
    uint16_t num_registers = (request[4] << 8) | request[5];

    if( start_address < REGS_START_ADDR || (start_address - REGS_START_ADDR + num_registers) > REGS_NUM )
    {
    	handle_error(request, response, resp_len);
        return;
    }
    
    for( uint16_t i = 0; i < num_registers; i++ )
    {
        registers[start_address - REGS_START_ADDR + i] = (request[7 + i * 2] << 8) | request[8 + i * 2];
    }
    
    response[0] = request[0];
    response[1] = request[1];
    response[2] = request[2];
    response[3] = request[3];
    response[4] = request[4];
    response[5] = request[5];
    *resp_len = 6;

    generate_crc(response, *resp_len);
    *resp_len += 2;
}


void modbus_proc( void )
{
	static timer_t ticks;
	static uint8_t stage;
	static uint8_t req_index;
	static uint8_t resp_index;
	static uint16_t resp_len;
    static uint8_t request[REQ_RESP_SIZE];
    static uint8_t response[REQ_RESP_SIZE];
    
    registers[0] = number;

    switch( stage )
	{
    case 0:
    	ticks = sys_timer_tick_get();

    	req_index = 0;
    	resp_index = 0;
		resp_len = 0;
    	stage++;

    	// no break;

	case 1:

		if( uart_recv(&request[req_index]) )
		{
			ticks = sys_timer_tick_get();

			if( ++req_index >= REQ_RESP_SIZE )
			{
				stage = 0;
			}
		}
		if( (sys_timer_tick_get() - ticks) >= 20  )
		{
			stage++;
		}
		break;

	case 2:
		if( req_is_valid(request, req_index) )
		{
			switch (request[1])
			{
			case 0x03:
				modbus_read(request, response, &resp_len);
				break;

			case 0x10:
				modbus_write(request, response, &resp_len);
				break;

			default:
				handle_error(request, response, &resp_len);
				break;
			}
			stage++;
		}
		else
		{
			stage = 0;
		}
		break;

	case 3:
		if( resp_index < resp_len )
		{
			if( uart_send(response[resp_index]) )
			{
				resp_index++;
			}
		}
		else
		{
			stage = 0;
		}
	}

    number = registers[0];
}
