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
int32_t IO_write(IO_io *io, const void *data, uint32_t length)
{
  return (*io->write)(io, data, length);
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
// Print unsigned int to an IO device
//------------------------------------------------------------------------------
static int32_t printNum(IO_io *io, uint64_t num, int base)
{
  char buffer[32];
  uint32_t len = printNumStr(buffer, num, base);
  return IO_write(io, buffer, len);
}

//------------------------------------------------------------------------------
// Print signed int to an IO device
//------------------------------------------------------------------------------
static int32_t printNumS(IO_io *io, int64_t num)
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
  return IO_write(io, buffer, len);
}

//------------------------------------------------------------------------------
// Print a floating point to an IO device
//------------------------------------------------------------------------------
static uint32_t printFloat(IO_io *io, double num)
{
  //----------------------------------------------------------------------------
  // Normalize the number
  //----------------------------------------------------------------------------
  int32_t exponent = 0;
  double numAbs = fabs(num);
  if(numAbs) {
    while(numAbs > 999999999.0) {
      ++exponent;
      numAbs /= 10.0;
    }
    while(numAbs < 0.000000001) {
      --exponent;
      numAbs *= 10;
    }
  }

  //----------------------------------------------------------------------------
  // Print
  //----------------------------------------------------------------------------
  int32_t integer = (int32_t)numAbs;
  int32_t len = 0;
  if(num < 0) len = printNumS(io, -integer);
  else len = printNumS(io, integer);

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
    len += IO_write(io, buffer, i);
  }

  if(exponent) {
    IO_write(io, "e", 1);
    len += printNumS(io, exponent);
  }
  return len;
}

//------------------------------------------------------------------------------
// Print formated string to an IO device
//------------------------------------------------------------------------------
int32_t IO_print(IO_io *io, const char *format, ...)
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
      ret = IO_write(io, start, length);
      if(ret < 0) return ret;
      written += ret;
      ++cursor;
      if(*cursor == 0)
        break;

      if(*cursor == 's') {
        const char *str = va_arg(ap, const char*);
        ret = IO_write(io, str, strlen(str));
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
          ret = printFloat(io, num);
          if(ret < 0) return ret;
          written += ret;
        }
        else if(!sgn) {
          uint64_t num;
          if(sz == 0) num = va_arg(ap, unsigned);
          else if(sz == 1) num = va_arg(ap, unsigned long);
          else num = va_arg(ap, unsigned long long);
          ret = printNum(io, num, base);
          if(ret < 0) return ret;
          written += ret;
        }
        else {
          int64_t num;
          if(sz == 0) num = va_arg(ap, int);
          else if(sz == 1) num = va_arg(ap, long);
          else num = va_arg(ap, long long);
          ret = printNumS(io, num);
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
    ret = IO_write(io, start, length);
    if(ret < 0) return ret;
    written += ret;
  }
  va_end (ap);
  return written;
}

//------------------------------------------------------------------------------
// Read data from an input device
//------------------------------------------------------------------------------
int32_t IO_read(IO_io *io, void *data, uint32_t length)
{
  return (*io->read)(io, data, length);
}

//------------------------------------------------------------------------------
// Check if the character is a whitespace
//------------------------------------------------------------------------------
static int is_whitespace(uint8_t chr)
{
  if(chr >= 9 && chr <= 13)
    return 1;
  if(chr == 32)
    return 1;
  return 0;
}

//------------------------------------------------------------------------------
// Scan a string
//------------------------------------------------------------------------------
static int32_t scan_string(IO_io *io, char *data, uint32_t length)
{
  if(length < 2)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Skip the initial whitespaces
  //----------------------------------------------------------------------------
  uint8_t chr;
  while(1) {
    int32_t ret = IO_read(io, &chr, 1);
    if(ret <= 0) return ret;
    if(!is_whitespace(chr))
      break;
  }

  //----------------------------------------------------------------------------
  // Read the data
  //----------------------------------------------------------------------------
  data[0] = chr;
  uint32_t i;
  for(i = 1; i < length-1; ++i) {
    int32_t ret = IO_read(io, &data[i], 1);
    if(ret <= 0) return ret;
    if(is_whitespace(data[i])) {
      data[i] = 0;
      return i;
    }
  }
  data[i] = 0;
  return length;
}

