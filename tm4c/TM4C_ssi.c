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

#include <io/IO_ssi.h>
#include <io/IO_error.h>
#include "TM4C.h"
#include "TM4C_dma.h"
#include "TM4C_gpio.h"

//------------------------------------------------------------------------------
// Hardware info for SSI
//------------------------------------------------------------------------------
struct ssi_data {
  uint8_t gpio_port;
  uint8_t gpio_pin_clk;
  uint8_t gpio_pin_fss;
  uint8_t gpio_pin_rx;
  uint8_t gpio_pin_tx;
  uint8_t gpio_pin_afsel;
  uint8_t interrupt_num;
  uint8_t dma_channel_rx;
  uint8_t dma_channel_tx;
  uint8_t dma_channel_enc;
};

static const struct ssi_data ssi_info[] = {
  {GPIO_PORTA_NUM, GPIO_PIN2_NUM, GPIO_PIN3_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM, 2,  7, 10, 11, 0},
  {GPIO_PORTF_NUM, GPIO_PIN2_NUM, GPIO_PIN3_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM, 2, 34, 10, 11, 1},
  {GPIO_PORTB_NUM, GPIO_PIN4_NUM, GPIO_PIN5_NUM, GPIO_PIN6_NUM, GPIO_PIN7_NUM, 2, 57, 12, 13, 2},
  {GPIO_PORTD_NUM, GPIO_PIN0_NUM, GPIO_PIN1_NUM, GPIO_PIN2_NUM, GPIO_PIN3_NUM, 1, 58, 14, 15, 2},
};

//------------------------------------------------------------------------------
// IO devices
//------------------------------------------------------------------------------
static struct IO_io *ssi_devices[4];

//------------------------------------------------------------------------------
// Handle uart interrupt
//------------------------------------------------------------------------------
static void ssi_handler(uint8_t module)
{
  uint16_t events = 0;
  uint32_t module_offset = module * SSI_MODULE_OFFSET;

  if(SSI_REG(module_offset, SSI_MIS) & 0x04) events |= IO_EVENT_READ;
  if(SSI_REG(module_offset, SSI_MIS) & 0x08) events |= IO_EVENT_WRITE;

  //----------------------------------------------------------------------------
  // No known events but we have still been called. Check if we got a DMA
  // interrupt
  //----------------------------------------------------------------------------
  if(!events) {
    uint8_t channel_rx = ssi_info[module].dma_channel_rx;
    uint8_t channel_tx = ssi_info[module].dma_channel_tx;
    uint8_t enc = ssi_info[module].dma_channel_enc;

    if(TM4C_dma_check_interrupt(channel_rx, enc))
      events |= IO_EVENT_DMA_READ;

    if(TM4C_dma_check_interrupt(channel_tx, enc))
      events |= IO_EVENT_DMA_WRITE;
  }

  //----------------------------------------------------------------------------
  // Call the user handler
  //----------------------------------------------------------------------------
  if(ssi_devices[module] && ssi_devices[module]->event)
    ssi_devices[module]->event(ssi_devices[module], events);
}

//------------------------------------------------------------------------------
// Interrupt handlers
//------------------------------------------------------------------------------
void ssi0_handler() { ssi_handler(0); }
void ssi1_handler() { ssi_handler(1); }
void ssi2_handler() { ssi_handler(2); }
void ssi3_handler() { ssi_handler(3); }

//------------------------------------------------------------------------------
// Write to given SSI
//------------------------------------------------------------------------------
static int32_t ssi_write_normal(IO_io *io, const void *data, uint32_t length)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  uint8_t byte = 1;
  if((SSI_REG(ssi_offset, SSI_CR0) & 0x0f) > 7)
    byte = 0;
  const uint8_t  *b_data8  = data;
  const uint16_t *b_data16 = data;

  for(uint32_t i = 0; i < length; ++i) {
    // we cannot write if TNF is 0
    if(io->flags & IO_NONBLOCKING) {
      if((SSI_REG(ssi_offset, SSI_SR) & 0x02) == 0) {
        if(i == 0) return -IO_EWOULDBLOCK;
        else return i;
      }
    }
    else
      while((SSI_REG(ssi_offset, SSI_SR) & 0x02) == 0);

    if(byte)
      SSI_REG(ssi_offset, SSI_DR) = b_data8[i];
    else
      SSI_REG(ssi_offset, SSI_DR) = b_data16[i];
  }
  return length;
}

