#ifndef VIEW_HPP_
#define VIEW_HPP_

#include <tuple>
#include <unordered_map>

#include "Types.hh"

class BaseView
{
};

template<typename ...ComponentTypeTs>
class View : public BaseView
{
  using ComponentData = std::tuple<Entity, ComponentTypeTs*...>;

  ComponentData &EntityComponentData(const Entity &_entity) const
  {
    return this->entityData[_entity];
  }

  void AddEntity(const Entity &_entity, ComponentTypeTs&... _comps)
  {
    this->entityData[_entity] = std::make_tuple(_entity, &_comps...);
  }

  std::unordered_map<Entity, ComponentData> data;
};

#endif
