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
