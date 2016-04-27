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

#include "IO.h"

//------------------------------------------------------------------------------
//! Initialize a sound device
//!
//! @param io     the io structure to be initialized
//! @param module number of a sound device to be configured
//------------------------------------------------------------------------------
int32_t IO_sound_init(IO_io *io, uint8_t module);

//------------------------------------------------------------------------------
// Player helpers
//------------------------------------------------------------------------------
struct IO_note {
  uint16_t frequency;  //! frequency in Hz
  uint16_t duration;   //! duration in miliseconds
};

typedef struct IO_note IO_note;

struct IO_tune {
  IO_note note[32];      //! 32 notes, a note of 0 duration ends the tune
  struct IO_tune *next;  //! pointer to the next tune
};

typedef struct IO_tune IO_tune;

//------------------------------------------------------------------------------
//! Play a tune
//!
//! @param io    IO device to play on
//! @param timer timer to use
//! @param tune  tune to play
//! @param start starting note
//------------------------------------------------------------------------------
int32_t IO_sound_play(IO_io *io, IO_io *timer, IO_tune *tune, uint16_t start);

//------------------------------------------------------------------------------
//! Stop current tune
//!
//! @return -errno on error, position in the tune on success
//------------------------------------------------------------------------------
int32_t IO_sound_stop(IO_io *io);

//------------------------------------------------------------------------------
//! Decode an RTTTL tune
//------------------------------------------------------------------------------
IO_tune *IO_sound_decode_RTTTL(const char *tune);

//------------------------------------------------------------------------------
//! Free an IO_tune
//------------------------------------------------------------------------------
void IO_sound_free_tune(IO_tune *tune);
