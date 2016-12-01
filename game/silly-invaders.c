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
#include "SI.h"
#include "SI_hardware.h"
#include "SI_sound.h"
#include <io/IO_sys.h>

#include <string.h>

//------------------------------------------------------------------------------
// Scenes
//------------------------------------------------------------------------------
uint8_t current_scene = SI_SCENE_INTRO;

struct {
  SI_scene scene;
  void (*cons)(SI_scene *scene);
} scenes[4];

//------------------------------------------------------------------------------
// Set active scene
//------------------------------------------------------------------------------
void set_active_scene(uint8_t scene)
{
  IO_disable_interrupts();
  scenes[scene].cons(&scenes[scene].scene);
  IO_enable_interrupts();
  current_scene = scene;
}

//------------------------------------------------------------------------------
// Game thread
//------------------------------------------------------------------------------
IO_sys_thread game_thread;
void game_thread_func()
{
  while(1) {
    SI_scene_render(&scenes[current_scene].scene, &display);
    IO_sys_sleep(1000/scenes[current_scene].scene.fps);
  }
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  SI_hardware_init();
  SI_sound_init();

  memset(scenes, 0, sizeof(scenes));

  scenes[SI_SCENE_INTRO].cons = intro_scene_setup;
  scenes[SI_SCENE_LEVEL].cons = level_scene_setup;
  scenes[SI_SCENE_GAME].cons  = game_scene_setup;
  scenes[SI_SCENE_SCORE].cons = score_scene_setup;
  set_active_scene(SI_SCENE_INTRO);

  IO_sys_thread_add(&game_thread, game_thread_func, 2000, 255);
  IO_sys_run(1000);
}
