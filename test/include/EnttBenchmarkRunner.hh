#ifndef ENTT_BENCHMARK_RUNNER_HH_
#define ENTT_BENCHMARK_RUNNER_HH_

#include <entt/entity/registry.hpp>

#include "BenchmarkRunner.hh"
#include "Components.hh"

class EnttBenchmarkRunner : public BenchmarkRunner
{
  /// \brief Documentation inherited
  public: void Init() final;

  /// \brief Documentation inherited
  public: void MakeEntityWithComponents() final;

  /// \brief Documentation inherited
  public: void EachImplementation() final;

  /// \brief Entt registry, which is like an ECM
  private: entt::registry registry;
};

void EnttBenchmarkRunner::Init()
{
}

void EnttBenchmarkRunner::MakeEntityWithComponents()
{
  const auto entity = registry.create();
  registry.emplace<Name>(entity);
  registry.emplace<Static>(entity);
  registry.emplace<LinearVelocity>(entity);
  registry.emplace<WorldLinearVelocity>(entity);
  registry.emplace<AngularVelocity>(entity);
  registry.emplace<WorldAngularVelocity>(entity);
  registry.emplace<LinearAcceleration>(entity);
  registry.emplace<WorldLinearAcceleration>(entity);
  registry.emplace<Pose>(entity);
  registry.emplace<WorldPose>(entity);
}

void EnttBenchmarkRunner::EachImplementation()
{
  this->entityCount = 0;
  auto view = registry.view<Name, Static, LinearVelocity, WorldLinearVelocity,
       AngularVelocity, WorldAngularVelocity, LinearAcceleration,
       WorldLinearAcceleration, Pose, WorldPose>();
  view.each(
      [this](const auto /*_entity*/, auto &/*_static*/, auto &/*_linVel*/,
        auto &/*_worldLinVel*/, auto &/*_angularVel*/,
        auto &/*_worldAngularVel*/, auto &/*_linAccel*/,
        auto &/*_worldLinAccel*/, auto &/*_pose*/, auto &/*_worldPose*/)
      {
        this->entityCount++;
      });
}

#endif
