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

//------------------------------------------------------------------------------
// Register addresses
//------------------------------------------------------------------------------
#define STCTRL_REG           (*(volatile unsigned long *)0xe000e010)
#define STRELOAD_REG         (*(volatile unsigned long *)0xe000e014)
#define STCURRENT_REG        (*(volatile unsigned long *)0xe000e018)
#define SYSPRI3_REG          (*(volatile unsigned long *)0xe000ed20)

#define GPIOHBCTL_REG        (*(volatile unsigned long *)0x400fe06c)
#define RCGCGPIO_REG         (*(volatile unsigned long *)0x400fe608)

#define GPIODATA_REG_PORTF   (*(volatile unsigned long *)0x4005d3fc)
#define GPIODIR_REG_PORTF    (*(volatile unsigned long *)0x4005d400)
#define GPIOAFSEL_REG_PORTF  (*(volatile unsigned long *)0x4005d420)
#define GPIOPUR_REG_PORTF    (*(volatile unsigned long *)0x4005d510)
#define GPIODEN_REG_PORTF    (*(volatile unsigned long *)0x4005d51c)
#define GPIOLOCK_REG_PORTF   (*(volatile unsigned long *)0x4005d520)
#define GPIOCR_REG_PORTF     (*(volatile unsigned long *)0x4005d524)
#define GPIOAMSEL_REG_PORTF  (*(volatile unsigned long *)0x4005d528)
#define GPIOPCTL_REG_PORTF   (*(volatile unsigned long *)0x4005d52c)

unsigned long led = 0x02;
unsigned long current = 0;

//------------------------------------------------------------------------------
// Initialize the SysTick timer
//------------------------------------------------------------------------------
void init_sys_tick()
{
  STCTRL_REG    = 0;                  // disable
  STRELOAD_REG  = 0x00ffffff;         // maximum reload value
  STCURRENT_REG = 0;                  // clear the current
  SYSPRI3_REG   = (SYSPRI3_REG & 0x00FFFFFF)|0x40000000; // priority to 2
  STCTRL_REG    = 0x00000007;         // enable with the system clock and
                                      // interrupts
}

//------------------------------------------------------------------------------
// Initialize GPIO
//------------------------------------------------------------------------------
void init_gpio()
{
  volatile unsigned long delay;

  // Enable the high performance bus (AHB) for Port F.
  GPIOHBCTL_REG |= 0x20;

  // Enable the clock for port F.
  RCGCGPIO_REG |= 0x20;

  // Do something to wait a couple cycles until the clock settles. We fetch
  // the value of the register to give it enough time.
  delay = RCGCGPIO_REG;
  (void)delay;

  // Additional protection is provided for pins that can be used as JTAG and
  // NMI. One of these pins is PF0 which we want to use because it connects to
  // the SW2 switch of the board. To enable it, we need to write a magic number
  // to the corresponding port lock register. Also to enable commiting to the
  // bits correspionding to pin the PF0 of GPIOAFSEL, GPIOPUR, GPIOPDR and
  // GPIODEN, we need to set the corresponding bit in the GPIOCR commit
  // register.
  GPIOLOCK_REG_PORTF  = 0x4c4f434b;
  GPIOCR_REG_PORTF   |= 0x01;

  // Select the direction for the pins: PF0 and PF4 (the switches) are input,
  // PF1-PF3 (the LEDs) are output
  GPIODIR_REG_PORTF |=  0x0e;

  // Enable the pullup resistors for PF4 (SW1) and PF0 (SW2).
  GPIOPUR_REG_PORTF |=  0x11;

  // Enable pins PF0-PF4
  GPIODEN_REG_PORTF |= 0x1f;
}

//------------------------------------------------------------------------------
// Handle the systic interrupt
//------------------------------------------------------------------------------
void systick_handler()
{
  if(current) {
    GPIODATA_REG_PORTF &= ~current;
    current = 0;
  }
  else {
    current = led;
    GPIODATA_REG_PORTF |= current;
  }
}

//------------------------------------------------------------------------------
// Start the show
//------------------------------------------------------------------------------
int main()
{
  init_sys_tick();
  init_gpio();
  while(1) {
    unsigned long sw1 = !(GPIODATA_REG_PORTF & 0x01);
    unsigned long sw2 = !(GPIODATA_REG_PORTF & 0x10);
    if(sw1 && sw2) led = 0x06;
    else if(sw1) led = 0x04;
    else if(sw2) led = 0x08;
    else led = 0x02;
    for(int i = 0; i < 100000; ++i);
  }
}
