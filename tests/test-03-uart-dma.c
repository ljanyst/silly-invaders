//------------------------------------------------------------------------------
// Copyright (c) 2016 by Lukasz Janyst <lukasz@jany.st>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED 'AS IS' AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
//------------------------------------------------------------------------------

#include <io/IO.h>
#include <io/IO_uart.h>

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
  IO_init();
  IO_io uart0;
  IO_uart_init(&uart0, 0, IO_DMA|IO_ASYNC, 115200);
  uart0.event = uart_event;
  IO_read(&uart0, buffer, 30);
  while(1)
    IO_wait_for_interrupt();
}
