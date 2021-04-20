#ifndef COMPONENTS_HPP_
#define COMPONENTS_HPP_

#include <ostream>

#include "Types.hh"

struct BaseComponent
{
  friend std::ostream &operator<<(std::ostream &_os,
      const BaseComponent &_comp);

  int x{0};
  int y{0};
  int z{0};

  constexpr const static ComponentTypeId typeId{kInvalidComponent};
};

std::ostream &operator<<(std::ostream &_os, const BaseComponent &_comp)
{
  _os << _comp.x << " " << _comp.y << " " << _comp.z;
  return _os;
}

struct Position : public BaseComponent
{
  constexpr const static ComponentTypeId typeId{1};
};

struct LinearVelocity : public BaseComponent
{
  constexpr const static ComponentTypeId typeId{2};
};

struct LinearAcceleration : public BaseComponent
{
  constexpr const static ComponentTypeId typeId{3};
};

#endif
