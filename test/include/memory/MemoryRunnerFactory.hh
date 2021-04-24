#ifndef MEMORY_RUNNER_FACTORY_HH_
#define MEMORY_RUNNER_FACTORY_HH_

#include <iostream>
#include <string>

#include "memory/MemoryRunner.hh"
#ifdef _IGN_GAZEBO
  #include "memory/IgnGazeboMemoryRunner.hh"
#endif // _IGN_GAZEBO
#include "memory/SimpleECMMemoryRunner.hh"

struct MemoryRunnerFactory
{
  // simpleECM ignGazeboECM

  /// \brief Create a MemoryRunner of a specific type
  /// \param[in] _type The type of MemoryRunner to create
  /// \return A pointer to the created MemoryRunner. If _type does not
  /// correspond to a valid MemoryRunner type, nullptr is returned
  static MemoryRunner * Create(const std::string &_type)
  {
    if (_type == "simpleECM")
      return new SimpleECMMemoryRunner();
#ifdef _IGN_GAZEBO
    else if (_type == "ignGazeboECM")
      return new IgnGazeboMemoryRunner();
#endif // _IGN_GAZEBO

    std::cerr << "Invalid type given [" << _type
      << "], so no memory runner will be created " << std::endl;
    return nullptr;
  }
};

#endif
