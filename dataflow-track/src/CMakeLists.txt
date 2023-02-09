add_executable(dataflow-test main.cpp)

target_include_directories(dataflow-test
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(dataflow-test dataflow)
