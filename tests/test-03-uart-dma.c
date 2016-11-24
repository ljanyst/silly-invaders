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
#include <io/IO_device.h>

char buffer[30];

//------------------------------------------------------------------------------
// UART event
//------------------------------------------------------------------------------
void uart_event(IO_io *io, uint16_t event)
{
  if(event & IO_EVENT_DMA_READ)
    IO_write(io, buffer, 30);

  if(event & IO_EVENT_DMA_WRITE)
    IO_read(io, buffer, 30);
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init(4096);
  IO_io uart0;
  IO_uart_init(&uart0, 0, IO_DMA|IO_ASYNC, 115200);
  uart0.event = uart_event;
  IO_read(&uart0, buffer, 30);
  while(1)
    IO_wait_for_interrupt();
}
