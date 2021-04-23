#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "benchmark/BenchmarkRunner.hh"
#include "benchmark/BenchmarkRunnerFactory.hh"

int main(int argc, char **argv)
{
  // users may specify the number of entities they'd like to spawn from the
  // command line
  int numEntities = 100;
  if (argc == 2)
  {
    numEntities = std::stoi(argv[1]);
  }

  // all of the ECM implementations that will be benchmarked
  std::vector<std::string> implementationTypes{"simpleECM"};
#ifdef _ENTT
  implementationTypes.push_back("entt group");
  implementationTypes.push_back("entt view");
#endif // _ENTT
#ifdef _IGN_GAZEBO
  implementationTypes.push_back("ignGazebo");
#endif // _IGN_GAZEBO

  // creating numEntites entities with the following components (10 total):
  //  name
  //  static
  //  linear velocity
  //  world linear velocity
  //  angular velocity
  //  world angular velocity
  //  linear acceleration
  //  world linear acceleration
  //  pose
  //  world pose
  const int numComponents = 10;
  std::cout << "Creating an ECM with " << numEntities << " entities, with "
    << numComponents << " components per entity" << std::endl;

  // the number of times we will call Each(...) on the ECM
  const int numEachCalls = 3;
  std::cout << "Calling Each(...) " << numEachCalls << " times, searching for "
    << numEntities << " entities with " << numComponents
    << " components in every Each(...) call" << std::endl << std::endl;

  for (std::size_t typeIdx = 0; typeIdx < implementationTypes.size(); ++typeIdx)
  {
    const auto &ecmType = implementationTypes[typeIdx];
    auto benchmarkRunner = BenchmarkRunnerFactory::Create(ecmType);
    if (!benchmarkRunner)
      continue;
    std::cout << ecmType << " implementation" << std::endl << std::endl;

    // instantiate the ecm and populate it with entities/components
    benchmarkRunner->Init();
    for (auto i = 0; i < numEntities; ++i)
      benchmarkRunner->MakeEntityWithComponents();

    // call Each(...) a few times and see how long it takes to find the entities
    // with all of the defined components (this first Each(...) call should take
    // the longest since it has to create the view - once the view is created,
    // subsequent Each(...) calls should be noticeably faster)
    for (auto i = 0; i < numEachCalls; ++i)
    {
      benchmarkRunner->StartTimer();
      benchmarkRunner->EachImplementation();
      benchmarkRunner->StopTimer();
      if (benchmarkRunner->Valid(numEntities))
        benchmarkRunner->DisplayElapsedTime();
    }

    if (typeIdx != implementationTypes.size() - 1)
      std::cout << std::endl << "-----" << std::endl << std::endl;

    delete benchmarkRunner;
    benchmarkRunner = nullptr;
  }

  return 0;
}
