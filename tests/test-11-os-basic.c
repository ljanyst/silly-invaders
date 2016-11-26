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
void thread0() { while(1); }
void thread1() { while(1); }
void thread2() { while(1); }
void thread3() { while(1); }
void thread4() { while(1); }
IO_sys_thread tcb[5];

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(0);

  IO_sys_thread_add(&tcb[0], thread0, 255);
  IO_sys_thread_add(&tcb[1], thread1, 255);
  IO_sys_thread_add(&tcb[2], thread2, 255);
  IO_sys_thread_add(&tcb[3], thread3, 255);
  IO_sys_thread_add(&tcb[4], thread4, 255);

  IO_sys_run(1000);
}
