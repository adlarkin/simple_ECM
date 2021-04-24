# need version 3.12 for add_compile_definitions
# (https://cmake.org/cmake/help/latest/command/add_compile_definitions.html)
cmake_minimum_required(VERSION 3.12)

project(SimpleECM)

# c++17 is required in order to use std::apply (https://en.cppreference.com/w/cpp/utility/apply),
# and is also needed for EnTT
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compile with warnings if using GNU compiler
# (https://cmake.org/cmake/help/latest/command/add_compile_options.html)
# (https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -pedantic)
endif()

# see if dependencies are met to compile ign-gazebo ECM tests
find_package(ignition-gazebo5 QUIET)
if (ignition-gazebo5_FOUND)
  add_compile_definitions(_IGN_GAZEBO)
else()
  message(WARNING "Unable to find ign-gazebo5, so ign-gazebo ECM tests will be skipped")
endif()

# see if dependencies are met to compile EnTT tests
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/entt/src/")
  add_compile_definitions(_ENTT)
  set(ENTT_INCLUDE_DIRS "entt/src")
else()
  message(WARNING "EnTT not found, so EnTT tests will be skipped (enable the EnTT submodule to build EnTT tests)")
endif()

# executable for running a simple ECM demo
add_executable(ecm_demo
  src/main.cc
)
target_include_directories(ecm_demo
  PRIVATE include
)

# Library for running tests. This will always support at least the simple ECM
# tests, but will also support the ign-gazebo ECM and EnTT tests if the
# dependencies are met
add_library(TestLib
  INTERFACE
)
target_include_directories(TestLib
  INTERFACE ${ENTT_INCLUDE_DIRS} include test/include
)
if (ignition-gazebo5_FOUND)
  target_link_libraries(TestLib
    INTERFACE ignition-gazebo5::ignition-gazebo5
  )
endif()

# executable for running benchmark tests
add_executable(benchmark_test
  test/each_benchmark.cc
)
target_link_libraries(benchmark_test
  TestLib
)

# executable for running memory tests
add_executable(memory_test
  test/memory_usage.cc
)
target_link_libraries(memory_test
  TestLib
)
