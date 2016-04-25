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
#include <io/IO_error.h>
#include <io/IO_device.h>
#include <string.h>

#define RECEIVER
#define ITEM_SIZE 30
char buffer[1050];
uint16_t wr;
uint16_t rd;
IO_io uart0;
IO_io ssi0;
void uart_event(IO_io *io, uint16_t event);

void ssi_event(IO_io *io, uint16_t event)
{
  if(event & IO_EVENT_DMA_READ) {
    rd += 30;
    IO_write(&uart0, buffer+wr, 30);
  }

  if(event & IO_EVENT_DMA_WRITE) {
    wr += 30;
    IO_read(&uart0, buffer+rd, 30);
  }
}

void uart_event(IO_io *io, uint16_t event)
{
  if(event & IO_EVENT_DMA_READ) {
    rd += 30;
    IO_write(&ssi0, buffer+wr, 30);
  }

  if(event & IO_EVENT_DMA_WRITE) {
    wr += 30;
    IO_read(&ssi0, buffer+rd, 30);
  }
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_ssi_attrs ssi0_attrs;
  memset(&ssi0_attrs, 0, sizeof(IO_ssi_attrs));
  ssi0_attrs.bandwidth = 2000000;
  ssi0_attrs.frame_size = 8;
  ssi0_attrs.frame_format = SSI_FRAME_FREESCALE;
  ssi0_attrs.freescale_spo = 1;
  ssi0_attrs.freescale_sph = 1;

#ifndef RECEIVER
  ssi0_attrs.master = 1;
#else
  ssi0_attrs.master = 0;
  ssi0_attrs.slave_out = 1;
#endif

  IO_uart_init(&uart0, 0, IO_NONBLOCKING|IO_ASYNC|IO_DMA, 115200);
  IO_ssi_init(&ssi0, 0, IO_NONBLOCKING|IO_ASYNC|IO_DMA, &ssi0_attrs);
  uart0.event = uart_event;
  ssi0.event = ssi_event;

  wr = 0;
  rd = 0;

#ifdef RECEIVER
  IO_read(&ssi0, buffer, 30);
#else
  IO_read(&uart0, buffer, 30);
#endif

  while(1)
    IO_wait_for_interrupt();
}
