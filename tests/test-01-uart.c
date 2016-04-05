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

//------------------------------------------------------------------------------
// Print the parsed value
//------------------------------------------------------------------------------
#define PRINT(RET, VAL, TYPE)                                    \
  if(RET > 0)                                                    \
    IO_print(&uart0, "You entered: " TYPE "\r\n", VAL);      \
  else                                                           \
    IO_print(&uart0, "Error while scanning data\r\n");

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  IO_init();
  IO_io uart0;
  IO_uart_init(&uart0, 0, 0, 115200);

  while(1) {
    //--------------------------------------------------------------------------
    // Type choice
    //--------------------------------------------------------------------------
    uint32_t choice;
    IO_print(&uart0,
             "Choose the type of variable you want to enter (6 for help)\r\n");
    int32_t ret = IO_scan(&uart0, IO_UINT32, &choice, 0);
    if(ret < 0 || choice > 6)
      continue;
    switch(choice) {

      //------------------------------------------------------------------------
      // String
      //------------------------------------------------------------------------
      case IO_STRING: {
        char buffer[64];
        IO_print(&uart0, "Enter a string\r\n");
        ret = IO_scan(&uart0, IO_STRING, buffer, 64);
        PRINT(ret, buffer, "%s");
        break;
      }

      //------------------------------------------------------------------------
      // Int32
      //------------------------------------------------------------------------
      case IO_INT32: {
        int32_t d;
        IO_print(&uart0, "Enter an int32\r\n");
        ret = IO_scan(&uart0, IO_INT32, &d, 0);
        PRINT(ret, d, "%d");
        break;
      }

      //------------------------------------------------------------------------
      // Int64
      //------------------------------------------------------------------------
      case IO_INT64: {
        int64_t d;
        IO_print(&uart0, "Enter an int64\r\n");
        ret = IO_scan(&uart0, IO_INT64, &d, 0);
        PRINT(ret, d, "%lld");
        break;
      }

      //------------------------------------------------------------------------
      // UInt32
      //------------------------------------------------------------------------
      case IO_UINT32: {
        uint32_t d;
        IO_print(&uart0, "Enter an uint32\r\n");
        ret = IO_scan(&uart0, IO_UINT32, &d, 0);
        PRINT(ret, d, "%u");
        break;
      }

      //------------------------------------------------------------------------
      // UInt64
      //------------------------------------------------------------------------
      case IO_UINT64: {
        uint64_t d;
        IO_print(&uart0, "Enter an uint64\r\n");
        ret = IO_scan(&uart0, IO_UINT64, &d, 0);
        PRINT(ret, d, "%llu");
        break;
      }

      //------------------------------------------------------------------------
      // Double
      //------------------------------------------------------------------------
      case IO_DOUBLE: {
        double d;
        IO_print(&uart0, "Enter a double\r\n");
        ret = IO_scan(&uart0, IO_DOUBLE, &d, 0);
        PRINT(ret, d, "%lf");
        break;
      }

      //------------------------------------------------------------------------
      // Help
      //------------------------------------------------------------------------
      case 6: {
        IO_print(&uart0, "Valid scan types:\r\n");
        IO_print(&uart0, " * 0 - string\r\n");
        IO_print(&uart0, " * 1 - int32\r\n");
        IO_print(&uart0, " * 2 - int64\r\n");
        IO_print(&uart0, " * 3 - uint32\r\n");
        IO_print(&uart0, " * 4 - uint64\r\n");
        IO_print(&uart0, " * 5 - double\r\n");
        break;
      }
    }
  }
}