//------------------------------------------------------------------------------
// Read from given SSI
//------------------------------------------------------------------------------
static int32_t ssi_read_normal(IO_io *io, void *data, uint32_t length)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  uint8_t byte = 1;
  if((SSI_REG(ssi_offset, SSI_CR0) & 0x0f) > 7)
    byte = 0;
  uint8_t  *b_data8  = data;
  uint16_t *b_data16 = data;

  for(uint32_t i = 0; i < length; ++i) {
    // we cannot read if RNE is 0
    if(io->flags & IO_NONBLOCKING) {
      if((SSI_REG(ssi_offset, SSI_SR) & 0x04) == 0) {
        if(i == 0) return -IO_EWOULDBLOCK;
        else return i;
      }
    }
    else
      while((SSI_REG(ssi_offset, SSI_SR) & 0x04) == 0);

    if(byte)
      b_data8[i] = SSI_REG(ssi_offset, SSI_DR) & 0xff;
    else
      b_data16[i] = SSI_REG(ssi_offset, SSI_DR) & 0xffff;
  }
  return length;
}

//------------------------------------------------------------------------------
// Transfer data to SSI using DMA
//------------------------------------------------------------------------------
static int32_t ssi_write_dma(IO_io *io, const void *data, uint32_t length)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  uint8_t dma_channel = ssi_info[io->channel].dma_channel_tx;
  uint8_t dma_enc = ssi_info[io->channel].dma_channel_enc;
  dma_control *ctrl = TM4C_dma_get_control(dma_channel, DMA_TYPE_PRIMARY);

  //----------------------------------------------------------------------------
  // Check whether the DMA channel is ready for a transfer
  //----------------------------------------------------------------------------
  if(io->flags & IO_NONBLOCKING) {
    if(ctrl->control & 0x03)
      return -IO_EWOULDBLOCK;
  }
  else
    while(ctrl->control & 0x03);

  //----------------------------------------------------------------------------
  // Set the channel control up
  //----------------------------------------------------------------------------
  if(length == 0)
    return 0;

  if(length > 1024)
    length = 1024;

  uint8_t byte = 1;
  if((SSI_REG(ssi_offset, SSI_CR0) & 0x0f) > 7)
    byte = 0;

  if(byte)
    ctrl->src = ((uint8_t *)data)+(length-1); // last item of the buffer
  else {
    ctrl->src = ((uint16_t *)data)+(length-1); // last item of the buffer
    ctrl->control |= (0x01 << 28);             // destination item size is
                                               // a half-word
    ctrl->control |= (0x01 << 26);             // source increments by a
                                               // half-word
    ctrl->control |= (0x01 << 24);             // source item data size is
                                               // a half-word
  }

  ctrl->dst = (void *)&SSI_REG(ssi_offset, SSI_DR);
  ctrl->control = 0;
  ctrl->control |= (0x03 << 30);      // destination does not increment
  ctrl->control |= (0x02 << 14);      // arbitration size is 4 transfers ==
                                      // interrupt trigger level for FIFO
  ctrl->control |= ((length-1) << 4); // transfer size, up to 1024 items
  ctrl->control |= 0x01;              // basic transfer mode

  TM4C_dma_run_transfer(dma_channel, dma_enc);
  return length;
}

