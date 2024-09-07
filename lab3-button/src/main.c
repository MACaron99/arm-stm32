
#include <system.h>
#include <drivers/adc.h>
#include <modules/aps.h>
#include "drivers/indicator.h"


static pt_t     h_sys_init;
static pt_t     h_sys_process;
static pt_t     h_sys_shutdown;

static int8_t   sys_status;
static int8_t   sys_is_service_mode;
static int8_t   sys_alarm_state;
static int8_t   sys_run_state;



int32_t sys_status_get( void )
{
    return sys_status;
}


int32_t sys_service_mode_get( void )
{
    return sys_is_service_mode;
}


void sys_service_mode_set( int32_t state )
{
    sys_is_service_mode = state;
}


int32_t sys_run_state_get( void )
{
    return sys_run_state;
}


int32_t sys_alarm_state_get( void )
{
    return sys_alarm_state;
}


static void sys_status_proc( void )
{
    int8_t status = SYSTEM_STATUS_UNKNOWN;

    sys_status = status;
}


static void sys_alarm_relay_proc( void )
{
    switch( sys_status )
    {
    case SYSTEM_STATUS_MALFUNCTION:
    case SYSTEM_STATUS_INPUT_FAULT:
    case SYSTEM_STATUS_OUTPUT_FAULT:
        sys_alarm_state = true;
        RELAY_ALARM_ON;
        break;

    case SYSTEM_STATUS_UNDERCOOL:
    case SYSTEM_STATUS_OVERHEAT:
        sys_alarm_state = true;
        RELAY_ALARM_ON;
        break;

    default:
        sys_alarm_state = false;
        RELAY_ALARM_OFF;
        break;
    }
}


static pt_status_t sys_shutdown( void )
{
    static timer_t  timer;


    PT_ENTER();

    sys_run_state = false;

    sys_timer_sleep(timer, _MS(100));

    core_powerdown();

    PT_EXIT();
}


static pt_status_t sys_process( void )
{
    PT_ENTER();

    while( 1 )
    {
        sys_status_proc();
        sys_alarm_relay_proc();

        if( aps_is_power_good() == false )
        {
            pt_create(&h_sys_shutdown, &sys_shutdown);
            break;
        }

//        sys_param_set(SYS_STATUS,  (conv_protection_flags_get() << 8) | sys_status);

        PT_YIELD();
    }

    PT_EXIT();
}


static pt_status_t sys_init( void )
{
    static timer_t  timer;
#if DEV_EXTMODBUS == 1
    static uint8_t  config_mode = false;
#endif

    PT_ENTER();

    sys_run_state = true;
    sys_status = SYSTEM_STATUS_OFFLINE;

    /* Handle TCOMP initialisation stage */
    if( sys_param_get(TCOMP_MAX_BOUNDARY) != 0 )
    {
    }
    else
    {
        sys_param_set(TCOMP_VALUE, 0);
    }


    while( sys_timer_tick_diff_get(timer) < _MS(2000)
#if DEV_EXTMODBUS == 1
            && (config_mode == false)
#endif
         )
    {
//        switch( ind_event_view() )
//        {
//        case IEV_BUTTON_MODE_PRESSED:
//            break;
//        case IEV_BUTTON_MODE_LONG_PRESSED:
//    #if ( DEV_EXTMODBUS == 1 )
//            // goto modbus menu
//            menu_set(Menu_Configuration);
//            ind_event_clear();
//            config_mode = true;
//            sys_timer_tick_update(timer);
//    #endif
//            break;
//        case IEV_CALIBRATION_MODE_ENTER:
//            sys_is_service_mode = true;
//            ind_service_mode_set(true);
//        }

        PT_YIELD();
    }

#if ( DEV_EXTMODBUS == 1 )
    if( config_mode )
    {
        fan_state_set(Fan_Auto);

        while( ind_event_view() != IEV_BUTTON_MODE_UNPRESSED )
        {
            PT_YIELD();
        }

        ind_event_clear();
        menu_lock(false);

        while( ind_event_view() != IEV_BUTTON_MODE_LONG_PRESSED )
        {
            PT_YIELD();
        }

        sys_param_save(MODBUS_ADDRESS);
        sys_param_save(MODBUS_BAUDRATE);
        sys_param_save(MODBUS_PARITY);

        fan_state_set(Fan_Off);

        pt_create(&h_sys_shutdown, &sys_shutdown, 0);

        PT_EXIT();
    }

#endif

    aps_output_voltage_check();

    if( sys_error_get() )
    {
//        sys_status = SYSTEM_STATUS_MALFUNCTION;

        while( 1 )
        {
            PT_YIELD();
        }
    }

    if( sys_is_service_mode == false)
    {
    #if ( DEV_NETPSU == 1 )
        inter_comm_mode_set(Remote_Online);
    #endif
    }
    else
    {
    }


    pt_create(&h_sys_process, &sys_process);

    PT_EXIT();
}


static void io_init( void )
{
    GPIO_InitTypeDef  gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    //
    // GPIO Clock enable
    //
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);


#if ( DEV_DCTRL == 1 )
    //
    // Configure discrette GPIO
    //
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_4 | GPIO_Pin_3;
    gpio.GPIO_Mode  = GPIO_Mode_IN;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);
#endif

    //
    // Configure alarm relay GPIO
    //
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_7;
    gpio.GPIO_Mode  = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);

    //
    // Configure address GPIO
    //
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_5 | GPIO_Pin_4;
    gpio.GPIO_Mode  = GPIO_Mode_IN;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    //
    // Configure DX GPIO
    //
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_3;
    gpio.GPIO_Mode  = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);
}


void main( void ) __attribute__ ((noreturn));
void main( void )
{
    core_init();
    core_iwdt_init();

    io_init();
    adc_init();
    aps_init();

    sys_param_init();
    sys_timer_init();

    pt_create(&h_sys_init, &sys_init);

    indicator_init();

    for( ;; )
    {
        core_iwdt_reset();

        sys_service_run();

        pt_schedule();
    }
}


