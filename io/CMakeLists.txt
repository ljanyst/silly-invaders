
macro(add_font name size)
  set(out_name ${CMAKE_BINARY_DIR}/io/fonts/${name}-${size}.c)
  set(font_name ${CMAKE_SOURCE_DIR}/io/fonts/${name}.ttf)
  add_custom_command(
    OUTPUT ${out_name}
    COMMAND ${CMAKE_SOURCE_DIR}/io/fonts/extract-glyphs.py ${name}${size} ${font_name} ${size} ${out_name}
    DEPENDS ${font_name}
    COMMENT "Creating font ${name}-${size}")
endmacro()

add_font(DejaVuSans 10)
add_font(DejaVuSerif 10)
add_font(SilkScreen 8)

add_library(
  io STATIC
  IO.c
  IO_device.c
  IO_display.c
  IO_malloc.c
  IO_font.c
  IO_sound.c
  fonts/DejaVuSans-10.c
  fonts/DejaVuSerif-10.c
  fonts/SilkScreen-8.c)
