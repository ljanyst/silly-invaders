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

#include "SI_scene.h"
#include "SI_scenes.h"
#include "SI_hardware.h"

#include <string.h>

//------------------------------------------------------------------------------
// Scenes
//------------------------------------------------------------------------------
uint8_t state = 0;
SI_scene scenes[1];

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  SI_hardware_init();
  memset(scenes, 0, sizeof(scenes));
  game_scene_setup(&scenes[0]);

  while(1) {
    SI_scene_render(&scenes[state], &display, &scene_timer);
    IO_wait_for_interrupt();
  }
}
