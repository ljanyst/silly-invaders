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
#include "TM4C_gpio.h"

//------------------------------------------------------------------------------
// Port interrupts
//------------------------------------------------------------------------------
static const int gpio_interrupt[6] = {0, 1, 2, 3, 4, 30};

//------------------------------------------------------------------------------
// Initialize GPIO port
//------------------------------------------------------------------------------
void TM4C_gpio_port_init(uint8_t port)
{
  // enable the AHB and GPIO clock
  GPIOHBCTL_REG |= (1 << port);
  RCGCGPIO_REG  |= (1 << port);
}

//------------------------------------------------------------------------------
// Initialize GPIO pin
//------------------------------------------------------------------------------
void TM4C_gpio_pin_init(uint8_t port, uint8_t pin, uint8_t afsel, uint8_t amsel,
  uint8_t out)
{
  uint16_t port_offset = port * GPIO_PORT_OFFSET;

  // enable alternative function of the pin
  if(afsel) {
    GPIO_REG(port_offset, GPIO_AFSEL) |= (1 << pin);

    GPIO_REG(port_offset, GPIO_PCTL) &= ~(0x0f << (pin*4));
    GPIO_REG(port_offset, GPIO_PCTL) |=  ((afsel & 0x0f) << (pin*4));
  }
  else
    GPIO_REG(port_offset, GPIO_AFSEL) &= ~(1 << pin);

  // enable the analog function of the pin
  if(amsel)
    GPIO_REG(port_offset, GPIO_AMSEL) &= ~(1 << pin);
  else
    GPIO_REG(port_offset, GPIO_AMSEL) &= ~(1 << pin);

  // set pin direction
  if(out)
    GPIO_REG(port_offset, GPIO_DIR) |= (1 << pin);
  else
    GPIO_REG(port_offset, GPIO_DIR) &= ~(1 << pin);

  // enable the pin
  GPIO_REG(port_offset, GPIO_DEN) |= (1 << pin);
}

//------------------------------------------------------------------------------
// Unlock GPIO pin
//------------------------------------------------------------------------------
void TM4C_gpio_pin_unlock(uint8_t port, uint8_t pin)
{
  uint16_t port_offset = port * GPIO_PORT_OFFSET;
  GPIO_REG(port_offset, GPIO_LOCK)  = 0x4c4f434b;
  GPIO_REG(port_offset, GPIO_CR)   |= (1 << pin);
}

static struct IO_io *gpio_devices[45];

//------------------------------------------------------------------------------
// Handle interrupts
//------------------------------------------------------------------------------
static void gpio_handler(uint8_t port)
{
  uint16_t port_offset = port * GPIO_PORT_OFFSET;
  uint8_t  pi = port * 8;
  for(int i = 0; i < 8; ++i) {
    if(GPIO_REG(port_offset, GPIO_MIS) & (1 << i)) {
      if(gpio_devices[pi+i] && gpio_devices[pi+i]->event)
        gpio_devices[pi+i]->event(gpio_devices[pi+i], IO_EVENT_CHANGE);
      GPIO_REG(port_offset, GPIO_ICR) |= (1 << i); // ack the interrupt
    }
  }
}

void gpio_porta_handler() { gpio_handler(0); }
void gpio_portb_handler() { gpio_handler(1); }
void gpio_portc_handler() { gpio_handler(2); }
void gpio_portd_handler() { gpio_handler(3); }
void gpio_porte_handler() { gpio_handler(4); }
void gpio_portf_handler() { gpio_handler(5); }

//------------------------------------------------------------------------------
// Set GPIO state
//------------------------------------------------------------------------------
static int32_t gpio_write(IO_io *io, const void *data, uint32_t length)
{
  //----------------------------------------------------------------------------
  // Extract the value
  //----------------------------------------------------------------------------
  if(length == 0)
    return -IO_EINVAL;
  const uint8_t *val = data;

  //----------------------------------------------------------------------------
  // Compute the address and set the state
  //----------------------------------------------------------------------------
  int port = (io->channel / 8) * GPIO_PORT_OFFSET;
  int pin  = io->channel % 8;

  if(*val)
    GPIO_DATA_REG(port, pin) = 0xff;
  else
    GPIO_DATA_REG(port, pin) = 0;

  return 1;
}

