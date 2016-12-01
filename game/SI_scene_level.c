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

#include "SI.h"
#include "SI_hardware.h"

#include <io/IO.h>
#include <io/IO_display.h>
#include <io/IO_font.h>
#include <io/IO_utils.h>
#include <io/IO_malloc.h>

#include <string.h>

//------------------------------------------------------------------------------
// Intro objects
//------------------------------------------------------------------------------
static SI_object_text level_obj;
static uint8_t level = 1;
static uint8_t secs  = 1;
static char level_text[] = {'L', 'e', 'v', 'e', 'l', ' ', '1', 0};

//------------------------------------------------------------------------------
//! Set level for the game scene
//------------------------------------------------------------------------------
void level_scene_set_level(uint8_t lvl)
{
  level = lvl;
}

//------------------------------------------------------------------------------
// Compute new positions of objects
//------------------------------------------------------------------------------
static void level_scene_pre_render(SI_scene *scene)
{
  if(!secs) {
    game_scene_set_level(level);
    set_active_scene(SI_SCENE_GAME);
  }
  --secs;
}

//------------------------------------------------------------------------------
// Set up the level scene
//------------------------------------------------------------------------------
void level_scene_setup(SI_scene *scene)
{
  memset(scene, 0, sizeof(SI_scene));
  if(!scene->num_objects) {
    scene->objects = IO_malloc(sizeof(SI_object *));
    scene->num_objects = 1;
  }


  level_text[6] = '0' + level;

  memset(&level_obj, 0, sizeof(level_obj));
  const IO_font *font = IO_font_get_by_name("DejaVuSerif10");
  SI_object_text_cons(&level_obj, font, level_text);

  level_obj.obj.y = (display_attrs.height - level_obj.obj.height)/2;
  level_obj.obj.x = (display_attrs.width - level_obj.obj.width)/2;
  level_obj.obj.flags = SI_OBJECT_VISIBLE;
  scene->objects[0] = &level_obj.obj;

  secs = 1;
  scene->pre_render = level_scene_pre_render;
  scene->fps   = 1;
}
