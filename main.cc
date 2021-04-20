#include "Components.hpp"
#include "Ecm.hpp"

#include <functional>
#include <iostream>

int main()
{
  ECM ecm;

  // create a few entities that have components
  for (auto i = 0; i < 3; ++i)
  {
    auto entity = ecm.CreateEntity();

    Position position;
    position.x = i;
    position.y = i;
    position.z = i;
    ecm.AddComponent<Position>(entity, position);

    LinearVelocity linVel;
    linVel.x = i;
    linVel.y = i;
    linVel.z = i;
    ecm.AddComponent<LinearVelocity>(entity, linVel);

    LinearAcceleration linAccel;
    linAccel.x = i;
    linAccel.y = i;
    linAccel.z = i;
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
    [](const Entity &_entity, Position *_position) -> bool
    {
      _position->x++;
      _position->y++;
      _position->z++;
      return true;
    };

  std::function<bool(const Entity &, LinearVelocity *)> updateLinVel =
    [](const Entity &_entity, LinearVelocity *_linVel) -> bool
    {
      _linVel->x++;
      _linVel->y++;
      _linVel->z++;
      return true;
    };

  std::function<bool(const Entity &, LinearAcceleration *)> updateLinAccel =
    [](const Entity &_entity, LinearAcceleration *_linAccel) -> bool
    {
      _linAccel->x++;
      _linAccel->y++;
      _linAccel->z++;
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

  // verify that the ECM has 5 views (5 callback functions were used, with each
  // CB having a unique order of component types in the method signature)
  std::cout << std::endl << "-----" << std::endl << std::endl
    << "The ECM has " << ecm.ViewCount() << " views" << std::endl;

  return 0;
}
