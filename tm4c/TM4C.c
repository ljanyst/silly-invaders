//------------------------------------------------------------------------------
// Copyright (c) 2016 by Lukasz Janyst <lukasz@jany.st>
//------------------------------------------------------------------------------
// This file is part of silly-invaders.
//
// silly-invaders is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// silly-invaders is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with silly-invaders.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#define __IO_IMPL__

#include <io/IO.h>
#include <io/IO_error.h>
#include <io/IO_malloc_low.h>
#include "TM4C.h"
#include "TM4C_events.h"
#include "TM4C_gpio.h"
#include "TM4C_dma.h"

extern unsigned long __bss_end_vma;

//------------------------------------------------------------------------------
// Set up heap
//------------------------------------------------------------------------------
void TM4C_heap_init(uint32_t stack_size)
{
  //----------------------------------------------------------------------------
  // Calculate the heap boundry
  //----------------------------------------------------------------------------
  uint8_t *heap_start  = (uint8_t *)&__bss_end_vma;
  uint8_t *stack_start = (uint8_t *)0x20007ff8;

  if(!stack_size) {
    IO_set_up_heap(heap_start, stack_start);
    return;
  }

  uint32_t tmp = 0x20007ff8 - stack_size;
  tmp = (((tmp-1)>>5)<<5); // align to 32 bytes making it larger if necessary
  uint8_t *stack_end   = (uint8_t *)tmp;
  uint8_t *stack_guard = stack_end-32;

  //----------------------------------------------------------------------------
  // Protect the guard chunk
  //----------------------------------------------------------------------------
  MPUCTRL_REG |= (uint32_t)0x05; // enable MPU and the background region

  uint32_t val = (uint32_t)stack_guard;
  val |= 0x10; // valid
  val |= 0x07; // highest priority region
  MPUBASE_REG &= ~0xfffffff7;
  MPUBASE_REG |= val;

  val = 0;
  val |= (1 << 28); // disable instruction fetches
  val |= (4 << 1);  // 0x04 == 32bytes
  val |= 1;         // enable the region
  MPUATTR_REG &= ~0x173fff3f;
  MPUATTR_REG |= val;

  //----------------------------------------------------------------------------
  // Set up the heap
  //----------------------------------------------------------------------------
  IO_set_up_heap(heap_start, stack_guard);
}

//------------------------------------------------------------------------------
// Initialize PLL
//------------------------------------------------------------------------------
void TM4C_pll_init()
{
  // Section 5.3 of the data heet

  // Enable the RCC2 register
  RCC2_REG |= (1 << 31);

  // Set the BYPASS bit to make the microcontroller run on the "raw" clock
  // source
  RCC2_REG |= (1 << 11);

  // Select the 16MHz cristal by setting the XTAL bits
  RCC_REG &= ~(0x1f << 6);
  RCC_REG |= (0x15 << 6);

  // Configure the main oscilator source, OSCSRC2 bits
  RCC2_REG &= ~(7 << 4);

  // Activate PLL by clearing the PWRDN2 bit
  RCC2_REG &= ~(1 << 13);

  // Use 400 MHz base PLL
  RCC2_REG |= (1 << 30);

  // Configure the divisor for 80 MHz
  RCC2_REG &= ~(0x0000007f << 22);
  RCC2_REG |= (4 << 22);

  // Wait for PLL to lock by polling the PLLLRIS bit
  while((RIS_REG & (1 << 6)) == 0);

  // Enable PLL by clearing the bypass
  RCC2_REG &= ~(1 << 11);
}

//------------------------------------------------------------------------------
// Count time in miliseconds
//------------------------------------------------------------------------------
static uint64_t time = 0;
void systick_handler()
{
  ++time;
}

//------------------------------------------------------------------------------
// Initialize the board
//------------------------------------------------------------------------------
int32_t TM4C_init(uint32_t stack_size)
{
  TM4C_pll_init();
  TM4C_dma_init();
  TM4C_heap_init(stack_size);

  // Enable the floating point coprocessor
  CPAC_REG |= (0x0f << 20);
  __asm__ volatile (
    "dsb\r\n"        // force memory writed before continuing
    "isb\r\n" );     // reset the pipeline

  STCTRL_REG = 0;          // disable SysTick
  STRELOAD_REG = 79999;    // fire every milisecond
  STCURRENT_REG = 0;       // reload
  STCTRL_REG = 0x07;       // core clock and interrupts
  return 0;
}

//------------------------------------------------------------------------------
// Enable events on IO device
//------------------------------------------------------------------------------
int32_t IO_event_enable(IO_io *io, uint16_t events)
{
  switch(io->type) {
    case IO_UART:
      return TM4C_uart_event_enable(io, events);
    case IO_SSI:
      return TM4C_ssi_event_enable(io, events);
    case IO_GPIO:
      return TM4C_gpio_event_enable(io, events);
    case IO_ADC:
      return TM4C_adc_event_enable(io, events);
  }
  return -IO_ENOSYS;
}

//------------------------------------------------------------------------------
// Disable events on IO device
//------------------------------------------------------------------------------
int32_t IO_event_disable(IO_io *io, uint16_t events)
{
  switch(io->type) {
    case IO_UART:
      return TM4C_uart_event_disable(io, events);
    case IO_SSI:
      return TM4C_ssi_event_disable(io, events);
    case IO_GPIO:
      return TM4C_gpio_event_disable(io, events);
    case IO_ADC:
      return TM4C_adc_event_disable(io, events);
  }
  return -IO_ENOSYS;
}

//------------------------------------------------------------------------------
// Enable interrupts
//------------------------------------------------------------------------------
void IO_enable_interrupts()
{
  __asm__ volatile("cpsie i");
}

//------------------------------------------------------------------------------
// Disable interrupts
//------------------------------------------------------------------------------
void IO_disable_interrupts()
{
  __asm__ volatile("cpsid i");
}

//------------------------------------------------------------------------------
// Wait for an interrupt
//------------------------------------------------------------------------------
void IO_wait_for_interrupt()
{
  __asm__ volatile("wfi");
}

//------------------------------------------------------------------------------
// Enable an interrupt
//------------------------------------------------------------------------------
void TM4C_enable_interrupt(uint8_t number, uint8_t priority)
{
   uint8_t en_bit   = number % 32;
   uint8_t en_reg   = number / 32;
   uint8_t pri_bits = (number % 4) * 8 + 5;
   uint8_t pri_reg  = number / 4;

   NVIC_EN_REG(en_reg) |= (1 << en_bit);
   NVIC_PRI_REG(pri_reg) &= ~(0x07 << pri_bits);
   NVIC_PRI_REG(pri_reg) |= ((priority & 0x07) << pri_bits);
}

//------------------------------------------------------------------------------
// Get time
//------------------------------------------------------------------------------
uint64_t IO_time()
{
  return time;
}
