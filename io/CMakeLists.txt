
macro(add_font name size)
  set(out_name fonts/${name}-${size}.c)
  set(font_name fonts/${name}.ttf)
  add_custom_command(
    OUTPUT ${out_name}
    COMMAND ${CMAKE_SOURCE_DIR}/io/fonts/extract-glyphs.py ${name}${size} ${font_name} ${size} ${out_name}
    DEPENDS ${fonr_name}
    COMMENT "Creating font ${name}-${size}")
endmacro()

add_font(DejaVuSans 10)
add_font(DejaVuSerif 10)

add_library(
  io STATIC
  IO.c
  IO_uart.c
  IO_ssi.c
  IO_gpio.c
  IO_display.c
  IO_malloc.c
  IO_font.c
  IO_button.c
  IO_timer.c
  fonts/DejaVuSans-10.c
  fonts/DejaVuSerif-10.c)
