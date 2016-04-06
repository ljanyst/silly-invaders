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

#include <io/IO.h>
#include <io/IO_error.h>
#include "TM4C.h"

//------------------------------------------------------------------------------
// Initialize PLL
//------------------------------------------------------------------------------
void TM4C_init_PLL()
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
// Initialize the board
//------------------------------------------------------------------------------
int32_t IO_init()
{
  TM4C_init_PLL();

  // Enable the floating point coprocessor
  CPAC_REG |= (0x0f << 20);
  __asm__ volatile (
    "dsb\r\n"        // force memory writed before continuing
    "isb\r\n" );     // reset the pipeline
  return 0;
}

//------------------------------------------------------------------------------
// Enable events on IO device
//------------------------------------------------------------------------------
int32_t IO_event_enable(IO_io *io, uint16_t events)
{
  switch(io->type) {
    case IO_UART:
      return TM4C_event_enable_uart(io, events);
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
      return TM4C_event_disable_uart(io, events);
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
