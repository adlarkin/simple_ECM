#ifndef SIMPLE_ECM_BENCHMARK_RUNNER_HH_
#define SIMPLE_ECM_BENCHMARK_RUNNER_HH_

#include <functional>

#include "BenchmarkRunner.hh"
#include "Components.hh"
#include "Ecm.hh"

class SimpleECMBenchmarkRunner : public BenchmarkRunner
{
  /// \brief Documentation inherited
  public: void Init() final;

  /// \brief Documentation inherited
  public: void MakeEntityWithComponents() final;

  /// \brief Documentation inherited
  public: void EachImplementation() final;

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
};

void SimpleECMBenchmarkRunner::Init()
{
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
}

void SimpleECMBenchmarkRunner::EachImplementation()
{
  this->entityCount = 0;
  this->simpleEcm.Each(this->findAllComponents);
}

#endif
