
add_library(
  tm4c STATIC
  TM4C_startup.c
  TM4C_uart.c
  TM4C_dma.c
  TM4C_ssi.c
  TM4C_gpio.c
  TM4C.c)

add_library(
  tm4c_platform_01 STATIC
  TM4C_platform_01.c)
