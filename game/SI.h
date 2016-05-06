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

#include "SI_scene.h"

//------------------------------------------------------------------------------
// Scenes
//------------------------------------------------------------------------------
#define SI_SCENE_INTRO 0
#define SI_SCENE_GAME  1

extern uint8_t current_scene;

//------------------------------------------------------------------------------
//! Set up the game scene
//------------------------------------------------------------------------------
void game_scene_setup(SI_scene *scene);

//------------------------------------------------------------------------------
//! Set up the intro scene
//------------------------------------------------------------------------------
void intro_scene_setup(SI_scene *scene);
