#ifndef SIMPLE_ECM_MEMORY_RUNNER_HH_
#define SIMPLE_ECM_MEMORY_RUNNER_HH_

#include <functional>

#include "simpleECM/Components.hh"
#include "simpleECM/Ecm.hh"
#include "memory/MemoryRunner.hh"

class SimpleECMMemoryRunner : public MemoryRunner
{
  /// \brief Documentation inherited
  public: void MakeEntityWithComponents() final;

  /// \brief Documentation inherited
  public: void Run() final;

  /// \brief The ECM that is being tested for memory usage
  private: ECM simpleEcm;
};

void SimpleECMMemoryRunner::MakeEntityWithComponents()
{
  const auto entity = this->simpleEcm.CreateEntity();
  this->simpleEcm.AddComponent(entity, Name());
  this->simpleEcm.AddComponent(entity, Static());
  this->simpleEcm.AddComponent(entity, LinearVelocity());
  this->simpleEcm.AddComponent(entity, WorldLinearVelocity());
  this->simpleEcm.AddComponent(entity, AngularVelocity());
  this->simpleEcm.AddComponent(entity, WorldAngularVelocity());
  this->simpleEcm.AddComponent(entity, LinearAcceleration());
  this->simpleEcm.AddComponent(entity, WorldLinearAcceleration());
  this->simpleEcm.AddComponent(entity, Pose());
  this->simpleEcm.AddComponent(entity, WorldPose());
}

void SimpleECMMemoryRunner::Run()
{
  // create callbacks covering all components that can be used with Each(...)
  // (the component ordering in each callback is unique, so each callback
  // should cause the creation of a new view)
  std::function<bool(const Entity &, Name *)> NameOnly =
    [](const Entity &, Name *) { return true; };

  std::function<bool(const Entity &, Name *, Static *)> NameStatic =
    [](const Entity &, Name *, Static *) { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *)>
    NameStaticLinVel =
    [](const Entity &, Name *, Static *, LinearVelocity *) { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *)> NameStaticLinVelWorld =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *) { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *, AngularVelocity *)> NameStaticLinAngVel =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *) { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *)>
    NameStaticLinAngVelWorld =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *)
    { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
      LinearAcceleration *)> NameStaticLinAngVelAccel =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
        LinearAcceleration *) { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
      LinearAcceleration *, WorldLinearAcceleration *)>
    NameStaticLinAngVelAccelWorld =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
        LinearAcceleration *, WorldLinearAcceleration *) { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
      LinearAcceleration *, WorldLinearAcceleration *, Pose *)>
    NameStaticLinAngVelAccelPose =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
        LinearAcceleration *, WorldLinearAcceleration *, Pose *)
    { return true; };

  std::function<bool(const Entity &, Name *, Static *, LinearVelocity *,
      WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
      LinearAcceleration *, WorldLinearAcceleration *, Pose *, WorldPose *)>
    NameStaticLinAngVelAccelPoseWorld =
    [](const Entity &, Name *, Static *, LinearVelocity *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
        LinearAcceleration *, WorldLinearAcceleration *, Pose *, WorldPose *)
    { return true; };

  // make a few callbacks that have the same components as callbacks that were
  // defined above, but change the component order (this should create more
  // views even though the components are the same since the simpleECM view
  // is defined by the components used and the order of them)
  std::function<bool(const Entity &, Static *, Name *)> StaticName =
    [](const Entity &, Static *, Name *) { return true; };

  std::function<bool(const Entity &, Static *, Name *, LinearVelocity *)>
    StaticNameLinVel =
    [](const Entity &, Static *, Name *, LinearVelocity *) { return true; };

  std::function<bool(const Entity &, Name *, LinearVelocity *, Static *)>
    NameLinVelStatic =
    [](const Entity &, Name *, LinearVelocity *, Static *) { return true; };

  std::function<bool(const Entity &, Static *, LinearVelocity *, Name *,
      WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
      LinearAcceleration *, WorldLinearAcceleration *, Pose *, WorldPose *)>
    StaticLinAngVelAccelPoseWorldName =
    [](const Entity &, Static *, LinearVelocity *, Name *,
        WorldLinearVelocity *, AngularVelocity *, WorldAngularVelocity *,
        LinearAcceleration *, WorldLinearAcceleration *, Pose *, WorldPose *)
    { return true; };

  this->simpleEcm.Each(NameOnly);
  this->simpleEcm.Each(NameStatic);
  this->simpleEcm.Each(NameStaticLinVel);
  this->simpleEcm.Each(NameStaticLinVelWorld);
  this->simpleEcm.Each(NameStaticLinAngVel);
  this->simpleEcm.Each(NameStaticLinAngVelWorld);
  this->simpleEcm.Each(NameStaticLinAngVelAccel);
  this->simpleEcm.Each(NameStaticLinAngVelAccelWorld);
  this->simpleEcm.Each(NameStaticLinAngVelAccelPose);
  this->simpleEcm.Each(NameStaticLinAngVelAccelPoseWorld);
  this->simpleEcm.Each(StaticName);
  this->simpleEcm.Each(StaticNameLinVel);
  this->simpleEcm.Each(NameLinVelStatic);
  this->simpleEcm.Each(StaticLinAngVelAccelPoseWorldName);

  const int expectedNumViews = 14;
  if (this->simpleEcm.ViewCount() != expectedNumViews)
  {
    std::cerr << "Internal error: expected " << expectedNumViews
      << " views to be created, but " << this->simpleEcm.ViewCount()
      << " views were created instead" << std::endl;
  }
}

#endif
