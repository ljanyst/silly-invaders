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

//------------------------------------------------------------------------------
// Print the parsed value
//------------------------------------------------------------------------------
#define PRINT(RET, VAL, TYPE)                                    \
  if(RET > 0)                                                    \
    IO_print(&uart0, "You entered: " TYPE "\r\n", VAL);          \
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
