set(SOURCE_FILES
  main.cpp)

add_executable(compress ${SOURCE_FILES})

target_compile_options(compress PRIVATE -W -Wall)

target_link_libraries(compress compression)

target_include_directories(compress PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(compress PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)