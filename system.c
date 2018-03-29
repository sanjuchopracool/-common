#include "system.h"

#ifdef USE_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
extern void xPortSysTickHandler(void);
#endif

#ifdef STM32F3DISCOVERY
#include "stm32f30x.h"
#endif

#if defined(STM32F429_439xx) || defined(STM32F40_41xxx)
#include <stm32f4xx.h>
#endif

// current uptime for 1kHz systick timer,
static __IO uint64_t sysTickUptime = 0;

// cycles per microsecond
static uint32_t usTicks = 0;

/**
 * @brief it basically calculate the usticks
 *        that is not of ticks per microseconds
 */
static void cycleCounterInit(void)
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    usTicks = clocks.SYSCLK_Frequency / 1000000;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
#ifdef USE_FREERTOS
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
#endif  /* INCLUDE_xTaskGetSchedulerState */
        xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
    }
#endif  /* INCLUDE_xTaskGetSchedulerState */
#endif /* USE_FREERTOS*/
    sysTickUptime++;
}

// Return system uptime in milliseconds (rollover in 49 days)
uint64_t millis(void)
{
    return sysTickUptime;
}

// Return system uptime in microseconds (rollover in 70minutes)
uint64_t micros(void)
{
    register uint64_t ms;
    register uint32_t cycle_cnt;
    do {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;

        /*
         * If the SysTick timer expired during the previous instruction, we need to give it a little time for that
         * interrupt to be delivered before we can recheck sysTickUptime:
         */
        __asm volatile("\tnop\n");
    } while (ms != sysTickUptime);
    return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

void delayMicroseconds(uint32_t us)
{
    uint32_t ms = us/ 1000; // convert to millisecond
    if ( ms )
    {
        delay( ms );
    }
    us = us % 1000;
    volatile uint64_t now = micros();
    while ( micros() - now < us);
}

void delay(uint32_t ms)
{
    volatile uint64_t now = millis();
    while ( millis() - now < ms );
}

void systemInit(void)
{
    // Init cycle counter
    cycleCounterInit();

    /* Setup SysTick Timer for 1 msec interrupts.
     ------------------------------------------
    1. The SysTick_Config() function is a CMSIS function which configure:
       - The SysTick Reload register with value passed as function parameter.
       - Configure the SysTick IRQ priority to the lowest value (0x0F).
       - Reset the SysTick Counter register.
       - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
       - Enable the SysTick Interrupt.
       - Start the SysTick Counter.

    2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
       SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
       SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
       inside the misc.c file.

    3. You can change the SysTick IRQ priority by calling the
       NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function
       call. The NVIC_SetPriority() is defined inside the core_cm4.h file.

    4. To adjust the SysTick time base, use the following formula:

         Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)

       - Reload Value is the parameter to be passed for SysTick_Config() function
       - Reload Value should not exceed 0xFFFFFF
   */
    SysTick_Config(SystemCoreClock / 1000);
}
