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
#include <io/IO_sys.h>

//------------------------------------------------------------------------------
// Threads
//------------------------------------------------------------------------------
void thread0() { for(float i = 0; i < 1*8000000; i += 1.2); }
void thread1() { for(int i = 0; i < 2*8000000; ++i); }
void thread2() { for(float i = 0; i < 1*8000000; i += 1.1); }
void thread3() { for(int i = 0; i < 4*8000000; ++i); }
void thread4() { for(float i = 0; i < 1*8000000; i += 1.3); }
IO_sys_thread tcb[5];

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(0);

  IO_sys_thread_add(&tcb[0], thread0, 500, 255);
  IO_sys_thread_add(&tcb[1], thread1, 500, 255);
  IO_sys_thread_add(&tcb[2], thread2, 500, 255);
  IO_sys_thread_add(&tcb[3], thread3, 500, 255);
  IO_sys_thread_add(&tcb[4], thread4, 500, 255);

  IO_sys_run(1000);
}
