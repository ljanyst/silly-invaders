
macro(add_bitmap name)
  set(out_name bitmaps/${name}.c)
  set(bmp_name bitmaps/${name}.bmp)
  add_custom_command(
    OUTPUT ${out_name}
    COMMAND ${CMAKE_SOURCE_DIR}/game/bitmaps/convert-bitmap.py ${name} ${bmp_name} ${out_name}
    DEPENDS ${fonr_name}
    COMMENT "Creating bitmap ${name}")
endmacro()

set(TM4C_LIB -Wl,--whole-archive tm4c_platform_01 tm4c pcd8544 -Wl,--no-whole-archive)

add_bitmap(BunkerDamagedImg)
add_bitmap(BunkerImg)
add_bitmap(DefenderImg)
add_bitmap(HeartImg)
add_bitmap(Invader1Img)
add_bitmap(Invader2Img)
add_bitmap(Invader3Img)
add_bitmap(Invader4Img)

add_executable(
  silly-invaders.axf
  silly-invaders.c
  SI_hardware.c
  SI_scene.c
  SI_scene_game.c
  SI_scene_intro.c
  SI_scene_level.c
  bitmaps/BunkerDamagedImg.c
  bitmaps/BunkerImg.c
  bitmaps/DefenderImg.c
  bitmaps/HeartImg.c
  bitmaps/Invader1Img.c
  bitmaps/Invader2Img.c
  bitmaps/Invader3Img.c
  bitmaps/Invader4Img.c)

target_link_libraries(silly-invaders.axf ${TM4C_LIB} io ${TIVAWARE_LIB})

add_raw_binary(silly-invaders.bin silly-invaders.axf)
