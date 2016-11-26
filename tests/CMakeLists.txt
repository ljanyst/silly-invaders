
set(TM4C_LIB -Wl,--whole-archive tm4c_platform_01 tm4c pcd8544 -Wl,--no-whole-archive)

macro(add_test name)
  add_executable(${name}.axf ${name}.c)
  add_raw_binary(${name}.bin ${name}.axf)
  target_link_libraries(${name}.axf ${TM4C_LIB} io ${TIVAWARE_LIB})
endmacro()

set(tests startup;uart;uart-async;uart-dma;ssi-dma;gpio;display;timer;input;sound;rng;os-basic)

add_executable(test-00-startup.axf test-00-startup.c)
add_raw_binary(test-00-startup.bin test-00-startup.axf)

foreach(i RANGE 1 11)
  list(GET tests ${i} name)
  if(i LESS 10)
    add_test(test-0${i}-${name})
  else()
    add_test(test-${i}-${name})
  endif()
endforeach()
