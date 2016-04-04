
set(TM4C_LIB -Wl,--whole-archive tm4c -Wl,--no-whole-archive)

macro(add_test name)
  add_executable(${name}.axf ${name}.c)
  add_raw_binary(${name}.bin ${name}.axf)
  target_link_libraries(${name}.axf ${TM4C_LIB} io ${TIVAWARE_LIB})
endmacro()

set(tests startup;uart)

foreach(i RANGE 1)
  list(GET tests ${i} name)
  if(i LESS 10)
    add_test(test-0${i}-${name})
  else()
    add_test(test-${i}-${name})
  endif()
endforeach()
