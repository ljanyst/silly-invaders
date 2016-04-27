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

#include "IO_malloc.h"
#include "IO_sound.h"
#include "IO_error.h"
#include "IO_utils.h"

#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
// Initialize a sound device
//------------------------------------------------------------------------------
int32_t __IO_sound_init(IO_io *io, uint8_t module)
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_sound_init, IO_sound_init);

//------------------------------------------------------------------------------
// Get number of sound devices available
//------------------------------------------------------------------------------
int32_t __IO_sound_count_low()
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_sound_count_low, IO_sound_count_low);

//------------------------------------------------------------------------------
// Helper structs
//------------------------------------------------------------------------------
struct snd_player {
  IO_io    *sound_dev;
  IO_io    *timer_dev;
  IO_tune  *tune;
  uint16_t  note;
  uint16_t  note_abs;
};
typedef struct snd_player snd_player;

static snd_player *snd_players = 0;
static uint8_t snd_num_players = 0;

static snd_player *get_player_by_timer(IO_io *timer)
{
  for(int i = 0; i < snd_num_players; ++i)
    if(snd_players[i].timer_dev == timer)
      return &snd_players[i];
  return 0;
}

//------------------------------------------------------------------------------
// Timer event
//------------------------------------------------------------------------------
static void snd_player_event(IO_io *io, uint16_t event)
{
  snd_player *player = get_player_by_timer(io);
  if(!player)
    return;

  IO_tune *t = player->tune;
  if(!t || t->note[player->note].duration == 0) {
    IO_set(player->sound_dev, 0);
    if(player->sound_dev->event)
      player->sound_dev->event(player->sound_dev, IO_EVENT_DONE);
    return;
  }

  uint64_t duration  = t->note[player->note].duration;
  uint16_t frequency = t->note[player->note].frequency;
  duration *= 1000000;
  ++player->note;
  ++player->note_abs;
  if(player->note == 32) {
    player->note = 0;
    player->tune = player->tune->next;
  }

  IO_set(player->sound_dev, frequency);
  IO_set(player->timer_dev, duration);
}

//------------------------------------------------------------------------------
// Play a tune
//------------------------------------------------------------------------------
int32_t IO_sound_play(IO_io *io, IO_io *timer, IO_tune *tune, uint16_t start)
{
  if(io->type != IO_SOUND || timer->type != IO_TIMER || !tune ||
     tune->note[0].duration == 0)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Check the number of available devices and create a player for each
  //----------------------------------------------------------------------------
  if(!snd_players) {
    int32_t count = IO_sound_count_low();
    if(count < 0)
      return count;
    if(count == 0)
      return -IO_ENOSYS;
    snd_num_players = count;
    snd_players = IO_malloc(sizeof(snd_player)*count);
    if(!snd_players)
      return -IO_ENOMEM;
  }

  //----------------------------------------------------------------------------
  // Find the first segment in the tune
  //----------------------------------------------------------------------------
  uint32_t seg = start / 32;
  IO_tune *current = tune;
  for(int i = 0; i < seg && current; ++i, current = current->next);
  if(!current)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Start the playback
  //----------------------------------------------------------------------------
  snd_players[io->channel].sound_dev  = io;
  snd_players[io->channel].timer_dev  = timer;
  snd_players[io->channel].tune       = current;
  snd_players[io->channel].note       = start % 32;
  snd_players[io->channel].note_abs   = start;
  timer->event = snd_player_event;

  return IO_set(timer, 100); // start the playback in 100ns
}

//------------------------------------------------------------------------------
// Stop current tune
//------------------------------------------------------------------------------
int32_t IO_sound_stop(IO_io *io)
{
  if(io->type != IO_SOUND)
    return -IO_EINVAL;

  // disable the timer
  IO_set(snd_players[io->channel].timer_dev, 0);
  return snd_players[io->channel].note_abs;
}

//------------------------------------------------------------------------------
// Tune chunk cache
//------------------------------------------------------------------------------
struct snd_tune_chunk {
  struct snd_tune_chunk *next;
};

typedef struct snd_tune_chunk snd_tune_chunk;

static snd_tune_chunk *snd_chunks = 0;

//------------------------------------------------------------------------------
// Allocate a tune
//------------------------------------------------------------------------------
static IO_tune *alloc_tune()
{
  IO_tune *tune;
  if(snd_chunks) {
    tune = (IO_tune*)snd_chunks;
    snd_chunks = snd_chunks->next;
  }
  else
    tune = IO_malloc(sizeof(IO_tune));

  memset(tune, 0, sizeof(IO_tune));
  return tune;
}

//------------------------------------------------------------------------------
// Copy an integer swallowing as many digits from the string as possible
//------------------------------------------------------------------------------
static int copy_int(const char *a, char *buffer, int bufLen)
{
  int len = 0;
  while(*a != 0)
  {
    if(*a < '0' || *a > '9')
      break;
    if(len < bufLen-1)
      buffer[len] = *a;
    ++len;
    ++a;
  }
  if(len >= bufLen)
    buffer[bufLen-1] = 0;
  else
    buffer[len] = 0;
  return len;
}

