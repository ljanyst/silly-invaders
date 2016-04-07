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
struct uart_data {
  uint8_t gpio_port;
  uint8_t gpio_pin_rx;
  uint8_t gpio_pin_tx;
  uint8_t interrupt_num;
};

static const struct uart_data uart_info[] = {
  {GPIO_PORTA_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM, 5},
  {GPIO_PORTB_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM, 6},
  {GPIO_PORTD_NUM, GPIO_PIN6_NUM, GPIO_PIN7_NUM, 33},
  {GPIO_PORTC_NUM, GPIO_PIN6_NUM, GPIO_PIN7_NUM, 59},
  {GPIO_PORTC_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM, 60},
  {GPIO_PORTE_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM, 61},
  {GPIO_PORTD_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM, 62},
  {GPIO_PORTE_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM, 63}
};

//------------------------------------------------------------------------------
// IO devices
//------------------------------------------------------------------------------
static struct IO_io *uart_devices[8];

//------------------------------------------------------------------------------
// Handle uart interrupt
//------------------------------------------------------------------------------
static void uart_handler(uint8_t module)
{
  uint16_t events = 0;
  uint32_t module_offset = module * UART_MODULE_OFFSET;

  if(UART_REG(module_offset, UART_RIS) & 0x10) events |= IO_EVENT_READ;
  if(UART_REG(module_offset, UART_RIS) & 0x20) events |= IO_EVENT_WRITE;

  if(uart_devices[module] && uart_devices[module]->event)
    uart_devices[module]->event(uart_devices[module], events);
}

//------------------------------------------------------------------------------
// Interrupt handlers
//------------------------------------------------------------------------------
void uart0_handler() { uart_handler(0); }
void uart1_handler() { uart_handler(1); }
void uart2_handler() { uart_handler(2); }
void uart3_handler() { uart_handler(3); }
void uart4_handler() { uart_handler(4); }
void uart5_handler() { uart_handler(5); }
void uart6_handler() { uart_handler(6); }
void uart7_handler() { uart_handler(7); }

//------------------------------------------------------------------------------
// Write a byte to given UART
//------------------------------------------------------------------------------
static int32_t uart_write_normal(IO_io *io, const void *data, uint32_t length)
{
  uint32_t uart_offset = io->channel*UART_MODULE_OFFSET;

  const uint8_t *b_data = data;
  for(uint32_t i = 0; i < length; ++i) {
    // we cannot write if TXFF is 1
    if(io->flags & IO_NONBLOCKING) {
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
static int32_t uart_read_normal(IO_io *io, void *data, uint32_t length)
{
  uint32_t uart_offset = io->channel*UART_MODULE_OFFSET;
  uint8_t *b_data = data;
  for(uint32_t i = 0; i < length; ++i) {
    // we cannot read if RXFE is 1
    if(io->flags & IO_NONBLOCKING) {
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
int32_t IO_uart_init(IO_io *io, uint8_t module, uint16_t flags, uint32_t baud)
{
  if(module > 7)
    return -IO_EINVAL;

  if(!io)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Initialize the hardware
  //----------------------------------------------------------------------------
  uint8_t  port        = uart_info[module].gpio_port;
  uint8_t  rx          = uart_info[module].gpio_pin_rx;
  uint8_t  tx          = uart_info[module].gpio_pin_tx;
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
  io->channel = module;
  io->flags = flags;
  io->write = uart_write_normal;
  io->read = uart_read_normal;
  io->type = IO_UART;
  uart_devices[module] = io;

  //----------------------------------------------------------------------------
  // Enable the interrupt if needed
  //----------------------------------------------------------------------------
  if(flags & IO_ASYNC) {
    uint8_t nvic_bit = uart_info[module].interrupt_num % 32;
    uint8_t nvic_reg = uart_info[module].interrupt_num / 32;
    NVIC_EN_REG(nvic_reg) |= (1 << nvic_bit);
  }

  return 0;
}

//------------------------------------------------------------------------------
// Enable events on UART device
//------------------------------------------------------------------------------
int32_t TM4C_event_enable_uart(IO_io *io, uint16_t events)
{
  uint32_t uart_offset = io->channel*UART_MODULE_OFFSET;
  if(events & IO_EVENT_READ)  UART_REG(uart_offset, UART_IM) |= 0x10;
  if(events & IO_EVENT_WRITE) UART_REG(uart_offset, UART_IM) |= 0x20;
  return 0;
}

//------------------------------------------------------------------------------
// Disable events on UART device
//------------------------------------------------------------------------------
int32_t TM4C_event_disable_uart(IO_io *io, uint16_t events)
{
  uint32_t uart_offset = io->channel*UART_MODULE_OFFSET;
  if(events & IO_EVENT_READ)  UART_REG(uart_offset, UART_IM) &= ~0x10;
  if(events & IO_EVENT_WRITE) UART_REG(uart_offset, UART_IM) &= ~0x20;
  return 0;
}
