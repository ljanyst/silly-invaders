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
static SI_object_text score_obj[2];
static uint32_t score = 0;
static uint8_t secs  = 3;

//------------------------------------------------------------------------------
// Convert score
//------------------------------------------------------------------------------
static IO_io   sprintf_io;
static char    score_str[32];
static uint8_t score_size = 0;

static int32_t sprintf_write(IO_io *io, const void *data, uint32_t length)
{
  const char *cdata = data;
  for(int i = 0; i < length && score_size < 31; ++i, ++score_size)
    score_str[score_size] = cdata[i];
  score_str[score_size] = 0;
  return length;
}

//------------------------------------------------------------------------------
//! Set level for the game scene
//------------------------------------------------------------------------------
void score_scene_set_score(uint32_t scor)
{
  score = scor;
}

//------------------------------------------------------------------------------
// Compute new positions of objects
//------------------------------------------------------------------------------
static void score_scene_pre_render(SI_scene *scene)
{
  if(!secs)
    set_active_scene(SI_SCENE_INTRO);
  --secs;
}

//------------------------------------------------------------------------------
// Set up the level scene
//------------------------------------------------------------------------------
void score_scene_setup(SI_scene *scene)
{
  memset(scene, 0, sizeof(SI_scene));
  if(!scene->num_objects) {
    scene->objects = IO_malloc(2*sizeof(SI_object *));
    scene->num_objects = 2;
  }

  score_size = 0;
  sprintf_io.write = sprintf_write;
  IO_print(&sprintf_io, "%lu", score);

  const IO_font *font = IO_font_get_by_name("DejaVuSerif10");
  memset(&score_obj, 0, sizeof(score_obj));
  SI_object_text_cons(&score_obj[0], font, "Your score:");
  SI_object_text_cons(&score_obj[1], font, score_str);

  uint16_t y_off = display_attrs.height - score_obj[0].obj.height - 2;
  y_off -= score_obj[1].obj.height;
  y_off /= 2;
  score_obj[0].obj.y = y_off;
  score_obj[1].obj.y = y_off + 2 + score_obj[0].obj.height;

  for(int i = 0; i < 2; ++i) {
    uint16_t x_off = (display_attrs.width - score_obj[i].obj.width)/2;
    score_obj[i].obj.x = x_off;
    score_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    scene->objects[i] = &score_obj[i].obj;
  }

  secs = 3;
  scene->pre_render = score_scene_pre_render;
  scene->fps   = 1;
  scene->flags = SI_SCENE_RENDER;
}
