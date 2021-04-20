set(SOURCE_FILES
  coding_factory.cc
  null_coding.cc
  pfor_coding.cc
  rice_coding.cc
  rice_coding2.cc
  s9_coding.cc
  s16_coding.cc
  vbyte_coding.cc
  unpack.cc)

add_library(compression SHARED ${SOURCE_FILES})

target_compile_options(compression PRIVATE -W -Wall -O9)

target_include_directories(compression PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})