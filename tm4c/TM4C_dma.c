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

#include "TM4C.h"
#include "TM4C_dma.h"

//------------------------------------------------------------------------------
// DMA control table
//------------------------------------------------------------------------------
static dma_control dma_control_table[64] __attribute__ ((aligned (1024)));

//------------------------------------------------------------------------------
// Initialize the DMA controller
//------------------------------------------------------------------------------
void TM4C_dma_init()
{
  uint32_t ctl_addr = (uint32_t)dma_control_table;

  RCGCDMA_REG |= 0x01;
  DMACFG_REG  |= 0x01;

  // the address should already be 1024-aligned but we do it anyways
  DMACTLBASE_REG |= ((ctl_addr >> 10) << 10);
}

//------------------------------------------------------------------------------
// Get the DMA control structure for the given channel
//------------------------------------------------------------------------------
dma_control *TM4C_dma_get_control(uint8_t channel, uint8_t type)
{
  if(channel > 31 || type > DMA_TYPE_ALTERNATE)
    return 0;
  return &dma_control_table[channel+type*32];
}

//------------------------------------------------------------------------------
// Initiate the DMA transfer on the given chanel and encoding
//------------------------------------------------------------------------------
void TM4C_dma_run_transfer(uint8_t channel, uint8_t enc)
{
  uint8_t enc_reg = channel / 8;
  uint8_t enc_field = (channel % 8) * 4;
  DMA_MAP_REG(enc_reg) &= ~(0x0f << enc_field);
  DMA_MAP_REG(enc_reg) |= ((enc & 0x0f) << enc_field);
  DMAPRIOCLR_REG       |= (1 << channel); // clear the priority bit
  DMAALTCLR_REG        |= (1 << channel); // clear the alternate control bit
  DMAUSEBURSTCLR_REG   |= (1 << channel); // clear the burst bit
  DMAENASET_REG        |= (1 << channel); // run the transfer
}

//------------------------------------------------------------------------------
// Check DMA channel interrupt
//------------------------------------------------------------------------------
int TM4C_dma_check_interrupt(uint8_t channel, uint8_t enc)
{
  uint8_t enc_reg = channel / 8;
  uint8_t enc_field = (channel % 8) * 4;

  if(((DMA_MAP_REG(enc_reg) >> enc_field) & 0x0f) != enc)
    return 0;

  if(DMACHIS_REG & (1 << channel)) {
    DMACHIS_REG |= (1 << channel);
    return 1;
  }
  return 0;
}
