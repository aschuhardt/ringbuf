cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(ringbuf
  VERSION 1.0.0
  DESCRIPTION "A small, general-purpose ring buffer structure"
  LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(RINGBUF_LIBRARIES ringbuf)
set(RINGBUF_LIBRARY ${RINGBUF_LIBRARIES})
set(RINGBUF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RINGBUF_INCLUDE_DIRS ${RINGBUF_INCLUDE_DIR})

add_library(${RINGBUF_LIBRARY} STATIC ${CMAKE_CURRENT_SOURCE_DIR}/ringbuf.c)
target_include_directories(${RINGBUF_LIBRARY} PUBLIC ${RINGBUF_INCLUDE_DIR})

add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/example/example.c)
target_link_libraries(example PRIVATE ${RINGBUF_LIBRARY})
set_property(TARGET example PROPERTY EXCLUDE_FROM_ALL TRUE)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/example/proserpine.txt
  ${CMAKE_BINARY_DIR}/proserpine.txt
  COPYONLY)

add_subdirectory(test)
