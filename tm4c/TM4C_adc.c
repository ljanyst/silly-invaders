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
// Helpers
//------------------------------------------------------------------------------
#define DEF_HELPERS(MODULE)                \
  uint16_t module_offset = 0;              \
  uint8_t  adc_sequencer = MODULE;         \
  if(MODULE > 3) {                         \
    module_offset = ADC_MODULE_OFFSET;     \
    adc_sequencer -= 4;                    \
  }


//------------------------------------------------------------------------------
// ADC config
//------------------------------------------------------------------------------
struct adc_data {
  uint8_t  gpio_port;
  uint8_t  gpio_pin;
  uint16_t mux;
  uint16_t ctl;
  uint16_t fifo;
  uint8_t  interrupt;
};

static const struct adc_data adc_info[] = {
  {GPIO_PORTE_NUM, GPIO_PIN3_NUM, ADC_SSMUX0, ADC_SSCTL0, ADC_SSFIFO0, 14},
  {GPIO_PORTE_NUM, GPIO_PIN2_NUM, ADC_SSMUX1, ADC_SSCTL1, ADC_SSFIFO1, 15},
  {GPIO_PORTE_NUM, GPIO_PIN1_NUM, ADC_SSMUX2, ADC_SSCTL2, ADC_SSFIFO2, 16},
  {GPIO_PORTE_NUM, GPIO_PIN0_NUM, ADC_SSMUX3, ADC_SSCTL3, ADC_SSFIFO3, 17},
  {GPIO_PORTD_NUM, GPIO_PIN3_NUM, ADC_SSMUX0, ADC_SSCTL0, ADC_SSFIFO0, 48},
  {GPIO_PORTD_NUM, GPIO_PIN2_NUM, ADC_SSMUX1, ADC_SSCTL1, ADC_SSFIFO1, 49},
  {GPIO_PORTD_NUM, GPIO_PIN1_NUM, ADC_SSMUX2, ADC_SSCTL2, ADC_SSFIFO2, 50},
  {GPIO_PORTD_NUM, GPIO_PIN0_NUM, ADC_SSMUX3, ADC_SSCTL3, ADC_SSFIFO3, 51}
};

static struct IO_io *adc_devices[8];

//------------------------------------------------------------------------------
// Handle interrupts
//------------------------------------------------------------------------------
static void adc_handler(uint8_t module)
{
  DEF_HELPERS(module);

  if(adc_devices[module] && adc_devices[module]->event)
    adc_devices[module]->event(adc_devices[module], IO_EVENT_DONE);
  ADC_REG(module_offset, ADC_ISC) |= (1 << adc_sequencer); // ack the interrupt
}

void adc0_seq0_handler() { adc_handler(0); }
void adc0_seq1_handler() { adc_handler(1); }
void adc0_seq2_handler() { adc_handler(2); }
void adc0_seq3_handler() { adc_handler(3); }
void adc1_seq0_handler() { adc_handler(4); }
void adc1_seq1_handler() { adc_handler(5); }
void adc1_seq2_handler() { adc_handler(6); }
void adc1_seq3_handler() { adc_handler(7); }

//------------------------------------------------------------------------------
// Set ADC state
//------------------------------------------------------------------------------
static int32_t adc_write(IO_io *io, const void *data, uint32_t length)
{
  if(length != 1 || io->flags == 0)
    return -IO_EINVAL;

  DEF_HELPERS(io->channel);

  // request sampling
  ADC_REG(module_offset, ADC_PSSI) |= (1 << adc_sequencer);

  return 1;
}

//------------------------------------------------------------------------------
// Read ADC state
//------------------------------------------------------------------------------
static int32_t adc_read(IO_io *io, void *data, uint32_t length)
{
  if(length != 1)
    return -IO_EINVAL;

  DEF_HELPERS(io->channel);

  uint16_t fifo = adc_info[io->channel].fifo;
  uint64_t *val = data;

  // blocking read requests the sampling first
  if(io->flags == 0)
    ADC_REG(module_offset, ADC_PSSI) |= (1 << adc_sequencer);

  uint8_t int_mask = (1 << adc_sequencer);

  if(io->flags & IO_NONBLOCKING) {
    if((ADC_REG(module_offset, ADC_RIS) & int_mask) == 0)
      return -IO_EWOULDBLOCK;
  }
  else
    while((ADC_REG(module_offset, ADC_RIS) & int_mask) == 0);
  *val = ADC_REG(module_offset, fifo);
  ADC_REG(module_offset, ADC_ISC) |= int_mask;
  return 1;
}

//------------------------------------------------------------------------------
// Sync ADC
//------------------------------------------------------------------------------
static int32_t adc_sync(IO_io *io)
{
  return 0;
}

//------------------------------------------------------------------------------
// Initialize an ADC
//------------------------------------------------------------------------------
int32_t IO_adc_init(IO_io *io, uint8_t module, uint16_t flags)
{
  if(module > 7 || (flags & IO_DMA))
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Set up the hardware
  //----------------------------------------------------------------------------
  DEF_HELPERS(module);
  uint16_t adc_mux       = adc_info[module].mux;
  uint16_t adc_ctl       = adc_info[module].ctl;

  // turn on the clock
  if(module < 4)
    RCGCADC_REG |= 0x01;
  else
    RCGCADC_REG |= 0x02;

  // set up GPIO
  uint8_t port = adc_info[module].gpio_port;
  uint8_t pin  = adc_info[module].gpio_pin;
  TM4C_gpio_port_init(port);
  TM4C_gpio_pin_init(port, pin, 0, 1, 0);

  // turn off the sequencer
  ADC_REG(module_offset, ADC_ACTSS) &= ~(1 << adc_sequencer);

  // select the pin
  ADC_REG(module_offset, adc_mux) = module;

  // make it the only pin in the sequence and assert the interrupt when done
  ADC_REG(module_offset, adc_ctl) = 0x06;

  if(flags & IO_ASYNC)
    TM4C_enable_interrupt(adc_info[module].interrupt, 7);

  // turn on the sequencer
  ADC_REG(module_offset, ADC_ACTSS) |= (1 << adc_sequencer);

  //----------------------------------------------------------------------------
  // Set up the software
  //----------------------------------------------------------------------------
  io->type    = IO_ADC;
  io->flags   = flags;
  io->event   = 0;
  io->sync    = adc_sync;
  io->read    = adc_read;
  io->write   = adc_write;
  adc_devices[module] = io;

  return 0;
}

//------------------------------------------------------------------------------
// Enable ADC events
//------------------------------------------------------------------------------
int32_t TM4C_adc_event_enable(IO_io *io, uint16_t events)
{
  if(events != IO_EVENT_DONE)
    return -IO_EINVAL;

  DEF_HELPERS(io->channel);

  ADC_REG(module_offset, ADC_IM) |= (1 << adc_sequencer);
  return 0;
}

//------------------------------------------------------------------------------
// Disable ADC events
//------------------------------------------------------------------------------
int32_t TM4C_adc_event_disable(IO_io *io, uint16_t events)
{
  if(events != IO_EVENT_DONE)
    return -IO_EINVAL;

  DEF_HELPERS(io->channel);

  ADC_REG(module_offset, ADC_IM) &= ~(1 << adc_sequencer);
  return 0;
}