//------------------------------------------------------------------------------
// Get data from SSI using dma
//------------------------------------------------------------------------------
static int32_t ssi_read_dma(IO_io *io, void *data, uint32_t length)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  uint8_t dma_channel = ssi_info[io->channel].dma_channel_rx;
  uint8_t dma_enc = ssi_info[io->channel].dma_channel_enc;
  dma_control *ctrl = TM4C_dma_get_control(dma_channel, DMA_TYPE_PRIMARY);

  //----------------------------------------------------------------------------
  // Check whether the DMA channel is ready for a transfer
  //----------------------------------------------------------------------------
  if(io->flags & IO_NONBLOCKING) {
    if(ctrl->control & 0x03)
      return -IO_EWOULDBLOCK;
  }
  else
    while(ctrl->control & 0x03);

  //----------------------------------------------------------------------------
  // Set the channel control up
  //----------------------------------------------------------------------------
  if(length == 0)
    return 0;

  if(length > 1024)
    length = 1024;

  uint8_t byte = 1;
  if((SSI_REG(ssi_offset, SSI_CR0) & 0x0f) > 7)
    byte = 0;

  ctrl->control = 0;
  ctrl->src = (void *)&SSI_REG(ssi_offset, SSI_DR);
  if(byte)
    ctrl->dst = ((uint8_t *)data)+(length-1); // last byte of the buffer
  else {
    ctrl->dst = ((uint16_t *)data)+(length-1); // last item of the buffer
    ctrl->control |= (0x01 << 30);             // destination increments by a
                                               // half-word
    ctrl->control |= (0x01 << 28);             // destination item size is
                                               // a half-word
    ctrl->control |= (0x01 << 24);             // source item data size is
                                               // a half-word
  }
  ctrl->control |= (0x03 << 26);      // source does not increment
  ctrl->control |= (0x02 << 14);      // arbitration size is 4 transfers ==
                                      // interrupt trigger level for FIFO
  ctrl->control |= ((length-1) << 4); // transfer size, up to 1024 items
  ctrl->control |= 0x01;              // basic transfer mode

  TM4C_dma_run_transfer(dma_channel, dma_enc);
  return length;
}

//------------------------------------------------------------------------------
// Sync ssi
//------------------------------------------------------------------------------
static int32_t ssi_sync(IO_io *io)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  // the busy bit is set untill the last byte from the TX FIFO has been
  // transmitted
  while(SSI_REG(ssi_offset, SSI_SR) & 0x10);
  return 0;
}

