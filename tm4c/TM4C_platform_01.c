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

#define __IO_IMPL__

#include <io/IO.h>
#include <io/IO_device.h>
#include <io/IO_error.h>
#include <io/IO_display.h>
#include <io/IO_display_low.h>
#include <io/IO_sound.h>
#include <io/IO_sound_low.h>

#include "TM4C.h"
#include "TM4C_gpio.h"

#include <drivers/pcd8544/pcd8544.h>

//------------------------------------------------------------------------------
// Initialize the platform
//------------------------------------------------------------------------------
int32_t IO_init(uint32_t stack_size)
{
  return TM4C_init(stack_size);
}

//------------------------------------------------------------------------------
// Initialize a button
//------------------------------------------------------------------------------
int32_t IO_button_init(IO_io *io, uint8_t module, uint16_t flags)
{
  if(module > 1)
    return -IO_EINVAL;

  if(module == 0)
    return IO_gpio_init(io, 40, flags, 0);

  return IO_gpio_init(io, 44, flags, 0);
}

//------------------------------------------------------------------------------
// Initialize an LED
//------------------------------------------------------------------------------
int32_t IO_led_init(IO_io *io, uint8_t module)
{
  if(module > 0)
    return -IO_EINVAL;

  return IO_gpio_init(io, 37, 0, 1);
}

//------------------------------------------------------------------------------
// Initialize a slider
//------------------------------------------------------------------------------
int32_t IO_slider_init(IO_io *io, uint8_t module, uint16_t flags)
{
  if(module > 0)
    return -IO_EINVAL;

  return IO_adc_init(io, 0, flags);
}

//------------------------------------------------------------------------------
// The display structs
//------------------------------------------------------------------------------
static pcd8544 display0;

//------------------------------------------------------------------------------
// Write the pixel matrix to the device
//------------------------------------------------------------------------------
static int32_t display_sync(IO_io *io)
{
  return PCD8544_sync(&display0);
}

//------------------------------------------------------------------------------
// Initialize a display device
//------------------------------------------------------------------------------
int32_t IO_display_init_low(IO_io *io, uint8_t module)
{
  if(module != 0)
    return -IO_EINVAL;

  //----------------------------------------------------------------------------
  // Initialize the display 0
  //----------------------------------------------------------------------------
  io->type    = IO_DISPLAY;
  io->sync    = display_sync;
  io->channel = 0;
  io->flags   = 0;
  io->read    = 0;
  io->write   = 0;
  io->event   = 0;
  return PCD8544_init(&display0, 0, 6, 7);
}

//------------------------------------------------------------------------------
// Get parameters of the device
//------------------------------------------------------------------------------
int32_t IO_display_get_attrs_low(IO_io *io, IO_display_attrs *attrs)
{
  if(io->type != IO_DISPLAY || io->channel != 0)
    return -IO_EINVAL;

  return PCD8544_get_attrs(attrs);
}

//------------------------------------------------------------------------------
// Clear the display
//------------------------------------------------------------------------------
int32_t IO_display_clear_low(IO_io *io)
{
  if(io->type != IO_DISPLAY || io->channel != 0)
    return -IO_EINVAL;

  return PCD8544_clear(&display0);
}

//------------------------------------------------------------------------------
// Put a pixel on the screen
//------------------------------------------------------------------------------
int32_t IO_display_put_pixel(IO_io *io, uint16_t x, uint16_t y, uint32_t argb)
{
  if(io->type != IO_DISPLAY || io->channel != 0)
    return -IO_EINVAL;

  return PCD8544_put_pixel(&display0, x, y, argb);
}

//------------------------------------------------------------------------------
// Get number of display devices available
//------------------------------------------------------------------------------
int32_t IO_display_count_low()
{
  return 1;
}

//------------------------------------------------------------------------------
// DAC
//------------------------------------------------------------------------------
static uint32_t *dac_data;

