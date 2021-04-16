#ifndef COMPONENTS_HPP_
#define COMPONENTS_HPP_

#include <ostream>

#include "Types.hh"

struct BaseComponent
{
  void Set(const int _x, const int _y, const int _z)
  {
    this->x = _x;
    this->y = _y;
    this->z = _z;
  }

  std::ostream &operator<<(std::ostream& os)
  {
    os << this->x << " " << this->y << " " << this->z;
    return os;
  }

  int x{0};
  int y{0};
  int z{0};

  const static ComponentTypeId typeId{kInvalidComponent};
};

struct Position : public BaseComponent
{
  const static ComponentTypeId typeId{1};
};

struct LinearVelocity : public BaseComponent
{
  const static ComponentTypeId typeId{2};
};

struct LinearAcceleration : public BaseComponent
{
  const static ComponentTypeId typeId{3};
};

#endif
