set(SOURCE_FILES
  golomb.cpp)

add_library(golomb SHARED ${SOURCE_FILES})

target_compile_options(golomb PRIVATE -W -Wall)

target_link_libraries(golomb z m)

target_include_directories(golomb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})