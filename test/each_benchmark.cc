#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "benchmark/BenchmarkRunner.hh"
#include "benchmark/BenchmarkRunnerFactory.hh"

int main(int argc, char **argv)
{
  // command line arguments are as follows:
  //  * the number of entities to create (required)
  //  * the number of entities that should have a component added/removed
  //    in between Each calls (optional). If this argument is not specified,
  //    no components will be added/removed from entities between Each calls
  int numEntitiesCreated = 0;
  int numEntitiesAddRemoveComp = 0;
  bool addAndRemoveComps = false;
  if (argc >= 2)
  {
    numEntitiesCreated = std::stoi(argv[1]);
    if (argc == 3)
    {
      numEntitiesAddRemoveComp = std::stoi(argv[2]);
      addAndRemoveComps = true;
      if (numEntitiesAddRemoveComp > numEntitiesCreated)
      {
        std::cerr << numEntitiesCreated
          << " entities are requested to be created, but "
          << numEntitiesAddRemoveComp
          << " entities should have components added/removed."
          << std::endl << "This isn't possible!" << std::endl;
        return -1;
      }
    }
  }
  else
  {
    const std::string entityCreationStr = "<# of entities to create>";
    const std::string entityAddRemoveCompStr =
      "[# of entities to add/remove components]";
    std::cerr << "Usage: " << argv[0] << entityCreationStr << " "
      << entityAddRemoveCompStr << std::endl << std::endl
      << entityAddRemoveCompStr << " should be <= than " << entityCreationStr
      << std::endl;
    return -1;
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
  std::cout << "Creating an ECM with " << numEntitiesCreated << " entities, with "
    << numComponents << " components per entity" << std::endl;

  // the number of times we will call Each(...) on the ECM
  const int numEachCalls = 3;

  for (std::size_t typeIdx = 0; typeIdx < implementationTypes.size(); ++typeIdx)
  {
    const auto &ecmType = implementationTypes[typeIdx];
    auto benchmarkRunner = BenchmarkRunnerFactory::Create(ecmType);
    if (!benchmarkRunner)
      continue;
    std::cout << std::endl << "-----" << std::endl << std::endl
      << ecmType << " implementation" << std::endl << std::endl;

    // instantiate the ecm and populate it with entities/components
    benchmarkRunner->Init(numEntitiesAddRemoveComp);
    for (auto i = 0; i < numEntitiesCreated; ++i)
      benchmarkRunner->MakeEntityWithComponents();

    // call Each(...) a few times and see how long it takes to find the entities
    // with all of the defined components (this first Each(...) call should take
    // the longest since it has to create the view - once the view is created,
    // subsequent Each(...) calls should be noticeably faster)
    std::cout << "Calling Each(...) " << numEachCalls  << " times on "
      << numEntitiesCreated << " created entities, with " << numComponents
      << " components per entity" << std::endl;
    for (auto i = 0; i < numEachCalls; ++i)
    {
      benchmarkRunner->StartTimer();
      benchmarkRunner->EachImplementation();
      benchmarkRunner->StopTimer();
      if (benchmarkRunner->Valid(numEntitiesCreated))
        benchmarkRunner->DisplayElapsedTime();
    }

    if (addAndRemoveComps)
    {
      std::cout << std::endl;

      // remove components from entities, and then call Each(...) a few times
      benchmarkRunner->StartTimer();
      benchmarkRunner->RemoveAComponent();
      benchmarkRunner->StopTimer();
      benchmarkRunner->DisplayElapsedTime("Removing a component from "
          + std::to_string(numEntitiesAddRemoveComp) + " entities took ");
      std::cout << "Calling Each(...) " << numEachCalls <<
        " times on the entities that didn't have a component removed:"
        << std::endl;
      for (auto i = 0; i < numEachCalls; ++i)
      {
        benchmarkRunner->StartTimer();
        benchmarkRunner->EachImplementation();
        benchmarkRunner->StopTimer();
        if (benchmarkRunner->Valid(
              numEntitiesCreated - numEntitiesAddRemoveComp))
          benchmarkRunner->DisplayElapsedTime();
      }
      std::cout << std::endl;

      // add components to entities, and then call Each(...) a few times
      benchmarkRunner->StartTimer();
      benchmarkRunner->AddAComponent();
      benchmarkRunner->StopTimer();
      benchmarkRunner->DisplayElapsedTime("Adding a component to "
          + std::to_string(numEntitiesAddRemoveComp) + " entities took ");
      std::cout << "Calling Each(...) " << numEachCalls <<
        " times on the entities that have all components:" << std::endl;
      for (auto i = 0; i < numEachCalls; ++i)
      {
        benchmarkRunner->StartTimer();
        benchmarkRunner->EachImplementation();
        benchmarkRunner->StopTimer();
        if (benchmarkRunner->Valid(numEntitiesCreated))
          benchmarkRunner->DisplayElapsedTime();
      }
    }

    delete benchmarkRunner;
    benchmarkRunner = nullptr;
  }

  return 0;
}
