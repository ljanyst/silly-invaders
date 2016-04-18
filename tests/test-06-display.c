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
#include <io/IO_malloc.h>
#include <io/IO_font.h>
#include <string.h>

//------------------------------------------------------------------------------
// Print array
//------------------------------------------------------------------------------
void print_array(IO_io *display, uint8_t **arr)
{
  IO_display_clear(display);
  for(int i = 0; i < 84; ++i)
    for(int j = 0; j < 48; ++j)
      IO_display_put_pixel(display, i, j, !arr[i][j]);
  IO_sync(display);
}

//------------------------------------------------------------------------------
// Count neighbors
//------------------------------------------------------------------------------
uint8_t count_neighbors(uint8_t **arr, int i, int j)
{
  int startx = i > 0 ? i-1 : 0;
  int starty = j > 0 ? j-1 : 0;
  int stopx  = i < 83 ? i+1 : 83;
  int stopy  = j < 47 ? j+1 : 47;
  uint8_t ct = 0;

  for(int x = startx; x <= stopx; ++x)
    for(int y = starty; y <= stopy; ++y)
      if(arr[x][y]) ++ct;

  if(arr[i][j]) --ct;
  return ct;
}

//------------------------------------------------------------------------------
// Calculate the next epoch of life
//------------------------------------------------------------------------------
void calc_epoch(uint8_t **old, uint8_t **new)
{
  for(int i = 0; i < 84; ++i)
    for(int j = 0; j < 48; ++j) {
      uint8_t nbs = count_neighbors(old, i, j);
      if(old[i][j]) {
        if(nbs < 2) new[i][j] = 0;
        else if(nbs <= 3) new[i][j] = 1;
        else new[i][j] = 0;
      }
      else if(nbs == 3)
        new[i][j] = 1;
      else
        new[i][j] = 0;
    }
}

//------------------------------------------------------------------------------
// Allocate a 2d array
//------------------------------------------------------------------------------
uint8_t **alloc_array(uint16_t width, uint16_t height)
{
  uint8_t **arr;
  arr = IO_malloc(width*sizeof(uint8_t *));
  if(!arr) return 0;
  for(int i = 0; i < width; ++i) {
    arr[i] = IO_malloc(height*sizeof(uint8_t));
    if(!arr[i]) {
      for(int j = 0; j < i; ++j)
        IO_free(arr[j]);
      IO_free(arr);
      return 0;
    }
    memset(arr[i], 0, height);
  }
  return arr;
}

//------------------------------------------------------------------------------
// Play life
//------------------------------------------------------------------------------
void play_life(IO_io *display, uint8_t **arr)
{
  IO_display_attrs attrs;
  IO_display_get_attrs(display, &attrs);

  uint8_t **old = arr;
  uint8_t **new = alloc_array(attrs.width, attrs.height);
  uint8_t **tmp = 0;
  while(1) {
    print_array(display, old);
    calc_epoch(old, new);
    tmp = new;
    new = old;
    old = tmp;
  }
}

//------------------------------------------------------------------------------
// Initialize the glider gun
//------------------------------------------------------------------------------
void glider_gun(IO_io *display)
{
  IO_display_attrs attrs;
  IO_display_get_attrs(display, &attrs);
  if(attrs.width < 37 || attrs.height < 10)
    return;

  //----------------------------------------------------------------------------
  // Initialize the Gosper Glider Gun
  //----------------------------------------------------------------------------
  uint8_t **arr = alloc_array(attrs.width, attrs.height);

  arr = IO_malloc(attrs.width*sizeof(uint8_t *));
  if(!arr) return;
  for(int i = 0; i < attrs.width; ++i) {
    arr[i] = IO_malloc(attrs.height*sizeof(uint8_t));
    if(!arr[i]) {
      for(int j = 0; j < i; ++j)
        IO_free(arr[j]);
      IO_free(arr);
      return;
    }
    memset(arr[i], 0, attrs.height);
  }

  arr[1][5] = 1; arr[2][5] = 1;
  arr[1][6] = 1; arr[2][6] = 1;

  arr[13][3] = 1; arr[14][3] = 1;
  arr[12][4] = 1;
  arr[11][5] = 1; arr[11][6] = 1; arr[11][7] = 1;
  arr[12][8] = 1;
  arr[13][9] = 1; arr[14][9] = 1;

  arr[15][6] = 1;

  arr[16][4] = 1;
  arr[16][8] = 1;
  arr[17][5] = 1; arr[17][6] = 1; arr[17][7] = 1;
  arr[18][6] = 1;

  arr[21][5] = 1; arr[21][4] = 1; arr[21][3] = 1;
  arr[22][5] = 1; arr[22][4] = 1; arr[22][3] = 1;
  arr[23][6] = 1; arr[23][2] = 1;

  arr[25][1] = 1; arr[25][2] = 1;
  arr[25][6] = 1; arr[25][7] = 1;

  arr[35][3] = 1; arr[36][3] = 1;
  arr[35][4] = 1; arr[36][4] = 1;

  //----------------------------------------------------------------------------
  // Play the game
  //----------------------------------------------------------------------------
  play_life(display, arr);
}

//------------------------------------------------------------------------------
// Print text in the center
//------------------------------------------------------------------------------
void print_centered(IO_io *display, const IO_font *font, const char *text)
{
  IO_display_set_font(display, font);

  IO_display_attrs attrs;
  IO_display_get_attrs(display, &attrs);

  uint16_t width, height;
  IO_font_get_box(font, text, &width, &height);

  uint16_t x = attrs.width/2 - width/2;
  IO_display_cursor_move(display, x, 0);

  IO_print(display, "%s", text);
}
//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_io display;
  IO_display_init(&display, 0);

  const IO_font *font1 = IO_font_get_by_name("DejaVuSans10");
  const IO_font *font2 = IO_font_get_by_name("DejaVuSerif10");

  IO_display_cursor_goto(&display, 0, 5);
  print_centered(&display, font1, "Hello World!\n\r");
  IO_display_cursor_move(&display, 0, 5);
  print_centered(&display, font2, "Lukasz Janyst");
  IO_sync(&display);

  for(int i = 0; i < 30000000; ++i);
  IO_display_clear(&display);
  IO_display_cursor_goto(&display, 0, 5);
  print_centered(&display, font1, "Conway's\n\r");
  IO_display_cursor_move(&display, 0, 5);
  print_centered(&display, font2, "Game of life");
  IO_sync(&display);

  for(int i = 0; i < 30000000; ++i);
  IO_display_clear(&display);
  IO_display_cursor_move(&display, 0, 15);
  print_centered(&display, font2, "Glider Gun!");
  IO_sync(&display);

  for(int i = 0; i < 30000000; ++i);
  glider_gun(&display);
}
