#ifndef SIMPLE_ECM_BENCHMARK_RUNNER_HH_
#define SIMPLE_ECM_BENCHMARK_RUNNER_HH_

#include <functional>
#include <vector>

#include "benchmark/BenchmarkRunner.hh"
#include "simpleECM/Components.hh"
#include "simpleECM/Ecm.hh"
#include "simpleECM/Types.hh"

class SimpleECMBenchmarkRunner : public BenchmarkRunner
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

  /// \brief The ECM that is being benchmarked
  private: ECM simpleEcm;

  /// \brief The callback function signature used for the ECM's Each(...) call
  private: using AllComponentEachFunc =
            std::function<bool(const Entity &,
                               Name *,
                               Static *,
                               LinearVelocity *,
                               WorldLinearVelocity *,
                               AngularVelocity *,
                               WorldAngularVelocity *,
                               LinearAcceleration *,
                               WorldLinearAcceleration *,
                               Pose *,
                               WorldPose *)>;

  /// \brief Callback function that is used in EachImplementation
  private: AllComponentEachFunc findAllComponents;

  /// \brief Keep track of the entities that should have a component removed
  /// or added
  private: std::vector<Entity> entitiesToModify;
};

void SimpleECMBenchmarkRunner::Init(const std::size_t _numEntitiesToModify)
{
  this->numEntitiesToModify = _numEntitiesToModify;

  this->findAllComponents =
    [this](const Entity &,
           Name *,
           Static *,
           LinearVelocity *,
           WorldLinearVelocity *,
           AngularVelocity *,
           WorldAngularVelocity *,
           LinearAcceleration *,
           WorldLinearAcceleration *,
           Pose *,
           WorldPose *) -> bool
    {
      this->entityCount++;
      return true;
    };
}

void SimpleECMBenchmarkRunner::MakeEntityWithComponents()
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

  if (this->entitiesToModify.size() < this->numEntitiesToModify)
    this->entitiesToModify.push_back(entity);
}

void SimpleECMBenchmarkRunner::EachImplementation()
{
  this->entityCount = 0;
  this->simpleEcm.Each(this->findAllComponents);
}

void SimpleECMBenchmarkRunner::RemoveAComponent()
{
  for (auto &entity : this->entitiesToModify)
    this->simpleEcm.RemoveComponent<LinearVelocity>(entity);
}

void SimpleECMBenchmarkRunner::AddAComponent()
{
  for (auto &entity : this->entitiesToModify)
    this->simpleEcm.AddComponent(entity, LinearVelocity());
}

#endif
