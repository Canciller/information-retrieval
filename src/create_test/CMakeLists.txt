set(SOURCE_FILES
  main.cpp)

add_executable(create_test ${SOURCE_FILES})

target_compile_options(create_test PRIVATE -W -Wall)

target_link_libraries(create_test compression)

target_include_directories(create_test PUBLIC
  ${PROJECT_SOURCE_DIR}/include)

set_target_properties(create_test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)