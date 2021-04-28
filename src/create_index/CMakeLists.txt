set(SOURCE_FILES
  main.cpp)

add_executable(create_index ${SOURCE_FILES})

target_compile_options(create_index PRIVATE -W -Wall)

target_link_libraries(create_index compression)

target_include_directories(create_index PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(create_index PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)