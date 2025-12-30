function(make_test NAME)
  add_executable(${NAME} source/${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE dtlx dtl Boost::ut fmt::fmt)

  if(NOT MSVC)
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)
    target_compile_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
    target_link_options(${NAME} PRIVATE -fsanitize=address,leak,undefined)
  else()
    target_compile_options(${NAME} PRIVATE /W4)
  endif()

  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
  add_custom_command(TARGET ${NAME} POST_BUILD COMMAND $<TARGET_FILE:${NAME}>)
endfunction()

enable_testing()

make_test(intdiff_test)
make_test(strdiff_test)
make_test(objdiff_test)
make_test(strmerge_test)
make_test(strpatch_test)
make_test(filediff_test)

add_custom_command(
  TARGET filediff_test
  COMMENT "Symlinking resource directory to build directory"
  PRE_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/resource/
    $<TARGET_FILE_DIR:filediff_test>/resource
)