//------------------------------------------------------------------------------
// Is digit
//------------------------------------------------------------------------------
static int is_digit(char chr, uint8_t base)
{
  if(base <= 10) {
    if(chr >= '0' && chr < ('0'+base))
      return 1;
  }
  else {
    if(chr >= '0' && chr <= '9')
      return 1;
    if(chr >= 'a' && chr < ('a'+base-10))
      return 1;
  }
  return 0;
}

//------------------------------------------------------------------------------
// Check if the scanned characters are correct
//------------------------------------------------------------------------------
int check_chars(const char *data, uint8_t type, uint8_t base)
{
  if(base == 0)
    base = 10;

  int minuses = 0;
  int es = 0;
  int dots = 0;

  for(const char *cursor = data; *cursor; ++cursor) {

    //--------------------------------------------------------------------------
    // Minus
    //--------------------------------------------------------------------------
    if(*cursor == '-') {
      ++minuses;

      if(type == IO_INT32 && minuses == 1 && cursor == data &&
         is_digit(*(cursor+1), base))
        continue;

      if(type == IO_DOUBLE && minuses <= 2) {
        if(cursor == data && is_digit(*(cursor+1), base))
          continue;
        if(*(cursor-1) == 'e' && is_digit(*(cursor+1), base))
          continue;
      }
    }

    //--------------------------------------------------------------------------
    // e
    //--------------------------------------------------------------------------
    else if(*cursor == 'e') {
      ++es;
      if(type == IO_DOUBLE && cursor != data && es == 1)
        continue;
    }

    //--------------------------------------------------------------------------
    // .
    //--------------------------------------------------------------------------
    else if(*cursor == '.') {
      ++dots;
      if(type == IO_DOUBLE && cursor != data && dots == 1 &&
         is_digit(*(cursor-1), base) && is_digit(*(cursor+1), base))
        continue;
    }

    //--------------------------------------------------------------------------
    // Something else
    //--------------------------------------------------------------------------
    else if(is_digit(*cursor, base))
      continue;

    //--------------------------------------------------------------------------
    // Unacceptable character
    //--------------------------------------------------------------------------
    return 0;
  }
  return 1;
}

//------------------------------------------------------------------------------
// Parse uint64
//------------------------------------------------------------------------------
void parse_uint64(uint64_t *result, char *buffer, uint8_t base)
{
  if(base == 0) base = 10;
  uint64_t n = 0;
  uint64_t mul = 1;
  uint8_t  tmp = 0;
  uint32_t len = strlen(buffer);
  for(int i = len-1; i >= 0; --i) {
    if(buffer[i] <= '9')
      tmp = buffer[i] - '0';
    else
      tmp = 10 + buffer[i] - 'a';
    n += tmp*mul;
    mul *= base;
  }
  *result = n;
}

//------------------------------------------------------------------------------
// Parse uint32
//------------------------------------------------------------------------------
void parse_uint32(uint32_t *result, char *buffer, uint8_t base)
{
  uint64_t r;
  parse_uint64(&r, buffer, base);
  *result = (uint32_t)r;
}

//------------------------------------------------------------------------------
// Parse int64
//------------------------------------------------------------------------------
void parse_int64(int64_t *result, char *buffer)
{
  uint64_t r;
  if(buffer[0] == '-') {
    parse_uint64(&r, buffer+1, 10);
    *result = -r;
  }
  else {
    parse_uint64(&r, buffer, 10);
    *result = r;
  }
}

//------------------------------------------------------------------------------
// Parse int32
//------------------------------------------------------------------------------
void parse_int32(int32_t *result, char *buffer)
{
  int64_t r;
  parse_int64(&r, buffer);
  *result = r;
}

