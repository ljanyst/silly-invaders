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

#include "IO.h"
#include "IO_error.h"
#include "IO_utils.h"

#include <stdarg.h>
#include <string.h>
#include <math.h>

//------------------------------------------------------------------------------
// Dummu IO initializer
//------------------------------------------------------------------------------
int32_t __IO_init()
{
  return -IO_ENOSYS;
}

WEAK_ALIAS(__IO_init, IO_init);

//------------------------------------------------------------------------------
// Write data to an output device
//------------------------------------------------------------------------------
int32_t IO_write(IO_output *out, const void *data, uint32_t length)
{
  int32_t i = 0;
  const uint8_t *b_data = data;
  for(i = 0; i < length; ++i) {
    int ret = (*out->put_byte)(out, b_data[i]);
    if(ret) return ret;
  }
  return i;
}

//------------------------------------------------------------------------------
// Print an unsigned integer to a string; the string buffer needs to be at lease
// 32 characters long
//------------------------------------------------------------------------------
static const char digits[] = "0123456789abcdef";
static uint32_t printNumStr(char *str, uint64_t num, int base)
{
  if(base <= 0 || base > 16)
    return 0;

  if(num == 0) {
    str[0] = '0';
    return 1;
  }
  uint64_t n = num;
  uint32_t len = 0;
  char *cursor1 = str;
  char *cursor2 = str;
  while(n && cursor1 != str+31) {
    int rem = n % base;
    *cursor1 = digits[rem];
    n /= base;
    ++cursor1;
    ++len;
  }

  *cursor1 = 0;
  --cursor1;
  while(cursor1 > cursor2) {
    char tmp = *cursor1;
    *cursor1 = *cursor2;
    *cursor2 = tmp;
    --cursor1;
    ++cursor2;
  }
  return len;
}

//------------------------------------------------------------------------------
// Print unsigned int to output
//------------------------------------------------------------------------------
static int32_t printNum(IO_output *out, uint64_t num, int base)
{
  char buffer[32];
  uint32_t len = printNumStr(buffer, num, base);
  return IO_write(out, buffer, len);
}

//------------------------------------------------------------------------------
// Print signed int to output
//------------------------------------------------------------------------------
static int32_t printNumS(IO_output *out, int64_t num)
{
  char buffer[33];
  char *ptr = buffer;
  uint32_t len = 0;

  if(num < 0) {
    *ptr = '-';
    ++ptr;
    num = -num;
    ++len;
  }
  len += printNumStr(ptr, num, 10);
  return IO_write(out, buffer, len);
}

//------------------------------------------------------------------------------
// Print a floating point to output
//------------------------------------------------------------------------------
static uint32_t printFloat(IO_output *out, double num)
{
  //----------------------------------------------------------------------------
  // Normalize the number
  //----------------------------------------------------------------------------
  int32_t exponent = 0;
  double numAbs = fabs(num);
  while(numAbs > 999999999.0) {
    ++exponent;
    numAbs /= 10.0;
  }
  while(numAbs < 0.000000001) {
    --exponent;
    numAbs *= 10;
  }

  //----------------------------------------------------------------------------
  // Print
  //----------------------------------------------------------------------------
  int32_t integer = (int32_t)numAbs;
  int32_t len = 0;
  if(num < 0) len = printNumS(out, -integer);
  else len = printNumS(out, integer);

  int32_t fraction = (int32_t)((numAbs-integer) * 1000000000.0);
  if(fraction && 12-len > 1) {
    char buffer[10];
    buffer[0] = '.';
    for(int i = 1; i < 10; ++i) buffer[i] = '0';
    for(int i = 9; fraction && i > 0; --i) {
      int rem = fraction % 10;
      buffer[i] = digits[rem];
      fraction /= 10;
    }
    int i = 9;
    for(; buffer[i] == '0'; --i) buffer[i] = 0;
    ++i;
    if(12-len < i) i = 12-len;
    len += IO_write(out, buffer, i);
  }

  if(exponent) {
    IO_write(out, "e", 1);
    len += printNumS(out, exponent);
  }
  return len;
}

//------------------------------------------------------------------------------
// Print formated string to output
//------------------------------------------------------------------------------
int32_t IO_print(IO_output *out, const char *format, ...)
{
  va_list ap;
  int length  = 0;
  int sz      = 0;
  int base    = 0;
  int sgn     = 0;
  int fl      = 0;
  int ret     = 0;
  int written = 0;
  const char *cursor = format;
  const char *start  = format;

  va_start(ap, format);
  while(*cursor) {
    if(*cursor == '%') {
      ret = IO_write(out, start, length);
      if(ret < 0) return ret;
      written += ret;
      ++cursor;
      if(*cursor == 0)
        break;

      if(*cursor == 's') {
        const char *str = va_arg(ap, const char*);
        ret = IO_write(out, str, strlen(str));
        if(ret < 0) return ret;
        written += ret;
      }

      else {
        while(*cursor == 'l') {
          ++sz;
          ++cursor;
        }
        if(sz > 2) sz = 2;

        if(*cursor == 'x')
          base = 16;
        else if(*cursor == 'u')
          base = 10;
        else if(*cursor == 'o')
          base = 8;
        else if(*cursor == 'd')
          sgn  = 1;
        else if(*cursor == 'f')
          fl   = 1;

        if(fl) {
          double num;
          if(sz <= 1) num = va_arg(ap, double);
          else num = va_arg(ap, long double);
          ret = printFloat(out, num);
          if(ret < 0) return ret;
          written += ret;
        }
        else if(!sgn) {
          uint64_t num;
          if(sz == 0) num = va_arg(ap, unsigned);
          else if(sz == 1) num = va_arg(ap, unsigned long);
          else num = va_arg(ap, unsigned long long);
          ret = printNum(out, num, base);
          if(ret < 0) return ret;
          written += ret;
        }
        else {
          int64_t num;
          if(sz == 0) num = va_arg(ap, int);
          else if(sz == 1) num = va_arg(ap, long);
          else num = va_arg(ap, long long);
          ret = printNumS(out, num);
          if(ret < 0) return ret;
          written += ret;
        }
        sz = 0; base = 0; sgn = 0;
      }
      ++cursor;
      start = cursor;
      length = 0;
      continue;
    }
    ++length;
    ++cursor;
  }
  if(length) {
    ret = IO_write(out, start, length);
    if(ret < 0) return ret;
    written += ret;
  }
  va_end (ap);
  return written;
}

//------------------------------------------------------------------------------
// Read data from an input device
//------------------------------------------------------------------------------
int32_t IO_read(IO_input *in, void *data, uint32_t length)
{
  int32_t i = 0;
  uint8_t *b_data = data;
  for(i = 0; i < length; ++i) {
    int ret = (*in->get_byte)(in, &b_data[i]);
    if(ret) return ret;
  }
  return i;
}
