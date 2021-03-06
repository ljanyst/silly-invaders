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

#pragma once

#include <stdint.h>

//------------------------------------------------------------------------------
//! DMA control structure
//------------------------------------------------------------------------------
struct dma_control {
  void *src;            //!< source address
  void *dst;            //!< destination address
  uint32_t control;     //!< control word
  uint32_t reserved;    //!< reserved
};

typedef struct dma_control dma_control;

//------------------------------------------------------------------------------
//! Initialize the DMA controller
//------------------------------------------------------------------------------
void TM4C_dma_init();

#define DMA_TYPE_PRIMARY 0
#define DMA_TYPE_ALTERNATE 1

//------------------------------------------------------------------------------
//! Get the DMA control structure for the given channel
//!
//! @param channel the channel numbe
//! @param type    DMA_TYPE_PRIMARY or DMA_TYPE_ALTERNATE
//------------------------------------------------------------------------------
dma_control *TM4C_dma_get_control(uint8_t channel, uint8_t type);

//------------------------------------------------------------------------------
//! Initiate the DMA transfer
//!
//! @param channel DMA channel
//! @param end     channel encoding
//------------------------------------------------------------------------------
void TM4C_dma_run_transfer(uint8_t channel, uint8_t enc);

//------------------------------------------------------------------------------
//! Check the interrupt status for the given channel
//!
//! @param channel DMA channel
//! @param enc     channel encoding
//------------------------------------------------------------------------------
int TM4C_dma_check_interrupt(uint8_t channel, uint8_t enc);
