#ifndef IGN_GAZEBO_MEMORY_RUNNER_HH_
#define IGN_GAZEBO_MEMORY_RUNNER_HH_


#include <ignition/gazebo/Entity.hh>
#include <ignition/gazebo/EntityComponentManager.hh>
#include <ignition/gazebo/components/AngularVelocity.hh>
#include <ignition/gazebo/components/LinearAcceleration.hh>
#include <ignition/gazebo/components/LinearVelocity.hh>
#include <ignition/gazebo/components/Name.hh>
#include <ignition/gazebo/components/Pose.hh>
#include <ignition/gazebo/components/Static.hh>

#include "memory/MemoryRunner.hh"

class IgnGazeboMemoryRunner : public MemoryRunner
{
  /// \brief Documentation inherited
  public: void MakeEntityWithComponents() final;

  /// \brief Documentation inherited
  public: void Run() final;

  /// \brief The ECM that is being tested for memory usage
  private: ignition::gazebo::EntityComponentManager ecm;
};

void IgnGazeboMemoryRunner::MakeEntityWithComponents()
{
  using namespace ignition::gazebo;

  auto entity = this->ecm.CreateEntity();
  this->ecm.CreateComponent(entity, components::Name());
  this->ecm.CreateComponent(entity, components::Static());
  this->ecm.CreateComponent(entity, components::LinearVelocity());
  this->ecm.CreateComponent(entity, components::WorldLinearVelocity());
  this->ecm.CreateComponent(entity, components::AngularVelocity());
  this->ecm.CreateComponent(entity, components::WorldAngularVelocity());
  this->ecm.CreateComponent(entity, components::LinearAcceleration());
  this->ecm.CreateComponent(entity, components::WorldLinearAcceleration());
  this->ecm.CreateComponent(entity, components::Pose());
  this->ecm.CreateComponent(entity, components::WorldPose());
}

void IgnGazeboMemoryRunner::Run()
{
  using namespace ignition::gazebo;

  // call Each(...) using all components (10 combinations used here, so 10 views
  // should be created)
  this->ecm.Each<components::Name>(
      [](const ignition::gazebo::Entity &, components::Name *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity,
    components::AngularVelocity>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *, components::AngularVelocity *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity,
    components::AngularVelocity, components::WorldAngularVelocity>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *, components::AngularVelocity *,
        components::WorldAngularVelocity *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity,
    components::AngularVelocity, components::WorldAngularVelocity,
    components::LinearAcceleration>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *, components::AngularVelocity *,
        components::WorldAngularVelocity *, components::LinearAcceleration *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity,
    components::AngularVelocity, components::WorldAngularVelocity,
    components::LinearAcceleration, components::WorldLinearAcceleration>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *, components::AngularVelocity *,
        components::WorldAngularVelocity *, components::LinearAcceleration *,
        components::WorldLinearAcceleration *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity,
    components::AngularVelocity, components::WorldAngularVelocity,
    components::LinearAcceleration, components::WorldLinearAcceleration,
    components::Pose>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *, components::AngularVelocity *,
        components::WorldAngularVelocity *, components::LinearAcceleration *,
        components::WorldLinearAcceleration *, components::Pose *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::Static,
    components::LinearVelocity, components::WorldLinearVelocity,
    components::AngularVelocity, components::WorldAngularVelocity,
    components::LinearAcceleration, components::WorldLinearAcceleration,
    components::Pose, components::WorldPose>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::Static *, components::LinearVelocity *,
        components::WorldLinearVelocity *, components::AngularVelocity *,
        components::WorldAngularVelocity *, components::LinearAcceleration *,
        components::WorldLinearAcceleration *, components::Pose *,
        components::WorldPose *)
      {
        return true;
      });

  // make a few Each calls with the same component combinations above, but with
  // a different component order. For ign-gazebo5 at least, this should not
  // create any more views since component order does not define a view based on
  // the view implementation (a view is defined by the set of components,
  // independent of order)
  this->ecm.Each<components::Static, components::Name>(
      [](const ignition::gazebo::Entity &, components::Static *,
        components::Name *)
      {
        return true;
      });

  this->ecm.Each<components::Static, components::Name,
    components::LinearVelocity>(
      [](const ignition::gazebo::Entity &, components::Static *,
        components::Name *, components::LinearVelocity *)
      {
        return true;
      });

  this->ecm.Each<components::Name, components::LinearVelocity,
    components::Static>(
      [](const ignition::gazebo::Entity &, components::Name *,
        components::LinearVelocity *, components::Static *)
      {
        return true;
      });

  this->ecm.Each<components::Static, components::LinearVelocity,
    components::Name, components::WorldLinearVelocity,
    components::AngularVelocity, components::WorldAngularVelocity,
    components::LinearAcceleration, components::WorldLinearAcceleration,
    components::Pose, components::WorldPose>(
      [](const ignition::gazebo::Entity &, components::Static *,
        components::LinearVelocity *, components::Name *,
        components::WorldLinearVelocity *, components::AngularVelocity *,
        components::WorldAngularVelocity *, components::LinearAcceleration *,
        components::WorldLinearAcceleration *, components::Pose *,
        components::WorldPose *)
      {
        return true;
      });
}

#endif
