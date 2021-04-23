#include <iostream>
#include <string>

#include "memory/MemoryRunner.hh"
#include "memory/MemoryRunnerFactory.hh"

int main(int argc, char **argv)
{
  // command line arguments are as follows:
  //  * the implementation to be memory tested (required). Must be one of:
  //    - simpleECM
  //    - ignGazeboECM
  //  * the number of entities to generate (optional). This should be a
  //    non-negative integer (default is 1000)
  std::string ecmImpl;
  int numEntities = 1000;
  if (argc >= 2)
  {
    ecmImpl = argv[1];
    if (argc == 3)
      numEntities = std::stoi(argv[2]);
  }
  else
  {
    std::cerr << "Usage: " << argv[0]
      << " <ECM implementation> [number of Entities]" << std::endl
      << "(<ECM implementation> should either be simpleECM or ignGazeboECM"
      << std::endl;
    return -1;
  }

  // load implementation and run memory test
  auto memoryRunner = MemoryRunnerFactory::Create(ecmImpl);
  if (!memoryRunner)
    return -1;
  std::cout << ecmImpl << " memory test for " << numEntities << " entities"
    << std::endl;
  for (auto i = 0; i < numEntities; ++i)
    memoryRunner->MakeEntityWithComponents();
  memoryRunner->Run();
  delete memoryRunner;
  memoryRunner = nullptr;

  return 0;
}
