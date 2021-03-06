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

#include <stddef.h>

//------------------------------------------------------------------------------
// Make a weak alias of the given function
//------------------------------------------------------------------------------
#define WEAK_ALIAS(FUNC, ALIAS) __typeof (FUNC) ALIAS __attribute__ ((weak, alias (#FUNC)))

//------------------------------------------------------------------------------
// Access 1d array as 2d
//------------------------------------------------------------------------------
#define ARR2D(A, X, Y, WIDTH) A[Y*WIDTH+X]

//------------------------------------------------------------------------------
// Get the container of an element
//------------------------------------------------------------------------------
#define CONTAINER_OF(TYPE, MEMBER, MEMBER_ADDR) \
  ((TYPE *) ( (char *)MEMBER_ADDR - offsetof(TYPE, MEMBER)))
