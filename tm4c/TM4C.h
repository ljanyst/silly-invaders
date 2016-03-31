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

//------------------------------------------------------------------------------
// Register definitions
//------------------------------------------------------------------------------
#define CPAC_REG             (*(volatile unsigned long *)0xe000ed88)

#define RIS_REG              (*(volatile unsigned long *)0x400fe050)
#define RCC_REG              (*(volatile unsigned long *)0x400fe060)
#define RCC2_REG             (*(volatile unsigned long *)0x400fe070)
