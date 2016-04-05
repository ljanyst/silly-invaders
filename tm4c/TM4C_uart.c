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

#include <io/IO_uart.h>
#include <io/IO_error.h>
#include "TM4C.h"

//------------------------------------------------------------------------------
// GPIO pins for UART
//------------------------------------------------------------------------------
struct uart_gpio_data {
  uint8_t port;
  uint8_t rx;
  uint8_t tx;
};

static const struct uart_gpio_data uart_gpio[] = {
  {GPIO_PORTA_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM},
  {GPIO_PORTB_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM},
  {GPIO_PORTD_NUM, GPIO_PIN6_NUM, GPIO_PIN7_NUM},
  {GPIO_PORTC_NUM, GPIO_PIN6_NUM, GPIO_PIN7_NUM},
  {GPIO_PORTC_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM},
  {GPIO_PORTE_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM},
  {GPIO_PORTD_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM},
  {GPIO_PORTE_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM}
};

//------------------------------------------------------------------------------
// Write a byte to given UART
//------------------------------------------------------------------------------
int32_t uart_write_normal(IO_output *out, void *data, uint32_t length)
{
  uint32_t uart_offset = (uint32_t)out->data;
  const uint8_t *b_data = data;
  for(uint32_t i = 0; i < length; ++i) {
    // we cannot write if TXFF is 1
    if(out->flags & IO_NONBLOCKING) {
      if((UART_REG(uart_offset, UART_FR) & 0x20) != 0) {
        if(i == 0) return -IO_EWOULDBLOCK;
        else return i;
      }
    }
    else
      while((UART_REG(uart_offset, UART_FR) & 0x20) != 0);
    UART_REG(uart_offset, UART_DR) = b_data[i];
  }
  return length;
}

//------------------------------------------------------------------------------
// Read a byte from given UART
//------------------------------------------------------------------------------
int32_t uart_read_normal(IO_input *in, void *data, uint32_t length)
{
  uint32_t uart_offset = (uint32_t)in->data;
  uint8_t *b_data = data;
  for(uint32_t i = 0; i < length; ++i) {
    // we cannot read if RXFE is 1
    if(in->flags & IO_NONBLOCKING) {
      if((UART_REG(uart_offset, UART_FR) & 0x10) != 0) {
        if(i == 0) return -IO_EWOULDBLOCK;
        else return i;
      }
    }
    else
      while((UART_REG(uart_offset, UART_FR) & 0x10) != 0);
    b_data[i] = UART_REG(uart_offset, UART_DR) & 0xff;
  }
  return length;
}

//------------------------------------------------------------------------------
// Initialize given UART module
//------------------------------------------------------------------------------
int32_t IO_uart_init(uint8_t module, uint16_t flags, uint32_t baud,
  IO_input *input, IO_output *output)
{
  if(module > 7)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Initialize the hardware
  //----------------------------------------------------------------------------
  uint8_t  port        = uart_gpio[module].port;
  uint8_t  rx          = uart_gpio[module].rx;
  uint8_t  tx          = uart_gpio[module].tx;
  uint16_t port_offset = port * GPIO_PORT_OFFSET;
  uint16_t uart_offset = module * UART_MODULE_OFFSET;

  // enable the uart cloc
  RCGCUART_REG |= (1 << module);

  // enable the AHB and GPIO clock
  GPIOHBCTL_REG |= (1 << port);
  RCGCGPIO_REG  |= (1 << port);

  // PD7 is reserved for NMI and needs to be unlocked
  if(module == 2) {
    GPIO_REG(GPIO_PORTD, GPIO_LOCK)  = 0x4c4f434b;
    GPIO_REG(GPIO_PORTD, GPIO_CR)   |= 0x80;
  }

  // enable alternative function of the pins
  GPIO_REG(port_offset, GPIO_AFSEL) |= (1 << rx);
  GPIO_REG(port_offset, GPIO_AFSEL) |= (1 << tx);

  GPIO_REG(port_offset, GPIO_PCTL) &= ~(0x0f << (rx*4));
  GPIO_REG(port_offset, GPIO_PCTL) &= ~(0x0f << (tx*4));
  GPIO_REG(port_offset, GPIO_PCTL) |=  (0x01 << (rx*4));
  GPIO_REG(port_offset, GPIO_PCTL) |=  (0x01 << (tx*4));

  // disable the analog function of the pins
  GPIO_REG(port_offset, GPIO_AMSEL) &= ~(1 << rx);
  GPIO_REG(port_offset, GPIO_AMSEL) &= ~(1 << tx);

  // set pin directions
  GPIO_REG(port_offset, GPIO_DIR) &= ~(1 << rx);
  GPIO_REG(port_offset, GPIO_DIR) |= (1 << tx);

  // enable the pins
  GPIO_REG(port_offset, GPIO_DEN) |= (1 << rx);
  GPIO_REG(port_offset, GPIO_DEN) |= (1 << tx);

  // calculate the desired baud rate
  float brd = ((float)80000000) / (16.0 * baud);
  unsigned long ibrd = (int)brd;
  unsigned long fbrd = (int)((brd-ibrd) * 64.0 + 0.5);

  // disable uart
  UART_REG(uart_offset, UART_CTL) &= ~0x01;

  // set up the baud rate
  UART_REG(uart_offset, UART_IBRD) = ibrd;
  UART_REG(uart_offset, UART_FBRD) = fbrd;

  // no parity, 8-bits, FIFOs
  UART_REG(uart_offset, UART_LCRH) = 0x00000070;

  // enable uart
  UART_REG(uart_offset, UART_CTL) |= 0x01;

  //----------------------------------------------------------------------------
  // Initialize the software
  //----------------------------------------------------------------------------
  if(output) {
    output->data = (void *)(uint32_t)uart_offset;
    output->write = uart_write_normal;
    output->flags = flags;
  }

  if(input) {
    input->data = (void *)(uint32_t)uart_offset;
    input->read = uart_read_normal;
    input->flags = flags;
  }

  return 0;
}
