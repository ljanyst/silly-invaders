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