//------------------------------------------------------------------------------
// Parse double
//------------------------------------------------------------------------------
void parse_double(double *result, char *buffer)
{
  *result = 0;
  //----------------------------------------------------------------------------
  // Find the parts
  //----------------------------------------------------------------------------
  char *integer = buffer;
  char *fraction = 0;
  char *exponent = 0;
  int32_t len = strlen(buffer);
  for(int i = 0; i < len; ++i) {
    if(buffer[i] == '.') {
      buffer[i] = 0;
      fraction = buffer+i+1;
    }
    else if(buffer[i] == 'e') {
      buffer[i] = 0;
      exponent = buffer+i+1;
    }
  }

  //----------------------------------------------------------------------------
  // Convert the parts
  //----------------------------------------------------------------------------
  int64_t  i_integer;
  uint64_t i_fraction = 0;
  uint32_t l_fraction = strlen(fraction);
  int64_t  i_exponent = 0;

  parse_int64(&i_integer, integer);
  if(fraction)
    parse_uint64(&i_fraction, fraction, 10);
  if(exponent)
    parse_int64(&i_exponent, exponent);

  //----------------------------------------------------------------------------
  // Put everything together
  //----------------------------------------------------------------------------
  double d = i_integer;
  double df = i_fraction;
  for(int i = 0; i < l_fraction; ++i)
    df /= 10.0;
  d += df;
  if(i_exponent > 0)
    for(int i = 0; i < i_exponent; ++i)
      d *= 10.0;
  else
    for(int i = 0; i > i_exponent; --i)
      d /= 10.0;
  *result = d;
}

//------------------------------------------------------------------------------
// Read and parse data from the input
//------------------------------------------------------------------------------
int32_t IO_scan(IO_io *io, uint8_t type, void *data, uint32_t param)
{
  if(type > IO_DOUBLE)
    return -IO_EINVAL;

  if(type == IO_STRING)
    return scan_string(io, data, param);

  char buffer[32];
  int32_t ret = scan_string(io, buffer, 32);
  if(ret < 0) return ret;
  switch(type) {
    case IO_INT32:
      if(!check_chars(buffer, IO_INT32, 10))
        return -IO_EINVAL;
      parse_int32(data, buffer);
      break;
    case IO_INT64:
      if(!check_chars(buffer, IO_INT32, 10))
        return -IO_EINVAL;
      parse_int64(data, buffer);
      break;
    case IO_UINT32:
      if(!check_chars(buffer, IO_UINT32, param))
        return -IO_EINVAL;
      parse_uint32(data, buffer, param);
      break;
    case IO_UINT64:
      if(!check_chars(buffer, IO_UINT32, param))
        return -IO_EINVAL;
      parse_uint64(data, buffer, param);
      break;
    case IO_DOUBLE:
      if(!check_chars(buffer, IO_DOUBLE, 10))
        return -IO_EINVAL;
      parse_double(data, buffer);
      break;
  }
  return ret;
}

//------------------------------------------------------------------------------
// Enable events on IO device
//------------------------------------------------------------------------------
int32_t __IO_event_enable(IO_io *io, uint16_t events)
{
  return -IO_ENOSYS;
}
WEAK_ALIAS(__IO_event_enable, IO_event_enable);

//------------------------------------------------------------------------------
// Disable events on IO device
//------------------------------------------------------------------------------
int32_t __IO_event_disable(IO_io *io, uint16_t events)
{
  return -IO_ENOSYS;
}
WEAK_ALIAS(__IO_event_disable, IO_event_disable);

//------------------------------------------------------------------------------
// Enable interrupts
//------------------------------------------------------------------------------
void __IO_enable_interrupts() {}
WEAK_ALIAS(__IO_enable_interrupts, IO_enable_interrupts);

//------------------------------------------------------------------------------
// Disable interrupts
//------------------------------------------------------------------------------
void __IO_disable_interrupts() {}
WEAK_ALIAS(__IO_disable_interrupts, IO_disable_interrupts);

//------------------------------------------------------------------------------
// Wait for an interrupt
//------------------------------------------------------------------------------
void __IO_wait_for_interrupt() {}
WEAK_ALIAS(__IO_wait_for_interrupt, IO_wait_for_interrupt);

//------------------------------------------------------------------------------
// Sync
//------------------------------------------------------------------------------
uint32_t IO_sync(IO_io *io)
{
  return io->sync(io);
}

//------------------------------------------------------------------------------
// Write a double word to a device
//------------------------------------------------------------------------------
int32_t IO_set(IO_io *io, uint64_t data)
{
  if(io->type != IO_GPIO && io->type != IO_TIMER && io->type != IO_ADC)
    return -IO_EINVAL;
  return IO_write(io, &data, 1);
}

//------------------------------------------------------------------------------
// Get a double word from a device
//------------------------------------------------------------------------------
int32_t IO_get(IO_io *io, uint64_t *data)
{
  if(io->type != IO_GPIO && io->type != IO_TIMER && io->type != IO_ADC)
    return -IO_EINVAL;
  return IO_read(io, data, 1);
}
