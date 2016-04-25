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
#include <io/IO_device.h>
#include <io/IO_error.h>
#include <string.h>

//------------------------------------------------------------------------------
// IO event notification
//------------------------------------------------------------------------------
char buffer[1024];
uint32_t cur_read;
uint32_t cur_written;
uint32_t wr_trigger;

//------------------------------------------------------------------------------
// UART event
//------------------------------------------------------------------------------
void uart_event(IO_io *io, uint16_t event)
{
  if(event & IO_EVENT_READ) {
    uint32_t ret = IO_read(io, buffer+cur_read, 1024-cur_read);
    if(ret == -IO_EWOULDBLOCK) return;
    cur_read += ret;
    wr_trigger += ret;
    if(cur_read == 1024) {
      IO_event_disable(io, IO_EVENT_READ);
      IO_event_enable(io, IO_EVENT_WRITE);
      uart_event(io, IO_EVENT_WRITE);
    }

    if(wr_trigger > 32) {
      wr_trigger = 0;
      IO_event_enable(io, IO_EVENT_WRITE);
      uart_event(io, IO_EVENT_WRITE);
    }
  }

  if(event & IO_EVENT_WRITE) {
    uint32_t ret = IO_write(io, buffer+cur_written, cur_read-cur_written);
    if(ret == -IO_EWOULDBLOCK) return;
    if(ret == 0) IO_event_disable(io, IO_EVENT_WRITE);
    cur_written += ret;
  }
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_io uart0;
  IO_uart_init(&uart0, 0, IO_NONBLOCKING|IO_ASYNC, 115200);
  uart0.event = uart_event;
  IO_event_enable(&uart0, IO_EVENT_READ);

  cur_read    = 0;
  cur_written = 0;
  wr_trigger  = 0;
  while(1)
    IO_wait_for_interrupt();
}
