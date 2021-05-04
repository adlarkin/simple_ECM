#ifndef ENTT_VIEW_BENCHMARK_RUNNER_HH_
#define ENTT_VIEW_BENCHMARK_RUNNER_HH_

#include <vector>

#include <entt/entity/registry.hpp>

#include "benchmark/BenchmarkRunner.hh"
#include "simpleECM/Components.hh"

class EnttViewBenchmarkRunner : public BenchmarkRunner
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

  /// \brief Entt registry, which is like an ECM
  private: entt::registry registry;

  /// \brief Keep track of the entities that should have a component removed or
  /// added
  private: std::vector<entt::entity> entitiesToModify;
};

void EnttViewBenchmarkRunner::Init(const std::size_t _numEntitiesToModify)
{
  this->numEntitiesToModify = _numEntitiesToModify;
}

void EnttViewBenchmarkRunner::MakeEntityWithComponents()
{
  const auto entity = this->registry.create();
  this->registry.emplace<Name>(entity);
  this->registry.emplace<Static>(entity);
  this->registry.emplace<LinearVelocity>(entity);
  this->registry.emplace<WorldLinearVelocity>(entity);
  this->registry.emplace<AngularVelocity>(entity);
  this->registry.emplace<WorldAngularVelocity>(entity);
  this->registry.emplace<LinearAcceleration>(entity);
  this->registry.emplace<WorldLinearAcceleration>(entity);
  this->registry.emplace<Pose>(entity);
  this->registry.emplace<WorldPose>(entity);

  if (this->entitiesToModify.size() < this->numEntitiesToModify)
    this->entitiesToModify.push_back(entity);
}

void EnttViewBenchmarkRunner::EachImplementation()
{
  this->entityCount = 0;
  auto view = this->registry.view<Name, Static, LinearVelocity,
       WorldLinearVelocity, AngularVelocity, WorldAngularVelocity,
       LinearAcceleration, WorldLinearAcceleration, Pose, WorldPose>();
  view.each(
      [this](const auto /*_entity*/, auto &/*_static*/, auto &/*_linVel*/,
        auto &/*_worldLinVel*/, auto &/*_angularVel*/,
        auto &/*_worldAngularVel*/, auto &/*_linAccel*/,
        auto &/*_worldLinAccel*/, auto &/*_pose*/, auto &/*_worldPose*/)
      {
        this->entityCount++;
      });
}

void EnttViewBenchmarkRunner::RemoveAComponent()
{
  for (auto &entity : this->entitiesToModify)
    this->registry.remove<LinearVelocity>(entity);
}

void EnttViewBenchmarkRunner::AddAComponent()
{
  for (auto &entity : this->entitiesToModify)
    this->registry.emplace<LinearVelocity>(entity);
}

#endif
