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

#include "IO.h"

struct IO_bitmap;
typedef struct IO_bitmap IO_bitmap;

//------------------------------------------------------------------------------
//! Font description
//------------------------------------------------------------------------------
struct IO_font {
  const char *name;
  uint8_t     size;
  IO_bitmap  *glyphs[93];
};

typedef struct IO_font IO_font;

//------------------------------------------------------------------------------
//! Get font by name
//------------------------------------------------------------------------------
const IO_font *IO_font_get_by_name(const char *name);

//------------------------------------------------------------------------------
//! Get glyph for the given character
//------------------------------------------------------------------------------
const IO_bitmap *IO_font_get_glyph(const IO_font *font, char ch);
