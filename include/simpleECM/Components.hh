#ifndef COMPONENTS_HH_
#define COMPONENTS_HH_

#include <ostream>
#include <string>

#include "simpleECM/Types.hh"

/// \brief A base component type, which should be inherited by other components
struct BaseComponent
{
  private: virtual std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << "This is a base component that needs to be inherited!";
    return _os;
  }

  public: friend std::ostream &operator<<(std::ostream &_os,
              const BaseComponent &_comp)
  {
    return _comp.ToOStream(_os);
  }

  public: constexpr const static ComponentTypeId typeId{kInvalidComponent};
};

/// \brief A component that contains the name of the entity
struct Name : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << this->name;
    return _os;
  }

  public: std::string name;
  public: constexpr const static ComponentTypeId typeId{1};
};

/// \brief A component that identifies an entity as a world
struct World : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << "This is a world";
    return _os;
  }

  public: constexpr const static ComponentTypeId typeId{2};
};

/// \brief A component that defines whether an entity is static or not
struct Static : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    if (isStatic)
      _os << "static";
    else
      _os << "not static";
    return _os;
  }

  public: bool isStatic;
  public: constexpr const static ComponentTypeId typeId{3};
};

/// \brief A component representing an entity's position
struct Position : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << this->data;
    return _os;
  }

  public: Vector3i data;
  public: constexpr const static ComponentTypeId typeId{4};
};

/// \brief A component representing an entity's position in world coordinates
struct WorldPosition : public Position
{
  public: constexpr const static ComponentTypeId typeId{5};
};

/// \brief A component representing an entity's linear velocity
struct LinearVelocity : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << this->data;
    return _os;
  }

  public: Vector3i data;
  public: constexpr const static ComponentTypeId typeId{6};
};

/// \brief A component representing an entity's linear velocity in world
/// coordinates
struct WorldLinearVelocity : public LinearVelocity
{
  public: constexpr const static ComponentTypeId typeId{7};
};

/// \brief A component representing an entity's angular velocity
struct AngularVelocity : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << this->data;
    return _os;
  }

  public: Vector3i data;
  public: constexpr const static ComponentTypeId typeId{8};
};

/// \brief A component representing an entity's angular velocity in world
/// coordinates
struct WorldAngularVelocity : public AngularVelocity
{
  public: constexpr const static ComponentTypeId typeId{9};
};

/// \brief A component representing an entity's linear acceleration
struct LinearAcceleration : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << this->data;
    return _os;
  }

  public: Vector3i data;
  public: constexpr const static ComponentTypeId typeId{10};
};

/// \brief A component representing an entity's linear acceleration in world
/// coordinates
struct WorldLinearAcceleration : public LinearAcceleration
{
  public: constexpr const static ComponentTypeId typeId{11};
};

/// \brief A component representing an entity's pose
struct Pose : public BaseComponent
{
  private: std::ostream &ToOStream(std::ostream &_os) const
  {
    _os << this->position << " " << this->orientation;
    return _os;
  }

  public: Vector3i position;
  public: Quaternioni orientation;
  public: constexpr const static ComponentTypeId typeId{12};
};

/// \brief A component representing an entity's pose in world coordinates
struct WorldPose : public Pose
{
  public: constexpr const static ComponentTypeId typeId{13};
};

#endif