//------------------------------------------------------------------------------
// Initialize given SSI module
//------------------------------------------------------------------------------
int32_t IO_ssi_init(IO_io *io, uint8_t module, uint16_t flags,
  IO_ssi_attrs *attrs)
{
  if(module > 7)
    return -IO_EINVAL;

  if(!io)
    return -IO_EINVAL;

  if(attrs->master && (attrs->bandwidth < 1231 || attrs->bandwidth > 40000000))
    return -IO_EINVAL;

  if(attrs->frame_format < 0 || attrs->frame_format > 3)
    return -IO_EINVAL;

  if(attrs->frame_size < 4 || attrs->frame_size > 16)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Initialize the hardware
  //----------------------------------------------------------------------------
  uint8_t  port       = ssi_info[module].gpio_port;
  uint8_t  clk        = ssi_info[module].gpio_pin_clk;
  uint8_t  fss        = ssi_info[module].gpio_pin_fss;
  uint8_t  rx         = ssi_info[module].gpio_pin_rx;
  uint8_t  tx         = ssi_info[module].gpio_pin_tx;
  uint8_t  afsel      = ssi_info[module].gpio_pin_afsel;
  uint16_t ssi_offset = module * SSI_MODULE_OFFSET;

  // enable the ssi clock
  RCGCSSI_REG |= (1 << module);

  //----------------------------------------------------------------------------
  // Set the GPIO up
  //----------------------------------------------------------------------------
  TM4C_gpio_port_init(port);

  if(module == 1)
    TM4C_gpio_pin_unlock(port, rx);

  uint8_t out = 1;
  if(!attrs->master)
    out = 0;

  TM4C_gpio_pin_init(port, clk, afsel, 0, out);
  TM4C_gpio_pin_init(port, fss, afsel, 0, out);
  TM4C_gpio_pin_init(port, rx,  afsel, 0, 0);
  TM4C_gpio_pin_init(port, tx,  afsel, 0, 1);

  //----------------------------------------------------------------------------
  // Configure SSI
  //----------------------------------------------------------------------------

  // calculate the desired clock parameters
  uint32_t clock = 80000000/attrs->bandwidth;
  uint32_t prescale = 2;
  for(; clock/prescale > 256; prescale *= 2);
  uint32_t scr = clock/prescale - 1;

  // disable ssi
  SSI_REG(ssi_offset, SSI_CR1) &= ~0x02;

  // master/slave?
  if(attrs->master) {
    SSI_REG(ssi_offset, SSI_CR1) &= ~0x04;
    // clock
    SSI_REG(ssi_offset, SSI_CPSR)  = prescale & 0xff;
    SSI_REG(ssi_offset, SSI_CR0)  &= ~(0xff << 8);
    SSI_REG(ssi_offset, SSI_CR0)  |= ((scr & 0xff) << 8);
  }
  else {
    SSI_REG(ssi_offset, SSI_CR1) |= 0x04;

    // slave output
    if(attrs->slave_out)
      SSI_REG(ssi_offset, SSI_CR1) &= ~0x08;
    else
      SSI_REG(ssi_offset, SSI_CR1) |= 0x08;
  }

  // system clock as the clock source
  SSI_REG(ssi_offset, SSI_CC) &= ~0x0f;

  // frame format
  uint8_t format;
  if(attrs->frame_format == SSI_FRAME_FREESCALE)      format = 0;
  else if(attrs->frame_format == SSI_FRAME_TEXAS)     format = 1;
  else if(attrs->frame_format == SSI_FRAME_MICROWIRE) format = 2;
  SSI_REG(ssi_offset, SSI_CR0) &= ~(0x3 << 4);
  SSI_REG(ssi_offset, SSI_CR0) |= (format << 4);

  if(attrs->frame_format == SSI_FRAME_FREESCALE) {
    if(attrs->freescale_spo)
      SSI_REG(ssi_offset, SSI_CR0) |= (1 << 6);
    else
      SSI_REG(ssi_offset, SSI_CR0) &= ~(1 << 6);
    if(attrs->freescale_sph)
      SSI_REG(ssi_offset, SSI_CR0) |= (1 << 7);
    else
      SSI_REG(ssi_offset, SSI_CR0) &= ~(1 << 7);
  }

  // frame size
  SSI_REG(ssi_offset, SSI_CR0) &= ~0x0f;
  SSI_REG(ssi_offset, SSI_CR0) |= ((attrs->frame_size-1) & 0x0f);

  if(flags & IO_DMA)
    SSI_REG(ssi_offset, SSI_DMACTL) |= 0x03;

  // remove the garbage from the incoming queue
  uint8_t byte;
  while(SSI_REG(ssi_offset, SSI_SR) & 0x04)
    byte = SSI_REG(ssi_offset, SSI_DR);
  (void)byte;

  // enable ssi
  SSI_REG(ssi_offset, SSI_CR1) |= 0x02;

  //----------------------------------------------------------------------------
  // Initialize the software
  //----------------------------------------------------------------------------
  io->channel = module;
  io->flags = flags;
  io->type = IO_SSI;
  io->event = 0;
  io->sync = ssi_sync;
  ssi_devices[module] = io;

  //----------------------------------------------------------------------------
  // Normal read/write
  //----------------------------------------------------------------------------
  if(!(flags & IO_DMA)) {
    io->write = ssi_write_normal;
    io->read = ssi_read_normal;
  }
  else {
    io->write = ssi_write_dma;
    io->read = ssi_read_dma;
  }

  //----------------------------------------------------------------------------
  // Enable the interrupt if needed
  //----------------------------------------------------------------------------
  if(flags & IO_ASYNC) {
    uint8_t nvic_bit = ssi_info[module].interrupt_num % 32;
    uint8_t nvic_reg = ssi_info[module].interrupt_num / 32;
    NVIC_EN_REG(nvic_reg) |= (1 << nvic_bit);
  }
  return 0;
}

//------------------------------------------------------------------------------
// Enable events on SSI device
//------------------------------------------------------------------------------
int32_t TM4C_ssi_event_enable(IO_io *io, uint16_t events)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  if(events & IO_EVENT_READ)  SSI_REG(ssi_offset, SSI_IM) |= 0x04;
  if(events & IO_EVENT_WRITE) SSI_REG(ssi_offset, SSI_IM) |= 0x08;
  return 0;
}

//------------------------------------------------------------------------------
// Disable events on SSI device
//------------------------------------------------------------------------------
int32_t TM4C_ssi_event_disable(IO_io *io, uint16_t events)
{
  uint32_t ssi_offset = io->channel*SSI_MODULE_OFFSET;
  if(events & IO_EVENT_READ)  SSI_REG(ssi_offset, SSI_IM) &= ~0x08;
  if(events & IO_EVENT_WRITE) SSI_REG(ssi_offset, SSI_IM) &= ~0x04;
  return 0;
}
