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

#include <io/IO_display.h>
#include <io/IO_sound.h>

//------------------------------------------------------------------------------
// Devices
//------------------------------------------------------------------------------
extern IO_io display;
extern IO_io sound;
extern IO_sound_player sound_player;
extern IO_io led;

//------------------------------------------------------------------------------
// Hardware values
//------------------------------------------------------------------------------
extern uint64_t         slider_value;
extern uint64_t         button_value;
extern IO_display_attrs display_attrs;

//------------------------------------------------------------------------------
//! Initialize the hardware
//------------------------------------------------------------------------------
void SI_hardware_init();
