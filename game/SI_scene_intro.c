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
// Bitmaps
//------------------------------------------------------------------------------
extern const IO_bitmap Invader1Img;
extern const IO_bitmap Invader2Img;
extern const IO_bitmap Invader3Img;
extern const IO_bitmap Invader4Img;

//------------------------------------------------------------------------------
// Intro objects
//------------------------------------------------------------------------------
static SI_object_text    title_obj[2];
static SI_object_bitmap  invader_obj[4];
static SI_object_text    press_obj;

//------------------------------------------------------------------------------
// Compute new positions of objects
//------------------------------------------------------------------------------
static void intro_scene_pre_render(SI_scene *scene)
{
  if(press_obj.obj.flags & SI_OBJECT_VISIBLE)
    press_obj.obj.flags &= ~SI_OBJECT_VISIBLE;
  else
    press_obj.obj.flags |= SI_OBJECT_VISIBLE;

  if(button_value) {
    level_scene_set_level(1);
    set_active_scene(SI_SCENE_LEVEL);
  }
  button_value = 0;
}

//------------------------------------------------------------------------------
// Set up the intro
//------------------------------------------------------------------------------
void intro_scene_setup(SI_scene *scene)
{
  memset(scene, 0, sizeof(SI_scene));
  if(!scene->num_objects) {
    scene->objects = IO_malloc(7*sizeof(SI_object *));
    scene->num_objects = 7;
  }

  //----------------------------------------------------------------------------
  // Title
  //----------------------------------------------------------------------------
  const IO_font *font_title = IO_font_get_by_name("DejaVuSerif10");

  memset(&title_obj, 0, sizeof(title_obj));
  SI_object_text_cons(&title_obj[0], font_title, "Silly");
  SI_object_text_cons(&title_obj[1], font_title, "Invaders");

  uint16_t y_off = display_attrs.height - title_obj[0].obj.height - 2;
  y_off -= title_obj[1].obj.height;
  y_off /= 2;
  title_obj[0].obj.y = y_off;
  title_obj[1].obj.y = y_off + 2 + title_obj[0].obj.height;

  for(int i = 0; i < 2; ++i) {
    uint16_t x_off = (display_attrs.width - title_obj[i].obj.width)/2;
    title_obj[i].obj.x = x_off;
    title_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    scene->objects[i] = &title_obj[i].obj;
  }

  //----------------------------------------------------------------------------
  // Invaders
  //----------------------------------------------------------------------------
  memset(&invader_obj, 0, sizeof(invader_obj));
  SI_object_bitmap_cons(&invader_obj[0], &Invader1Img);
  SI_object_bitmap_cons(&invader_obj[1], &Invader2Img);
  SI_object_bitmap_cons(&invader_obj[2], &Invader3Img);
  SI_object_bitmap_cons(&invader_obj[3], &Invader4Img);

  invader_obj[0].obj.x = 1;
  invader_obj[0].obj.y = 1;
  invader_obj[1].obj.x = display_attrs.width - invader_obj[1].obj.width - 1;
  invader_obj[1].obj.y = 1;
  invader_obj[2].obj.x = 1;
  invader_obj[2].obj.y = display_attrs.height - invader_obj[2].obj.height - 1;
  invader_obj[3].obj.x = display_attrs.width - invader_obj[3].obj.width - 1;
  invader_obj[3].obj.y = display_attrs.height - invader_obj[3].obj.height - 1;

  for(int i = 0; i < 4; ++i) {
    invader_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    scene->objects[i+2] = &invader_obj[i].obj;
  }

  //----------------------------------------------------------------------------
  // Press a key
  //----------------------------------------------------------------------------
  const IO_font *font_press = IO_font_get_by_name("SilkScreen8");
  memset(&press_obj, 0, sizeof(press_obj));
  SI_object_text_cons(&press_obj, font_press, "Press a key");
  press_obj.obj.x = (display_attrs.width - press_obj.obj.width)/2;
  press_obj.obj.y = display_attrs.height - press_obj.obj.height - 1;
  press_obj.obj.flags = SI_OBJECT_VISIBLE;
  scene->objects[6] = &press_obj.obj;

  scene->pre_render = intro_scene_pre_render;
  scene->fps   = 2;
}
