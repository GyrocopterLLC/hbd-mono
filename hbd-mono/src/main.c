/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "gpio.h"
#include "gfxfont.h"
#include "lcd.h"
#include "Fonts/Org_01.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F0 empty sample (trace via NONE).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
static void rtc_init(void);
//static void mini_rand_init(void);
//static uint32_t mini_rand(void);
static void main_screen(void);
void Main_Delay(uint32_t delayms);

__IO uint32_t g_MainSysTick;
// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{

//  rtc_init();

//  mini_rand_init();
  lcd_init();
  lcd_cleardisp();

  GPIO_Clk(GPIOA);
  GPIO_Output(GPIOA,5);

  g_MainSysTick = 0;

  SysTick_Config(SystemCoreClock / 1000);
  NVIC_SetPriority(SysTick_IRQn, 3);

  // Infinite loop
  while (1)
    {
       // Add your code here.
//      uint32_t myrand;
//      for(uint32_t i = 0; i < 14; i++)
//      {
//        // Get a new random number
//        myrand = mini_rand();
//        // Draw a circle
//        lcd_drawCircle((myrand % LCD_WIDTH), ((myrand>>8) % LCD_HEIGHT), ((myrand >> 16) % 8));
//      }

      /*
      for(uint8_t i = 0; i < 32; i++)
        lcd_pix(i,i,1);
      */
      main_screen();
      lcd_show();
      GPIOA->BRR |= 0x0020;
//      lcd_show();
      Main_Delay(500);
      lcd_cleardisp();
      lcd_show();
      GPIOA->BSRR |= 0x0020;
      Main_Delay(500);

    }
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

static void rtc_init(void)
{
  RCC->BDCR |= RCC_BDCR_RTCEN;

  RTC->WPR = 0xCA; // RTC unlock sequence (part 1/2)
  RTC->WPR = 0x53; // RTC unlock sequence (part 2/2)
  RTC->CR &= ~(RTC_CR_WUTE); // disable wake up timer
  while((RTC->ISR & RTC_ISR_WUTWF) == 0); // wait for okay to write
  RTC->CR |= 0x0004; // Wake up timer source is 1Hz clock
  RTC->WUTR = 0; // Every second
  RTC->CR |= RTC_CR_WUTE; // re-enable it

}

static void main_screen(void)
{
  lcd_write(5,16,"Hi there.",&Org_01);
}

/*
static void mini_rand_init(void)
{
  // Turn on CRC module
  RCC->AHBENR |= RCC_AHBENR_CRCEN;
  // Feed my seed
  CRC->INIT = 0xDEADBEEF;
}

static uint32_t mini_rand(void)
{
  // Feed the CRC with my value
  CRC->DR = 0x4B8ECD09;

  // Return the new CRC value
  return (CRC->DR);
}
*/

void Main_Delay(uint32_t delayms)
{
  uint32_t start_time = g_MainSysTick;
  while( (start_time + delayms) > g_MainSysTick) ;
}

/**** INTERRUPTS ****/

void SysTick_Handler(void)
{
  g_MainSysTick++;
}

