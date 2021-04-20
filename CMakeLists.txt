cmake_minimum_required(VERSION 3.10)

project(SimpleECM)

# c++17 is required in order to use std::apply
# (https://en.cppreference.com/w/cpp/utility/apply)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ecm_demo
  main.cc
  Components.hpp
  Ecm.hpp
  Types.hh
  View.hpp
)
