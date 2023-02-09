add_library(dataflow STATIC dataflow.cpp)

target_include_directories(dataflow PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(dataflow axp graphviz)
