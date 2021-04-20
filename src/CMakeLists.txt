set(SOURCE_FILES
  main.cpp)

add_executable(ir ${SOURCE_FILES})

target_compile_options(ir PRIVATE -W -Wall)

target_link_libraries(ir compression)

target_include_directories(ir PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(ir PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)