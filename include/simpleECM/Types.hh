#ifndef TYPES_HH_
#define TYPES_HH_

#include <cstdint>
#include <ostream>

/// \brief An entity, which can have 0 or more components
using Entity = std::uint64_t;

/// \brief An identifier that specifies a component type
using ComponentTypeId = std::uint64_t;

/// \brief An invalid component type
const ComponentTypeId kInvalidComponent{0};

/// \brief A 3D vector (can be used to store data in components)
template<typename T>
struct Vector3
{
  friend std::ostream &operator<<(std::ostream &_os, const Vector3<T> &_vec)
  {
    _os << _vec.x << " " << _vec.y << " " << _vec.z;
    return _os;
  }

  T x;
  T y;
  T z;
};

/// \brief A 3D vector of integers
typedef Vector3<int> Vector3i;

/// \brief A quaternion (can be used to store data in components)
template<typename T>
struct Quaternion
{
  friend std::ostream &operator<<(std::ostream &_os, const Quaternion<T> &_q)
  {
    _os << _q.w << " " << _q.x << " " << _q.y << " " << _q.z;
    return _os;
  }

  T w;
  T x;
  T y;
  T z;
};

/// \brief A quaternion of integers
typedef Quaternion<int> Quaternioni;

#endif
