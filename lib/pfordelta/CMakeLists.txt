set(SOURCE_FILES
  pack.cpp
  unpack.cpp
  coding_policy.cpp
  pfordelta.cpp)

add_library(pfordelta SHARED ${SOURCE_FILES})

target_compile_options(pfordelta PRIVATE -W -Wall -O9)

target_include_directories(pfordelta PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})