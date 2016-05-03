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

#include <io/IO.h>
#include <io/IO_display.h>
#include <io/IO_font.h>
#include <io/IO_device.h>
#include <io/IO_malloc.h>
#include <string.h>

#include "SI_scene.h"

//------------------------------------------------------------------------------
// Devices
//------------------------------------------------------------------------------
IO_io display;
IO_io scene_timer;
IO_io slider;
IO_io slider_timer;

IO_display_attrs display_attrs;

//------------------------------------------------------------------------------
// Bitmaps
//------------------------------------------------------------------------------
extern const IO_bitmap BunkerDamagedImg;
extern const IO_bitmap BunkerImg;
extern const IO_bitmap DefenderImg;
extern const IO_bitmap HeartImg;
extern const IO_bitmap Invader1Img;
extern const IO_bitmap Invader2Img;
extern const IO_bitmap Invader3Img;
extern const IO_bitmap Invader4Img;

//------------------------------------------------------------------------------
// Scenes
//------------------------------------------------------------------------------
uint8_t state = 0;
SI_scene scenes[1];
uint32_t score = 1234567890;

//------------------------------------------------------------------------------
// Hardware values
//------------------------------------------------------------------------------
uint64_t slider_value = 0;

//------------------------------------------------------------------------------
// Slider timer event
//------------------------------------------------------------------------------
void slider_timer_event(IO_io *io, uint16_t event)
{
  IO_set(&slider, 0); // request a sample
}

//------------------------------------------------------------------------------
// Slider event
//------------------------------------------------------------------------------
void slider_event(IO_io *io, uint16_t event)
{
  IO_get(&slider, &slider_value);
  IO_set(&slider_timer, 20000000); // fire in 0.04 sec
}

//------------------------------------------------------------------------------
// Game scene
//------------------------------------------------------------------------------
SI_object_bitmap  defender_obj;
SI_object_dynamic score_obj;
SI_object_bitmap  life_obj[3];
SI_object_bitmap  invader_obj[5];
SI_object_bitmap  bunker_obj[3];

void game_scene_draw_score(IO_io *display, SI_object_dynamic *obj)
{
  const IO_font *font = IO_font_get_by_name("SilkScreen8");
  IO_display_set_font(display, font);
  IO_display_cursor_goto(display, 0, 0);
  IO_print(display, "%u", score);
}

void game_scene_pre_render(SI_scene *scene)
{
  uint32_t defx = display_attrs.width - defender_obj.obj.width;
  defx *= slider_value;
  defx /= 4095;
  defender_obj.obj.x = defx;
}

void game_scene_setup(SI_scene *scene, uint8_t level)
{
  if(!scene->num_objects) {
    scene->objects = IO_malloc(13*sizeof(SI_object *));
    scene->num_objects = 13;
  }

  memset(&defender_obj, 0, sizeof(defender_obj));
  SI_object_set_bitmap(&defender_obj, &DefenderImg);
  defender_obj.obj.y = display_attrs.height - defender_obj.obj.height;
  defender_obj.obj.flags = SI_OBJECT_VISIBLE;
  scene->objects[0] = &defender_obj.obj;

  memset(&score_obj, 0, sizeof(score_obj));
  score_obj.draw = game_scene_draw_score;
  score_obj.obj.type  = SI_OBJECT_DYNAMIC;
  score_obj.obj.flags = SI_OBJECT_VISIBLE;
  scene->objects[1] = &score_obj.obj;

  memset(life_obj, 0, sizeof(life_obj));
  for(int i = 0; i < 3; ++i) {
    SI_object_set_bitmap(&life_obj[i], &HeartImg);
    life_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    life_obj[i].obj.x = display_attrs.width+1 - (i+1)*(HeartImg.width+1);
    scene->objects[i+2] = &life_obj[i].obj;
  }

  const IO_bitmap *invader_img = 0;
  switch(level) {
    case 1: invader_img = &Invader1Img; break;
    case 2: invader_img = &Invader2Img; break;
    case 3: invader_img = &Invader3Img; break;
    case 4: invader_img = &Invader4Img; break;
  }
  memset(invader_obj, 0, sizeof(invader_obj));
  for(int i = 0; i < 5; ++i) {
    SI_object_set_bitmap(&invader_obj[i], invader_img);
    invader_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    invader_obj[i].obj.y = 8;
    invader_obj[i].obj.x = i*(invader_img->width+1);
    scene->objects[i+5] = &invader_obj[i].obj;
  }

  memset(bunker_obj, 0, sizeof(bunker_obj));
  uint16_t bunker_area = display_attrs.width/3;
  uint16_t bunker_offset = (bunker_area - BunkerImg.width)/2;
  for(int i = 0; i < 3; ++i) {
    SI_object_set_bitmap(&bunker_obj[i], &BunkerImg);
    bunker_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    bunker_obj[i].obj.y = display_attrs.height - 8;
    bunker_obj[i].obj.x = i*bunker_area + bunker_offset;;
    scene->objects[i+10] = &bunker_obj[i].obj;
  }

  scene->pre_render = game_scene_pre_render;
  scene->fps   = 25;
  scene->flags = SI_SCENE_RENDER;
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  //----------------------------------------------------------------------------
  // Hardware
  //----------------------------------------------------------------------------
  IO_init();

  IO_display_init(&display, 0);
  IO_display_get_attrs(&display, &display_attrs);

  IO_timer_init(&scene_timer, 0);

  IO_timer_init(&slider_timer, 1);
  IO_slider_init(&slider, 0, IO_ASYNC);
  slider_timer.event = slider_timer_event;
  slider.event       = slider_event;
  IO_event_enable(&slider, IO_EVENT_DONE);
  IO_set(&slider, 0); // request a sample

  //----------------------------------------------------------------------------
  // Set up scenes
  //----------------------------------------------------------------------------
  memset(scenes, 0, sizeof(scenes));
  game_scene_setup(&scenes[0], 4);

  //----------------------------------------------------------------------------
  // Play the game
  //----------------------------------------------------------------------------
  while(1) {
    SI_scene_render(&scenes[state], &display, &scene_timer);
    IO_wait_for_interrupt();
  }
}