//------------------------------------------------------------------------------
// Read GPIO state
//------------------------------------------------------------------------------
static int32_t gpio_read(IO_io *io, void *data, uint32_t length)
{
  //----------------------------------------------------------------------------
  // Extract the value
  //----------------------------------------------------------------------------
  if(length == 0)
    return -IO_EINVAL;
  uint8_t *val = data;

  //----------------------------------------------------------------------------
  // Compute the address and get the state
  //----------------------------------------------------------------------------
  int port = (io->channel / 8) * GPIO_PORT_OFFSET;
  int pin  = io->channel % 8;

  if(GPIO_DATA_REG(port, pin))
    *val = 1;
  else
    *val = 0;

  return 1;
}

//------------------------------------------------------------------------------
// Sync gpio
//------------------------------------------------------------------------------
static int32_t gpio_sync(IO_io *io)
{
  return 0;
}


//------------------------------------------------------------------------------
// Initialize a GPIO pin
//------------------------------------------------------------------------------
int32_t IO_gpio_init(IO_io *io, uint8_t pin, uint16_t flags, uint8_t dir)
{
  if(pin > 44 || (pin > 37 && pin < 40))
    return -IO_EINVAL;

  if(flags != 0 && !(flags == IO_ASYNC && dir == 0))
    return -IO_EINVAL;

  uint8_t port = pin / 8;
  uint8_t ppin = pin % 8;

  TM4C_gpio_port_init(port);

  if((port == GPIO_PORTF_NUM && ppin == GPIO_PIN0_NUM) ||
     (port == GPIO_PORTD_NUM && ppin == GPIO_PIN7_NUM))
    TM4C_gpio_pin_unlock(port, ppin);

  TM4C_gpio_pin_init(port, ppin, 0, 0, dir);

  if(flags == IO_ASYNC) {
    uint8_t nvic_bit = gpio_interrupt[port] % 32;
    uint8_t nvic_reg = gpio_interrupt[port] / 32;
    NVIC_EN_REG(nvic_reg) |= (1 << nvic_bit);

    uint16_t port_offset = port * GPIO_PORT_OFFSET;
    GPIO_REG(port_offset, GPIO_IBE) |= (1 << ppin); // trigger both edges
  }

  io->channel = pin;
  io->type    = IO_GPIO;
  io->flags   = flags;
  io->event   = 0;
  io->read    = gpio_read;
  io->write   = gpio_write;
  io->sync    = gpio_sync;
  gpio_devices[pin] = io;
  return 0;
}

//------------------------------------------------------------------------------
// Get state of the pin
//------------------------------------------------------------------------------
int32_t IO_gpio_get_state(IO_io *io)
{
  uint8_t state = 0;
  IO_read(io, &state, 1);
  return state;
}

//------------------------------------------------------------------------------
// Set state of the pin
//------------------------------------------------------------------------------
void IO_gpio_set_state(IO_io *io, uint8_t state)
{
  IO_write(io, &state, 1);
}

//------------------------------------------------------------------------------
// Enable GPIO events
//------------------------------------------------------------------------------
int32_t TM4C_gpio_event_enable(IO_io *io, uint16_t events)
{
  if(events != IO_EVENT_CHANGE)
    return -IO_EINVAL;

  int port = (io->channel / 8) * GPIO_PORT_OFFSET;
  int pin  = io->channel % 8;

  GPIO_REG(port, GPIO_IM) |= (1 << pin);
  return 0;
}

//------------------------------------------------------------------------------
// Disable gpio events
//------------------------------------------------------------------------------
int32_t TM4C_gpio_event_disable(IO_io *io, uint16_t events)
{
  if(events != IO_EVENT_CHANGE)
    return -IO_EINVAL;

  int port = (io->channel / 8) * GPIO_PORT_OFFSET;
  int pin  = io->channel % 8;

  GPIO_REG(port, GPIO_IM) &= ~(1 << pin);

  return 0;
}
