# need version 3.12 for add_compile_definitions command
cmake_minimum_required(VERSION 3.12)

project(SimpleECM)

# c++17 is required in order to use std::apply (https://en.cppreference.com/w/cpp/utility/apply),
# and is also needed for ENTT
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compile with warnings if using GNU compiler
# (https://cmake.org/cmake/help/latest/command/add_compile_options.html)
# (https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -pedantic)
endif()

# see if dependencies are met to compile ign-gazebo ECM benchmark tests
find_package(ignition-gazebo5 QUIET)
if (ignition-gazebo5_FOUND)
  add_compile_definitions(_IGN_GAZEBO)
else()
  message(WARNING "Unable to find ign-gazebo5, so ign-gazebo ECM benchmark tests will be skipped")
endif()

# see if dependencies are met to compile ENTT benchmark tests
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/entt/src/")
  add_compile_definitions(_ENTT)
  set(ENTT_INCLUDE_DIRS "entt/src")
else()
  message(WARNING "ENTT not found, so ENTT benchmark tests will be skipped (enable the ENTT submodule to build ENTT benchmark tests)")
endif()

# executable for running a simple ECM demo
add_executable(ecm_demo
  src/main.cc
)
target_include_directories(ecm_demo
  PRIVATE include
)

# Library for running benchmark tests. This will always support at least the
# simple ECM benchmark test, but will also support the ign-gazebo ECM and ENTT
# benchmark tests if the dependencies are met
add_library(BenchmarkRunnerLib
  INTERFACE
)
target_include_directories(BenchmarkRunnerLib
  INTERFACE ${ENTT_INCLUDE_DIRS} include test/include
)
if (ignition-gazebo5_FOUND)
  target_link_libraries(BenchmarkRunnerLib
    INTERFACE ignition-gazebo5::ignition-gazebo5
  )
endif()

# executable for running benchmark tests
add_executable(benchmark_test
  test/each_benchmark.cc
)
target_include_directories(benchmark_test
  PRIVATE include
)
target_link_libraries(benchmark_test
  BenchmarkRunnerLib
)
