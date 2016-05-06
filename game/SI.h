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
#define SI_SCENE_LEVEL 1
#define SI_SCENE_GAME  2
#define SI_SCENE_SCORE 3

//------------------------------------------------------------------------------
//! Set active scene
//------------------------------------------------------------------------------
void set_active_scene(uint8_t scene);

//------------------------------------------------------------------------------
//! Set level for the game scene
//------------------------------------------------------------------------------
void game_scene_set_level(uint8_t level);

//------------------------------------------------------------------------------
//! Set up the game scene
//------------------------------------------------------------------------------
void game_scene_setup(SI_scene *scene);

//------------------------------------------------------------------------------
//! Set up the intro scene
//------------------------------------------------------------------------------
void intro_scene_setup(SI_scene *scene);

//------------------------------------------------------------------------------
//! Set up the level scene
//------------------------------------------------------------------------------
void level_scene_setup(SI_scene *scene);

//------------------------------------------------------------------------------
//! Set level for the score scene
//------------------------------------------------------------------------------
void level_scene_set_level(uint8_t level);

//------------------------------------------------------------------------------
//! Set up the score scene
//------------------------------------------------------------------------------
void score_scene_setup(SI_scene *scene);

//------------------------------------------------------------------------------
//! Set score for the score scene
//------------------------------------------------------------------------------
void score_scene_set_score(uint32_t score);
