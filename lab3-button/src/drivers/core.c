
#include <stdint.h>

#include "core.h"


uint32_t  sys_core_clock    = 8000000;


void  core_iwdt_reset()
{
    IWDG->KR = 0xAAAA;  // reload key
}


void  core_iwdt_init()
{
    // Set watchdog period to 2 seconds
    IWDG->KR    = 0x0000CCCC;       // Watchdog start
    IWDG->KR    = 0x00005555;       // Watchdog write unlock

    IWDG->PR    = 0x05;             // Presc  128
    IWDG->RLR   = 0x271;            // Reload 625

    while( IWDG->SR ) {};           // Wait for the registers to be updated

    IWDG->KR    = 0x0000AAAA;       // Refresh the counter value with IWDG_RLR
}


void  core_powerdown()
{
    /* Select STANDBY mode */
    PWR->CR |= PWR_CR_PDDS;

    /* Set SLEEPDEEP bit of Cortex-M0 System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Request Wait For Interrupt */
    __WFI();
}


/**
  * @brief  Update SystemCoreClock according to Clock Register Values
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *
  * @note   - The system frequency computed by this function is not the real 
  *           frequency in the chip. It is calculated based on the predefined 
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *                                              
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *                          
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
  *             or HSI_VALUE(*) multiplied/divided by the PLL factors.
  *
  *         (*) HSI_VALUE is a constant defined in stm32f0xx.h file (default value
  *             8 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *
  *         (**) HSE_VALUE is a constant defined in stm32f0xx.h file (default value
  *              8 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  * @param  None
  * @retval None
  */
void
core_clock_update( void )
{
    __I uint8_t  AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    uint32_t  tmp = 0, pllmull = 0, pllsource = 0, prediv1factor = 0;


    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch( tmp )
    {
    case 0x00:  /* HSI used as system clock */
        sys_core_clock = HSI_VALUE;
        break;
    case 0x04:  /* HSE used as system clock */
        sys_core_clock = HSE_VALUE;
        break;
    case 0x08:  /* PLL used as system clock */
        /* Get PLL clock source and multiplication factor ----------------------*/
        pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
        pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
        pllmull = ( pllmull >> 18) + 2;

        if (pllsource == 0x00)
        {
            /* HSI oscillator clock divided by 2 selected as PLL clock entry */
            sys_core_clock = (HSI_VALUE >> 1) * pllmull;
        }
        else
        {
            prediv1factor = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
            /* HSE oscillator clock selected as PREDIV1 clock entry */
            sys_core_clock = (HSE_VALUE / prediv1factor) * pllmull;
        }
        break;
    default: /* HSI used as system clock */
        sys_core_clock = HSI_VALUE;
        break;
    }

    /* Compute HCLK clock frequency ----------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];

    /* HCLK clock frequency */
    sys_core_clock >>= tmp;
}

/**
  * @brief  Configures the System clock frequency, AHB/APBx prescalers and Flash
  *         settings.
  * @note   This function should be called only once the RCC clock configuration
  *         is reset to the default reset state (done in SystemInit() function).
  * @param  None
  * @retval None
  */
static void
core_clock_set( void )
{
#if defined INTERNAL_CLOCK_SOURCE
    /*Configure all clocks to max for best performance.
    * If there are EMI, power, or noise problems, try slowing the clocks*/

    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

    /* PLL configuration = HSI/2 * 12 = 48 MHz */
    RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC);
    RCC->CFGR |=   RCC_CFGR_PLLMUL12;

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while( (RCC->CR & RCC_CR_PLLRDY) == 0 )
    {
        ;
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |=   RCC_CFGR_SW_PLL;

    /* Wait till PLL is used as system clock source */
    while( (RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL )
    {
        ;
    }
#else
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while( (HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT) );

    if( (RCC->CR & RCC_CR_HSERDY) != RESET )
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if( HSEStatus == (uint32_t)0x01 )
    {
        /* Enable Prefetch Buffer and set Flash Latency */
        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

        /* PLL configuration = (HSE / 2) * 6 = 48 MHz */
        RCC->CFGR  &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
        RCC->CFGR  |= (uint32_t)(RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLMULL6);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV1_DIV2);

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
    }
    #endif

    core_clock_update();
}


void
core_init (void)
{
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    #if defined(STM32F051)
    /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
    RCC->CFGR &= (uint32_t)0xF8FFB80C;
    #else
    /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
    RCC->CFGR &= (uint32_t)0x08FFB80C;
    #endif /* STM32F051 */

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
    RCC->CFGR &= (uint32_t)0xFFC0FFFF;

    /* Reset PREDIV1[3:0] bits */
    RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

    /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFFEAC;

    /* Reset HSI14 bit */
    RCC->CR2 &= (uint32_t)0xFFFFFFFE;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;

    /* Configure the System clock frequency, AHB/APBx prescalers and Flash settings */
    core_clock_set();
}


