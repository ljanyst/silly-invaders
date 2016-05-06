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

#include <io/IO_display.h>
#include <io/IO_font.h>
#include <io/IO_utils.h>

#include "SI_scene.h"

//------------------------------------------------------------------------------
// Bitmap
//------------------------------------------------------------------------------
void SI_object_bitmap_draw(SI_object *obj, IO_io *display)
{
  SI_object_bitmap *this = CONTAINER_OF(SI_object_bitmap, obj, obj);
  IO_display_print_bitmap(display, obj->x, obj->y, this->bmp);
}

void SI_object_bitmap_cons(SI_object_bitmap *obj, const IO_bitmap *bmp)
{
  obj->bmp        = bmp;
  obj->obj.width  = bmp->width;
  obj->obj.height = bmp->height;
  obj->obj.draw   = SI_object_bitmap_draw;
}

//------------------------------------------------------------------------------
// Text
//------------------------------------------------------------------------------
void SI_object_text_draw(SI_object *obj, IO_io *display)
{
  SI_object_text *this = CONTAINER_OF(SI_object_text, obj, obj);
  IO_display_set_font(display, this->font);
  IO_display_cursor_goto(display, obj->x, obj->y);
  IO_print(display, "%s", this->text);
}

void SI_object_text_cons(SI_object_text *obj, const IO_font *font,
  const char *text)
{
  obj->text = text;
  obj->font = font;
  IO_font_get_box(font, text, &obj->obj.width, &obj->obj.height);
  obj->obj.draw = SI_object_text_draw;
}


//------------------------------------------------------------------------------
// This should be a map from a timer to a scene, but we're lazy
//------------------------------------------------------------------------------
static SI_scene *current_scene;

//------------------------------------------------------------------------------
// Scene timer event
//------------------------------------------------------------------------------
void scene_timer_event(IO_io *io, uint16_t event)
{
  current_scene->flags = SI_SCENE_RENDER;
}

//------------------------------------------------------------------------------
// Rendere scene on the display
//------------------------------------------------------------------------------
void SI_scene_render(SI_scene *scene, IO_io *display, IO_io *timer)
{
  if(!scene || !display || !timer || scene->flags == SI_SCENE_IGNORE)
    return;
  current_scene = scene;

  if(scene->pre_render)
    scene->pre_render(scene);

  IO_display_clear(display);
  for(int i = 0; i < scene->num_objects; ++i) {
    SI_object *obj = scene->objects[i];
    if(!(obj->flags & SI_OBJECT_VISIBLE))
      continue;

    obj->draw(obj, display);
  }
  IO_sync(display);

  if(scene->collision) {
    for(int i = 0; i < scene->num_objects; ++i) {
      SI_object *obj1 = scene->objects[i];
      if(!(obj1->flags & SI_OBJECT_VISIBLE) ||
         !(obj1->flags & SI_OBJECT_TRACKABLE))
        continue;

      for(int j = 0; j < scene->num_objects; ++j) {
        SI_object *obj2 = scene->objects[j];
        if(obj1 == obj2 || !(obj2->flags & SI_OBJECT_VISIBLE))
          continue;

        if(obj1->x < obj2->x + obj2->width &&
           obj1->x + obj1->width > obj2->x &&
           obj1->y < obj2->y + obj2->height &&
           obj1->height + obj1->y > obj2->y) {
           scene->collision(obj1, obj2);
        }
      }
    }
  }

  scene->flags = SI_SCENE_IGNORE;
  timer->event = scene_timer_event;
  IO_set(timer, 1000000000/scene->fps);
}
