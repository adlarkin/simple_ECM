#include <functional>
#include <iostream>
#include <vector>

#include "simpleECM/Components.hh"
#include "simpleECM/Ecm.hh"
#include "simpleECM/Types.hh"

int main()
{
  std::vector<Entity> entities;

  ECM ecm;

  // create a few entities that have components
  for (auto i = 0; i < 3; ++i)
  {
    auto entity = ecm.CreateEntity();
    entities.push_back(entity);

    Position position;
    position.data.x = i;
    position.data.y = i;
    position.data.z = i;
    ecm.AddComponent<Position>(entity, position);

    LinearVelocity linVel;
    linVel.data.x = i;
    linVel.data.y = i;
    linVel.data.z = i;
    ecm.AddComponent<LinearVelocity>(entity, linVel);

    LinearAcceleration linAccel;
    linAccel.data.x = i;
    linAccel.data.y = i;
    linAccel.data.z = i;
    ecm.AddComponent<LinearAcceleration>(entity, linAccel);
  }

  // create several different callback functions that either read or update
  // component data. Using different component combinations across various
  // callbacks enables the usage of different views
  std::function<bool(const Entity &, Position *, LinearVelocity *,
      LinearAcceleration *)> printAllComponents =
    [](const Entity &_entity, Position *_position, LinearVelocity *_linVel,
        LinearAcceleration *_linAccel) -> bool
    {
      std::cout << "Entity " << _entity << " has components:"
        << "\n\tPosition \t\t" << *_position
        << "\n\tLinear Velocity \t" << *_linVel
        << "\n\tLinear Acceleration \t" << *_linAccel << std::endl;
      return true;
    };

  std::function<bool(const Entity &, Position *)> updatePosition =
    [](const Entity &, Position *_position) -> bool
    {
      _position->data.x++;
      _position->data.y++;
      _position->data.z++;
      return true;
    };

  std::function<bool(const Entity &, LinearVelocity *)> updateLinVel =
    [](const Entity &, LinearVelocity *_linVel) -> bool
    {
      _linVel->data.x++;
      _linVel->data.y++;
      _linVel->data.z++;
      return true;
    };

  std::function<bool(const Entity &, LinearAcceleration *)> updateLinAccel =
    [](const Entity &, LinearAcceleration *_linAccel) -> bool
    {
      _linAccel->data.x++;
      _linAccel->data.y++;
      _linAccel->data.z++;
      return true;
    };

  std::function<bool(const Entity &, Position *, LinearVelocity *)> posLinVel =
    [](const Entity &_entity, Position *, LinearVelocity *) -> bool
    {
      std::cout << "Entity " << _entity
        << " has a position and linear velocity component" << std::endl;
      return true;
    };

  // this callback function does the same thing as printAllComponents,
  // but requests the entities in a different order and therefore uses a
  // different view
  std::function<bool(const Entity &, LinearVelocity *, Position *,
      LinearAcceleration *)> printAllUpdatedComponents =
    [](const Entity &_entity, LinearVelocity *_linVel, Position *_position,
        LinearAcceleration *_linAccel) -> bool
    {
      std::cout << "Entity " << _entity << " has updated components:"
        << "\n\tPosition \t\t" << *_position
        << "\n\tLinear Velocity \t" << *_linVel
        << "\n\tLinear Acceleration \t" << *_linAccel << std::endl;
      return true;
    };

  // apply the callbacks to the ECM via ECM::Each
  ecm.Each(printAllComponents);
  std::cout << std::endl << "-----" << std::endl << std::endl
    << "Updating component data..." << std::endl;
  ecm.Each(updatePosition);
  ecm.Each(updateLinVel);
  ecm.Each(updateLinAccel);
  std::cout << "Done updating component data" << std::endl << std::endl
    << "-----" << std::endl << std::endl;
  ecm.Each(printAllUpdatedComponents);

  // remove components from the first entity to see how views are updated
  std::cout << std::endl << "-----" << std::endl << std::endl
    << "Removing the following components:" << std::endl
    << "\t- position component from Entity " << entities[0] << std::endl
    << "\t- linear velocity component from Entity " << entities[1] << std::endl
    << "\t- linear acceleration component from Entity " << entities[2]
    << std::endl;
  ecm.RemoveComponent<Position>(entities[0]);
  ecm.RemoveComponent<LinearVelocity>(entities[1]);
  ecm.RemoveComponent<LinearAcceleration>(entities[2]);
  std::cout << "Done removing the components" << std::endl << std::endl
    << "-----" << std::endl << std::endl;
  std::cout << "The following entities have position, linear velocity, and "
    << "linear acceleration components:" << std::endl;
  ecm.Each(printAllComponents);
  std::cout << std::endl << "-----" << std::endl << std::endl;
  ecm.Each(posLinVel);

  // add components back in and check the views again
  std::cout << std::endl << "-----" << std::endl << std::endl
    << "Re-creating the components that were just removed..." << std::endl;
  ecm.AddComponent<Position>(entities[0], Position());
  ecm.AddComponent<LinearVelocity>(entities[1], LinearVelocity());
  ecm.AddComponent<LinearAcceleration>(entities[2], LinearAcceleration());
  std::cout << "Done adding back in the components" << std::endl << std::endl
    << "-----" << std::endl << std::endl;
  ecm.Each(printAllComponents);

  // create a brand new component to make sure it's added to the appropriate
  // views
  std::cout << std::endl << "-----" << std::endl << std::endl
    << "Creating a new entity with position and linear velocity components..."
    << std::endl;
  auto entity = ecm.CreateEntity();
  entities.push_back(entity);
  ecm.AddComponent<Position>(entity, Position());
  ecm.AddComponent<LinearVelocity>(entity, LinearVelocity());
  std::cout << "Done creating the new entity" << std::endl << std::endl
    << "-----" << std::endl << std::endl;
  ecm.Each(posLinVel);

  // verify that the ECM has 6 views (6 callback functions were used, with each
  // CB having a unique order of component types in the method signature)
  std::cout << std::endl << "-----" << std::endl << std::endl
    << "The ECM has " << ecm.ViewCount() << " views" << std::endl;

  return 0;
}
