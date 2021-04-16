#ifndef ECM_HPP_
#define ECM_HPP_

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "Types.hh"
#include "View.hpp"

class ECM
{
  /// \brief Create an Entity
  /// \return The Entity that was created
  public: Entity CreateEntity();

  /// \brief Add a component to an Entity
  public: template<typename ComponentTypeT>
          void AddComponent(const Entity &_entity, ComponentTypeT _component);

  /// \brief Execute a callback function on each entity that has a set of components
  public: template<typename ...ComponentTypeTs>
          void Each(std::function<bool(const Entity &_entity, ComponentTypeTs*...)> _f);

  /// \brief Find the view that matches a set of component types.
  /// If no view with the set of component types exists, a new one is created.
  /// \return A pointer to the view
  private: template<typename ...ComponentTypeTs>
           View<ComponentTypeTs...> *FindView();

  /// \brief See if an entity has a component of a particular type
  /// \return true if _entity has a component of _typeId, false otherwise (False is returned
  /// if _entity does not exist)
  private: bool HasComponent(const Entity &_entity, const ComponentTypeId &_typeId) const;

  /// \brief A map of an Entity to its components
  private: std::unordered_map<Entity, std::vector<std::unique_ptr<BaseComponent>>> entityComponents;

  /// \brief A map that keeps track of where each type of component is located in the
  /// this->entityComponents vector. Since the this->entityComponents vector is of type
  /// BaseComponent, we need to keep track of which component type corresponds to a given
  /// index in the vector so that we can cast the BaseComponent to this type if needed.
  ///
  /// The key of this map is the Entity, and the value is a map of the component type to
  /// the corresponding index in the this->entityComponents vector (a component of a
  /// particular type is only a key for the value map if a component of this type exists
  /// in the this->entityComponents vector)
  private: std::unordered_map<Entity, std::unordered_map<ComponentTypeId, size_t>> componentTypeIndex;

  /// \brief All of the views. A view is defined by the set of components that make up the view,
  /// which is the key of this map.
  ///
  /// An issue here is that std::set only checks for the elements in the container,
  /// but not the order - for std::tuple, order matters. But, we cannot use std::tuple as the
  /// key since this would require a template (we don't know the number/type/order of components
  /// that will be used in the view at compile time). Since we can't use a template here,
  /// this means extra work in checking if two views are the same because if we are using a tuple,
  /// we may have a view of <position, velocity, acceleration> and another view of
  /// <velocity, acceleration, position>
  /// (in terms of std::set, these would be the same, but they're not the same for std::tuple).
  private: std::map<std::set<ComponentTypeId>, std::unique_ptr<BaseView>> views;

  private: std::vector<std::unique_ptr<BaseComponent>> vec;
};

Entity ECM::CreateEntity()
{
  Entity entityId = this->entityComponents.size();
  this->entityComponents[entityId];
  this->componentTypeIndex[entityId];

  return entityId;
}

template<typename ComponentTypeT>
void ECM::AddComponent(const Entity &_entity, ComponentTypeT _component)
{
  if (this->HasComponent(_entity, ComponentTypeT::typeId))
    return;

  auto entityCompIter = this->entityComponents.find(_entity);
  auto vectorIdx = entityCompIter->second.size();
  auto comp = std::make_unique<BaseComponent>(_component);
  entityCompIter->second.push_back(std::move(comp));
  this->componentTypeIndex[_entity][ComponentTypeT::typeId] = vectorIdx;
}

template<typename ...ComponentTypeTs>
void ECM::Each(std::function<bool(const Entity &_entity, ComponentTypeTs*...)> _f)
{
  auto view = this->FindView<ComponentTypeTs ...>();

  // TODO finish this
}

template<typename ...ComponentTypeTs>
View<ComponentTypeTs...> *ECM::FindView()
{
  // TODO implement this
}

bool ECM::HasComponent(const Entity &_entity, const ComponentTypeId &_typeId) const
{
  auto typeMapIter = this->componentTypeIndex.find(_entity);
  return typeMapIter != this->componentTypeIndex.end() &&
         typeMapIter->second.find(_typeId) != typeMapIter->second.end();
}

#endif
