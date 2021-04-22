#ifndef BENCHMARK_RUNNER_FACTORY_HH_
#define BENCHMARK_RUNNER_FACTORY_HH_

#include <iostream>
#include <string>

#include "BenchmarkRunner.hh"
#ifdef _ENTT
  #include "EnttGroupBenchmarkRunner.hh"
  #include "EnttViewBenchmarkRunner.hh"
#endif // _ENTT
#ifdef _IGN_GAZEBO
  #include "IgnGazeboBenchmarkRunner.hh"
#endif // _IGN_GAZEBO
#include "SimpleECMBenchmarkRunner.hh"

struct BenchmarkRunnerFactory
{
  /// \brief Create a BenchmarkRunner of a specific type
  /// \param[in] _type The type of BenchmarkRunner to create
  /// \return A pointer to the created BenchmarkRunner. If _type does not
  /// correspond to a valid BenchmarkRunner type, nullptr is returned
  static BenchmarkRunner *Create(const std::string &_type)
  {
    if (_type == "simpleECM")
      return new SimpleECMBenchmarkRunner();
#ifdef _ENTT
    else if (_type == "entt view")
      return new EnttViewBenchmarkRunner();
    else if (_type == "entt group")
      return new EnttGroupBenchmarkRunner();
#endif // _ENTT
#ifdef _IGN_GAZEBO
    else if (_type == "ignGazebo")
      return new IgnGazeboBenchmarkRunner();
#endif // _IGN_GAZEBO

    std::cerr << "Invalid type given [" << _type
      << "], so no benchmark runner will be created " << std::endl;
    return nullptr;
  }
};

#endif
