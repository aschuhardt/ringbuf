add_executable(run_tests
  ${CMAKE_CURRENT_SOURCE_DIR}/ctest/main.c
  unit_tests.c)
target_include_directories(run_tests
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/ctest
  ${RINGBUF_INCLUDE_DIR})
target_link_libraries(run_tests
  PRIVATE
  ${RINGBUF_LIBRARY})

add_custom_target(test
  COMMAND run_tests
  DEPENDS run_tests
  USES_TERMINAL)
