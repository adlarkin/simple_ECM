#ifndef ECM_HH_
#define ECM_HH_

#include <cstddef>
#include <functional>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "simpleECM/Components.hh"
#include "simpleECM/Types.hh"
#include "simpleECM/View.hh"

class ECM
{
  /// \brief Create an Entity
  /// \return The Entity that was created
  public: Entity CreateEntity();

  /// \brief Add a component to an entity (the entity must already exist)
  /// \param[in] _entity The entity
  /// \param[in] _component The component
  public: template<typename ComponentTypeT>
          void AddComponent(const Entity &_entity,
              const ComponentTypeT &_component);

  /// \brief Execute a callback function on each entity with a set of components
  /// \param[in] _f The callback function to be executed
  public: template<typename ...ComponentTypeTs>
          void Each(std::function<bool(const Entity &_entity,
                                       ComponentTypeTs*...)> _f);

  /// \brief Get the number of views stored in the ECM
  /// \return The number of views stored in the ECM
  public: std::size_t ViewCount() const;

  /// \brief Find the view that matches a set of component types.
  /// If no view with the set of component types exists, a new one is created.
  /// \return A pointer to the view
  private: template<typename ...ComponentTypeTs>
           View<ComponentTypeTs...> *FindView();

  /// \brief Check if an entity has a component of a particular type
  /// \param[in] _entity The entity
  /// \param[in] _typeId The component type
  /// \return true if _entity has a component of _typeId, false otherwise
  /// (false is returned if _entity does not exist)
  private: bool HasComponent(const Entity &_entity,
               const ComponentTypeId &_typeId) const;

  /// \brief Get the pointer to an entity's component of a particular type
  /// \param[in] _entity The entity
  /// \return The pointer to the component, if it exists. Otherwise, nullptr
  private: template<typename ComponentTypeT>
           ComponentTypeT *Component(const Entity &_entity) const;

  /// \brief A map of an Entity to its components
  private: std::unordered_map<Entity,
            std::vector<std::unique_ptr<BaseComponent>>> entityComponents;

  /// \brief A map that keeps track of where each type of component is located
  /// in the this->entityComponents vector. Since the this->entityComponents
  /// vector is of type BaseComponent, we need to keep track of which component
  /// type corresponds to a given index in the vector so that we can cast the
  /// BaseComponent to this type if needed.
  ///
  /// The key of this map is the Entity, and the value is a map of the component
  /// type to the corresponding index in the this->entityComponents vector
  /// (a component of a particular type is only a key for the value map if a
  /// component of this type exists in the this->entityComponents vector)
  private: std::unordered_map<Entity,
                              std::unordered_map<ComponentTypeId, std::size_t>>
                                componentTypeIndex;

  /// \brief Hash functor for std::vector<ComponentTypeId>
  private: struct VectorHasher
  {
    std::size_t operator()(const std::vector<ComponentTypeId> &_vec) const
    {
      auto hash = _vec.size();
      for (const auto &i : _vec)
        hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
      return hash;
    }
  };

  /// \brief All of the views. A view is defined by the list of components that
  /// make up the view, which is the key of this map (order of the component
  /// types matter since views store data in a std::tuple, so that is why the
  /// key of this map is a std::vector instead of a std::set - a tuple of type
  /// <position, velocity> is different than a <velocity, position> tuple)
  private: std::unordered_map<std::vector<ComponentTypeId>,
            std::unique_ptr<BaseView>, VectorHasher> views;
};

Entity ECM::CreateEntity()
{
  Entity entityId = this->entityComponents.size();
  this->entityComponents[entityId];
  this->componentTypeIndex[entityId];

  return entityId;
}

template<typename ComponentTypeT>
void ECM::AddComponent(const Entity &_entity, const ComponentTypeT &_component)
{
  if (this->HasComponent(_entity, ComponentTypeT::typeId))
    return;

  auto entityCompIter = this->entityComponents.find(_entity);
  auto vectorIdx = entityCompIter->second.size();
  entityCompIter->second.push_back(
      std::make_unique<ComponentTypeT>(_component));
  this->componentTypeIndex[_entity][ComponentTypeT::typeId] = vectorIdx;
}

template<typename ...ComponentTypeTs>
void ECM::Each(
    std::function<bool(const Entity &_entity, ComponentTypeTs*...)> _f)
{
  auto view = this->FindView<ComponentTypeTs...>();

  for (const auto &entity : view->Entities())
  {
    if (!std::apply(_f, view->EntityComponentData(entity)))
      break;
  }
}

std::size_t ECM::ViewCount() const
{
  return this->views.size();
}

template<typename ...ComponentTypeTs>
View<ComponentTypeTs...> *ECM::FindView()
{
  auto viewKey = std::vector<ComponentTypeId> {ComponentTypeTs::typeId...};

  // does the view already exist?
  auto iter = this->views.find(viewKey);
  if (iter != this->views.end())
    return static_cast<View<ComponentTypeTs...>*>((iter->second).get());

  // create a new view if one wasn't found
  View<ComponentTypeTs...> view;

  // only add entities to the view that have all of the components in viewKey
  for (const auto &entityCompData : this->entityComponents)
  {
    const auto entity = entityCompData.first;

    bool hasAllComponentTypes = true;
    for (const auto &compType : viewKey)
    {
      if (!this->HasComponent(entity, compType))
      {
        hasAllComponentTypes = false;
        break;
      }
    }

    if (!hasAllComponentTypes)
      continue;

    view.AddEntity(entity, this->Component<ComponentTypeTs>(entity)...);
  }

  this->views.emplace(std::make_pair(viewKey,
          std::make_unique<View<ComponentTypeTs...>>(view)));
  return static_cast<View<ComponentTypeTs...>*>(this->views[viewKey].get());
}

bool ECM::HasComponent(const Entity &_entity,
    const ComponentTypeId &_typeId) const
{
  auto typeMapIter = this->componentTypeIndex.find(_entity);
  return typeMapIter != this->componentTypeIndex.end() &&
         typeMapIter->second.find(_typeId) != typeMapIter->second.end();
}

template<typename ComponentTypeT>
ComponentTypeT *ECM::Component(const Entity &_entity) const
{
  if (!this->HasComponent(_entity, ComponentTypeT::typeId))
    return nullptr;

  const auto componentIdx =
    this->componentTypeIndex.at(_entity).at(ComponentTypeT::typeId);
  const auto baseCompPtr =
    this->entityComponents.at(_entity).at(componentIdx).get();
  return static_cast<ComponentTypeT*>(baseCompPtr);
}

#endif
