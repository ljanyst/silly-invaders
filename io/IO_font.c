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

#include "IO_font.h"
#include <string.h>

//------------------------------------------------------------------------------
// The fonts we autogenerate
//------------------------------------------------------------------------------
extern const IO_font DejaVuSans10;
extern const IO_font RobotoRegular12;

static const IO_font * const fonts[] = {
  &DejaVuSans10,
  &RobotoRegular12,
  0
};

//------------------------------------------------------------------------------
// Get font by name
//------------------------------------------------------------------------------
const IO_font *IO_font_get_by_name(const char *name)
{
  for(int i = 0; fonts[i]; ++i) {
    if(!strcmp(name, fonts[i]->name))
      return fonts[i];
  }
  return fonts[0];
}

//------------------------------------------------------------------------------
// Get glyph for the given character
//------------------------------------------------------------------------------
const IO_bitmap *IO_font_get_glyph(const IO_font *font, char ch)
{
  if(!font) return 0;
  if(ch < 32) return font->glyphs[3];
  if(ch < 95) return font->glyphs[ch-32];
  if(ch == 95 || ch == 96) return font->glyphs[3];
  if(ch < 127) return font->glyphs[ch-34];
  return font->glyphs[3];
}
