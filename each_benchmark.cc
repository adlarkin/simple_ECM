#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_set>

#include "Components.hpp"
#include "Ecm.hpp"

int main(int argc, char **argv)
{
  // users may specify the number of entities they'd like to spawn from the
  // command line
  int numEntities = 100;
  if (argc == 2)
  {
    numEntities = std::stoi(argv[1]);
  }

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

  // instantiate the ecm and populate it with entities/components
  std::cout << "Creating an ECM with " << numEntities << " entities, with "
    << numComponents << " components per entity" << std::endl;
  ECM ecm;
  for (auto i = 0; i < numEntities; ++i)
  {
    auto entity = ecm.CreateEntity();

    Name nameComp;
    ecm.AddComponent(entity, nameComp);

    Static staticComp;
    ecm.AddComponent(entity, staticComp);

    LinearVelocity linVelComp;
    ecm.AddComponent(entity, linVelComp);

    WorldLinearVelocity worldLinVelComp;
    ecm.AddComponent(entity, worldLinVelComp);

    AngularVelocity angularVelComp;
    ecm.AddComponent(entity, angularVelComp);

    WorldAngularVelocity worldAngularVelComp;
    ecm.AddComponent(entity, worldAngularVelComp);

    LinearAcceleration linAccelComp;
    ecm.AddComponent(entity, linAccelComp);

    WorldLinearAcceleration worldLinAccelComp;
    ecm.AddComponent(entity, worldLinAccelComp);

    Pose poseComp;
    ecm.AddComponent(entity, poseComp);

    WorldPose worldPoseComp;
    ecm.AddComponent(entity, worldPoseComp);
  }

  std::unordered_set<Entity> entities;

  using AllComponentEachFunc = std::function<bool(const Entity &, Name *,
      Static *, LinearVelocity *, WorldLinearVelocity *, AngularVelocity *,
      WorldAngularVelocity *, LinearAcceleration *, WorldLinearAcceleration *,
      Pose *, WorldPose *)>;

  AllComponentEachFunc findAllComponents =
    [&entities](const Entity &_entity, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
        LinearAcceleration *, WorldLinearAcceleration *, Pose *, WorldPose *)
    -> bool
    {
      entities.insert(_entity);
      return true;
    };

  // call ecm.Each a few times and see how long it takes to find the entities
  // with all of the defined components (this first ecm.Each call should take
  // the longest since it has to create the view - once the view is created,
  // subsequent ecm.Each calls should be noticeably faster)
  int numTests = 3;
  std::cout << "Calling ECM::Each " << numTests << " times, searching for "
    << numEntities << " entities with " << numComponents
    << " components in every ECM::Each call" << std::endl;
  for (auto i = 0; i < numTests; ++i)
  {
    auto start = std::chrono::steady_clock::now();
    ecm.Each(findAllComponents);
    auto end = std::chrono::steady_clock::now();
    auto msDuration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (entities.size() != numEntities)
    {
      std::cerr << "Internal error: should have found " << numEntities
        << " entities, but " << entities.size()
        << " entities were found instead" << std::endl;
    }
    else
    {
      std::cout << "Attempt " << i << " took " << msDuration.count() << " ms"
          << std::endl;
    }

    entities.clear();
  }

  return 0;
}
