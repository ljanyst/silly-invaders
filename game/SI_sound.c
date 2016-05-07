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

#include "SI_sound.h"
#include "SI_hardware.h"

//------------------------------------------------------------------------------
// Tunes
//------------------------------------------------------------------------------
IO_tune *tune_shoot;
IO_tune *tune_hit;
IO_tune *tune_hit_me;

//------------------------------------------------------------------------------
// RTTTLs
//------------------------------------------------------------------------------
static const char * const rtttl_shoot = ":d=4,o=6,b=125:16g5";
static const char * const rtttl_hit = ":d=4,o=6,b=125:16d5,16d_5";
static const char * const rtttl_hit_me = ":d=4,o=6,b=125:16g5,16a_5";

//------------------------------------------------------------------------------
// Initialize the hardware
//------------------------------------------------------------------------------
void SI_sound_init()
{
  tune_shoot     = IO_sound_decode_RTTTL(rtttl_shoot);
  tune_hit       = IO_sound_decode_RTTTL(rtttl_hit);
  tune_hit_me    = IO_sound_decode_RTTTL(rtttl_hit_me);
}
