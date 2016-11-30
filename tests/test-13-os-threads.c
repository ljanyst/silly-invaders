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
#include <io/IO_display.h>

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
float counter1 = 0;
float counter2 = 0;
IO_sys_semaphore s1, s2, s3, s4, s5;
IO_io screen;

//------------------------------------------------------------------------------
// Writes the variables to the screen
//------------------------------------------------------------------------------
void writer()
{
  while(1) {
    IO_sys_wait(&s5);
    IO_display_clear(&screen);
    IO_print(&screen, "C #1: %f\n\r", counter1);
    IO_print(&screen, "C #2: %f\n", counter2);
    IO_sync(&screen);
  }
}

//------------------------------------------------------------------------------
// Changers
//------------------------------------------------------------------------------
void changer1()
{
  while(1) {
    IO_sys_wait(&s3);
    counter1 += 2.5;
    IO_sys_signal(&s5);
  }
}

void changer2()
{
  while(1) {
    IO_sys_wait(&s4);
    counter2 += 1.5;
    IO_sys_signal(&s5);
  }
}

//------------------------------------------------------------------------------
// Synchronizers
//------------------------------------------------------------------------------
void sync1()
{
  while(1) {
    IO_sys_sleep(3000);
    IO_sys_signal(&s1);
    IO_sys_signal(&s2);
  }
}

void sync2()
{
  while(1) {
    IO_sys_wait(&s1);
    IO_sys_sleep(1000);
    IO_sys_signal(&s3);
  }
}
void sync3()
{
  while(1) {
    IO_sys_wait(&s2);
    IO_sys_sleep(2000);
    IO_sys_signal(&s4);
  }
}

IO_sys_thread tcb[6];

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(0);
  IO_display_init(&screen, 0);
  IO_sys_semaphore_init(&s1, 0);
  IO_sys_semaphore_init(&s2, 0);
  IO_sys_semaphore_init(&s3, 0);
  IO_sys_semaphore_init(&s4, 0);
  IO_sys_semaphore_init(&s5, 0);

  IO_sys_thread_add(&tcb[0], writer,   1000, 255);
  IO_sys_thread_add(&tcb[1], changer1,  500, 255);
  IO_sys_thread_add(&tcb[2], changer2,  500, 255);
  IO_sys_thread_add(&tcb[3], sync1,     500, 255);
  IO_sys_thread_add(&tcb[4], sync2,     500, 255);
  IO_sys_thread_add(&tcb[5], sync3,     500, 255);

  IO_sys_run(1000);
}
