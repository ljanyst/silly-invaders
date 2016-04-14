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

#define __IO_IMPL__
#include "IO_malloc.h"
#include "IO_malloc_low.h"

//------------------------------------------------------------------------------
// Malloc helper structs
//------------------------------------------------------------------------------
struct IO_memchunk {
  struct IO_memchunk *next;
  uint32_t            size;
};

typedef struct IO_memchunk IO_memchunk;

static IO_memchunk *head;
#define MEMCHUNK_USED 0x40000000

//------------------------------------------------------------------------------
// Allocate memory on the heap
//------------------------------------------------------------------------------
void *IO_malloc(uint32_t size)
{
  //----------------------------------------------------------------------------
  // Allocating anything less than 8 bytes is kind of pointless, the
  // book-keeping overhead is too big. We will also align to 4 bytes.
  //----------------------------------------------------------------------------
  uint32_t alloc_size = (((size-1)>>2)<<2)+4;
  if(alloc_size < 8)
    alloc_size = 8;

  //----------------------------------------------------------------------------
  // Try to find a suitable chunk that is unused
  //----------------------------------------------------------------------------
  IO_memchunk *chunk = head;
  while(chunk) {
    if(!(chunk->size & MEMCHUNK_USED) && chunk->size >= alloc_size)
      break;
    chunk = chunk->next;
  }

  if(!chunk)
    return 0;

  //----------------------------------------------------------------------------
  // Split the chunk if it's big enough to contain one more header and at least
  // 12 more bytes
  //----------------------------------------------------------------------------
  if(chunk->size > alloc_size + sizeof(IO_memchunk) + 12)
  {
    IO_memchunk *new_chunk = (IO_memchunk *)((char *)chunk+sizeof(IO_memchunk)+alloc_size);
    new_chunk->size = chunk->size-alloc_size-sizeof(IO_memchunk);
    new_chunk->next = chunk->next;
    chunk->next = new_chunk;
    chunk->size = alloc_size;
  }

  //----------------------------------------------------------------------------
  // Mark the chunk as used and return the memory
  //----------------------------------------------------------------------------
  chunk->size |= MEMCHUNK_USED;
  return (char*)chunk+sizeof(IO_memchunk);
}

//------------------------------------------------------------------------------
// Free the memory
//------------------------------------------------------------------------------
void IO_free(void *ptr)
{
  if(!ptr)
    return;

  IO_memchunk *chunk = (IO_memchunk *)((char *)ptr-sizeof(IO_memchunk));
  chunk->size &= ~MEMCHUNK_USED;
}

//------------------------------------------------------------------------------
// Set up the heap
//------------------------------------------------------------------------------
void IO_set_up_heap(uint8_t *heap_start, uint8_t *heap_end)
{
  head = (IO_memchunk*)heap_start;
  head->next = 0;
  head->size = heap_end-heap_start-sizeof(IO_memchunk);
}