//------------------------------------------------------------------------------
// Decode an RTTTL tune
//------------------------------------------------------------------------------
IO_tune *IO_sound_decode_RTTTL(const char *tune)
{
  if(!tune)
    return 0;

  //----------------------------------------------------------------------------
  // Ignore the title
  //----------------------------------------------------------------------------
  const char *ptr = tune;
  for( ; *ptr && *ptr != ':'; ++ptr);
  ++ptr;

  //----------------------------------------------------------------------------
  // Decode header
  //----------------------------------------------------------------------------
  unsigned int octave   = 6;
  unsigned int duration = 4;
  unsigned int tempo    = 63; // beats per minute
  unsigned int loops    = 1;

  char int_buffer[32];

  while(*ptr != 0 && *ptr != ':') {
    if((*ptr != 'd' && *ptr != 'o' && *ptr != 'b' && *ptr != 'l' && *ptr+1 != '=') || *(ptr+1) == 0)
      return 0;

    char opt = *ptr;
    ptr += 2;
    ptr += copy_int(ptr, int_buffer, 32);
    int val = atoi(int_buffer);

    if(*ptr == ',') ++ptr;
    switch(opt) {
      case 'd':
        duration = val;
        break;
      case 'o':
        octave = val;
        break;
      case 'b':
        tempo = val;
        break;
      case 'l':
        loops = val;
        break;
    }
  }

  if(*ptr == 0)
    return 0;
  ++ptr;

  //----------------------------------------------------------------------------
  // Decode the tune
  //----------------------------------------------------------------------------
  IO_tune *t_tune = alloc_tune();
  IO_tune *current_chunk = t_tune;
  uint8_t  n = 0;

  // a beat is the length of a quater-note, so here, the variable beat is
  // a length of a whole note in miliseconds
  // 4 (quater-notes) * 60(s) * 1000(ms) / beats-per-minute
  unsigned int beat = 240000/tempo;
  const char *tune_start = ptr;
  for(int i = 0; i < loops; ++i) {
    ptr = tune_start;
    while(*ptr) {
      int note          = 0;
      int note_duration = duration;
      int note_octave   = octave;

      //------------------------------------------------------------------------
      // Override duration
      //------------------------------------------------------------------------
      if(*ptr >= '0' && *ptr <= '9') {
        ptr += copy_int(ptr, int_buffer, 32);
        note_duration = atoi(int_buffer);
      }

      if(*ptr == 0) {
        IO_sound_free_tune(t_tune);
        return 0;
      }

      //------------------------------------------------------------------------
      // Check the note
      //------------------------------------------------------------------------
      if(*(ptr+1) == '#') {
        switch(*ptr) {
          case 'a':
            note = 466;
            break;
          case 'c':
            note = 554;
            break;
          case 'd':
            note = 622;
            break;
          case 'f':
            note = 740;
            break;
          case 'g':
            note = 830;
            break;
          default:
            note = 0;
        }
        ptr += 2;
      }
      else {
        switch(*ptr) {
          case 'a':
            note = 440;
            break;
          case 'b':
            note = 493;
            break;
          case 'c':
            note = 523;
            break;
          case 'd':
            note = 587;
            break;
          case 'e':
            note = 659;
            break;
          case 'f':
            note = 698;
            break;
          case 'g':
            note = 784;
            break;
          default:
            note = 0;
        }
        ++ptr;
      }

      //------------------------------------------------------------------------
      // Check extended duration
      //------------------------------------------------------------------------
      if(*ptr == '.') {
        note_duration += (note_duration/2);
        ++ptr;
      }

      //------------------------------------------------------------------------
      // Override octave
      //------------------------------------------------------------------------
      if(*ptr >= '0' && *ptr <= '9') {
        ptr += copy_int(ptr, int_buffer, 32);
        note_octave = atoi(int_buffer);
      }
      note_octave -= 4;
      for(int k = 0; k < note_octave; ++k)
        note *= 2;

      //------------------------------------------------------------------------
      // Check again extended duration
      //------------------------------------------------------------------------
      if(*ptr == '.') {
        note_duration += (note_duration/2);
        ++ptr;
      }
      if(*ptr == ',') ++ptr;

      //------------------------------------------------------------------------
      // Add the note to the tune
      //------------------------------------------------------------------------
      current_chunk->note[n].frequency = note;
      current_chunk->note[n].duration  = beat/note_duration;
      ++n;
      if(n == 32) {
        current_chunk->next = alloc_tune();
        if(!current_chunk->next) {
          IO_sound_free_tune(t_tune);
          return 0;
        }
        current_chunk = current_chunk->next;
        n = 0;
      }
    }
  }
  return t_tune;
}

//------------------------------------------------------------------------------
// Free an IO_tune
//------------------------------------------------------------------------------
void IO_sound_free_tune(IO_tune *tune)
{
  IO_tune *cur = tune;
  snd_tune_chunk *t;
  while(cur) {
    t = (snd_tune_chunk*)cur;
    cur = cur->next;
    t->next = snd_chunks;
    snd_chunks = t;
  }
}
