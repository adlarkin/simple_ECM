#ifndef ENTT_GROUP_BENCHMARK_RUNNER_HH_
#define ENTT_GROUP_BENCHMARK_RUNNER_HH_

#include <entt/entity/registry.hpp>

#include "benchmark/BenchmarkRunner.hh"
#include "simpleECM/Components.hh"

class EnttGroupBenchmarkRunner : public BenchmarkRunner
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

void EnttGroupBenchmarkRunner::Init()
{
}

void EnttGroupBenchmarkRunner::MakeEntityWithComponents()
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

void EnttGroupBenchmarkRunner::EachImplementation()
{
  this->entityCount = 0;

  // using a full-owning group since the behavior for EnTT full-owning groups
  // is the closest match to the behavior of views in the simpleECM
  auto group = registry.group<Name, Static, LinearVelocity,
      WorldLinearVelocity, AngularVelocity, WorldAngularVelocity,
      LinearAcceleration, WorldLinearAcceleration, Pose, WorldPose>();
  group.each(
      [this](const auto /*_entity*/, auto &/*_static*/, auto &/*_linVel*/,
        auto &/*_worldLinVel*/, auto &/*_angularVel*/,
        auto &/*_worldAngularVel*/, auto &/*_linAccel*/,
        auto &/*_worldLinAccel*/, auto &/*_pose*/, auto &/*_worldPose*/)
      {
        this->entityCount++;
      });

  // iterators can also be used instead of the each(...) call above. Performance
  // doesn't seem to vary much with this call w.r.t. the each(...) call above
  /*
  for (auto &&[entity, name, staticComp, linVel, worldLinVel, angularVel,
      worldAngularVel, linAccel, worldLinAccel, pose, worldPose] : group.each())
  {
    this->entityCount++;
  }
  */

  // here is another way to use iterators - this approach actually seems slower
  // than the iteration approach above, so I think it's best to use this
  // approach if you need an entity that has a set of components, but you only
  // plan on using a subset of those components (if the number of component
  // queries in the loop body decreases, then performance improves)
  /*
  for (auto entity : group)
  {
    this->entityCount++;
    auto &name = group.get<Name>(entity);
    auto &staticComp = group.get<Static>(entity);
    auto &linVel = group.get<LinearVelocity>(entity);
    auto &worldLinVel = group.get<WorldLinearVelocity>(entity);
    auto &angularVel = group.get<AngularVelocity>(entity);
    auto &worldAngularVel = group.get<WorldAngularVelocity>(entity);
    auto &linAccel = group.get<LinearAcceleration>(entity);
    auto &worldLinAccel = group.get<WorldLinearAcceleration>(entity);
    auto &pose = group.get<Pose>(entity);
    auto &worldPose = group.get<WorldPose>(entity);
  }
  */
}

#endif
