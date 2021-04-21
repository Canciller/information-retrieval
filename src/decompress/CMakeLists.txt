set(SOURCE_FILES
  main.cpp)

add_executable(decompress ${SOURCE_FILES})

target_compile_options(decompress PRIVATE -W -Wall)

target_link_libraries(decompress compression)

target_include_directories(decompress PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(decompress PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)