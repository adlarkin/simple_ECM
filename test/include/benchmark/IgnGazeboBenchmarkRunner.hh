#ifndef IGN_GAZEBO_BENCHMARK_RUNNER_HH_
#define IGN_GAZEBO_BENCHMARK_RUNNER_HH_

#include <vector>

#include <ignition/gazebo/Entity.hh>
#include <ignition/gazebo/EntityComponentManager.hh>
#include <ignition/gazebo/components/AngularVelocity.hh>
#include <ignition/gazebo/components/LinearAcceleration.hh>
#include <ignition/gazebo/components/LinearVelocity.hh>
#include <ignition/gazebo/components/Name.hh>
#include <ignition/gazebo/components/Pose.hh>
#include <ignition/gazebo/components/Static.hh>

#include "benchmark/BenchmarkRunner.hh"

class IgnGazeboBenchmarkRunner : public BenchmarkRunner
{
  /// \brief Documentation inherited
  public: void Init(const std::size_t _numEntitiesToModify) final;

  /// \brief Documentation inherited
  public: void MakeEntityWithComponents() final;

  /// \brief Documentation inherited
  public: void EachImplementation() final;

  /// \brief Documentation inherited
  public: void RemoveAComponent() final;

  /// \brief Documentation inherited
  public: void AddAComponent() final;

  /// \brief ECM
  private: ignition::gazebo::EntityComponentManager ecm;

  /// \brief Keep track of the entities that should have a component removed
  /// or added
  private: std::vector<ignition::gazebo::Entity> entitiesToModify;
};

void IgnGazeboBenchmarkRunner::Init(const std::size_t _numEntitiesToModify)
{
  this->numEntitiesToModify = _numEntitiesToModify;
}

void IgnGazeboBenchmarkRunner::MakeEntityWithComponents()
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

  if (this->entitiesToModify.size() < this->numEntitiesToModify)
    this->entitiesToModify.push_back(entity);
}

void IgnGazeboBenchmarkRunner::EachImplementation()
{
  using namespace ignition::gazebo;

  this->entityCount = 0;
  this->ecm.Each<components::Name,
                 components::Static,
                 components::LinearVelocity,
                 components::WorldLinearVelocity,
                 components::AngularVelocity,
                 components::WorldAngularVelocity,
                 components::LinearAcceleration,
                 components::WorldLinearAcceleration,
                 components::Pose,
                 components::WorldPose>(
    [this](const ignition::gazebo::Entity &, components::Name *,
      components::Static *, components::LinearVelocity *,
      components::WorldLinearVelocity *, components::AngularVelocity *,
      components::WorldAngularVelocity *, components::LinearAcceleration *,
      components::WorldLinearAcceleration *, components::Pose *,
      components::WorldPose *)
    {
      this->entityCount++;
      return true;
    });
}

void IgnGazeboBenchmarkRunner::RemoveAComponent()
{
  for (auto &entity : this->entitiesToModify)
  {
    this->ecm.RemoveComponent<ignition::gazebo::components::LinearVelocity>(
        entity);
  }
}

void IgnGazeboBenchmarkRunner::AddAComponent()
{
  for (auto &entity : this->entitiesToModify)
  {
    this->ecm.CreateComponent(entity,
        ignition::gazebo::components::LinearVelocity());
  }
}

#endif