//------------------------------------------------------------------------------
// Set DAC state
//------------------------------------------------------------------------------
static int32_t dac_write(IO_io *io, const void *data, uint32_t length)
{
  //----------------------------------------------------------------------------
  // Extract the value
  //----------------------------------------------------------------------------
  if(length != 1)
    return -IO_EINVAL;

  const uint64_t *val = data;

  *dac_data = *val;

  return 1;
}


//------------------------------------------------------------------------------
// Initialize a DAC
//------------------------------------------------------------------------------
int32_t IO_dac_init(IO_io *io, uint8_t module)
{
  if(module > 0)
    return -IO_EINVAL;

  TM4C_gpio_port_init(GPIO_PORTD_NUM);
  TM4C_gpio_pin_init(GPIO_PORTD_NUM, GPIO_PIN0_NUM, 0, 0, 1);
  TM4C_gpio_pin_init(GPIO_PORTD_NUM, GPIO_PIN1_NUM, 0, 0, 1);
  TM4C_gpio_pin_init(GPIO_PORTD_NUM, GPIO_PIN2_NUM, 0, 0, 1);
  TM4C_gpio_pin_init(GPIO_PORTD_NUM, GPIO_PIN3_NUM, 0, 0, 1);

  uint32_t addr =  GPIO_REG_BASE + GPIO_PORTD;
  addr += GPIO_PIN0_BIT_OFFSET;
  addr += GPIO_PIN1_BIT_OFFSET;
  addr += GPIO_PIN2_BIT_OFFSET;
  addr += GPIO_PIN3_BIT_OFFSET;
  dac_data = (uint32_t*)addr;

  io->type    = IO_DAC;
  io->sync    = 0;
  io->channel = 0;
  io->flags   = 0;
  io->read    = 0;
  io->write   = dac_write;
  io->event   = 0;

  return 0;
}

//------------------------------------------------------------------------------
// Sound
//------------------------------------------------------------------------------
static IO_io snd_timer;
static IO_io snd_dac;
uint64_t     snd_interval = 0;
uint8_t      snd_step = 0;

// Found in EdX Embedded Systems Course materials
static const uint8_t snd_trumpet[] = {
  10, 11, 11, 12, 10,  8,  3,  1,  8, 15, 15, 11, 10, 10, 11, 10, 10, 10, 10,
  10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 10, 10, 10 };

//------------------------------------------------------------------------------
// Sound timer event
//------------------------------------------------------------------------------
static void snd_timer_event(IO_io *io, uint16_t event)
{
  IO_set(&snd_dac, snd_trumpet[snd_step++]);
  snd_step %= 32;
  if(snd_interval)
    IO_set(io, snd_interval);
}

//------------------------------------------------------------------------------
// Write a tone frequency
//------------------------------------------------------------------------------
static int32_t snd_write(IO_io *io, const void *data, uint32_t length)
{
  //----------------------------------------------------------------------------
  // Extract the value
  //----------------------------------------------------------------------------
  if(length != 1)
    return -IO_EINVAL;
  const uint64_t *val = data;

  if(!(*val)) {
    snd_interval = 0;
    return 1;
  }
  uint8_t turn_on = 0;
  if(!snd_interval)
    turn_on = 1;

  double interval = 1.0/(*val);
  interval /= 32.0;
  interval *= 1000000000;
  snd_interval = interval;

  if(turn_on)
    IO_set(&snd_timer, interval);
  return 1;
}

//------------------------------------------------------------------------------
// Initialize a sound device
//------------------------------------------------------------------------------
int32_t IO_sound_init(IO_io *io, uint8_t module)
{
  if(module > 0)
    return -IO_EINVAL;

  IO_dac_init(&snd_dac, 0);
  IO_timer_init(&snd_timer, 11);
  TM4C_enable_interrupt(104, 0); // adjust the interrupt priority for timer 11
  snd_timer.event = snd_timer_event;

  io->type    = IO_SOUND;
  io->sync    = 0;
  io->channel = 0;
  io->flags   = 0;
  io->read    = 0;
  io->write   = snd_write;
  io->event   = 0;

  return 0;
}

//------------------------------------------------------------------------------
// Get number of sound devices available
//------------------------------------------------------------------------------
int32_t IO_sound_count_low()
{
  return 1;
}
