#ifndef ECM_HH_
#define ECM_HH_

#include <cstddef>
#include <functional>
#include <memory>
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

  // TODO handle adding/removing a component before Each is called. In this
  // case, the entity with this component may try to be added to a view when it
  // shouldn't be

  /// \brief Add a component to an entity (the entity must already exist)
  /// \param[in] _entity The entity
  /// \param[in] _component The component
  public: template<typename ComponentTypeT>
          void AddComponent(const Entity &_entity,
              const ComponentTypeT &_component);

  /// \brief Remove a component from an entity
  /// \param[in] _entity The entity
  public: template<typename ComponentTypeT>
          void RemoveComponent(const Entity &_entity);

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

  /// \brief Check if an entity has a reference to a component of a particular
  /// type. This does not indicate whether the component being referred to is
  /// valid/being used by the entity or not (the component may have been removed
  /// after it was originally added to the entity). The Component method should
  /// be used to learn more about the state of the referenced component, if a
  /// reference exists
  /// \param[in] _entity The entity
  /// \param[in] _typeId The component type
  /// \return true if _entity has a reference to a component of _typeId, false
  /// otherwise (false is returned if _entity does not exist)
  /// \sa Component
  private: bool HasComponent(const Entity &_entity,
               const ComponentTypeId &_typeId) const;

  private: bool HasValidComponent(const Entity &_entity,
               const ComponentTypeId &_typeId) const;

  /// \brief Get the pointer to an entity's component of a particular type
  /// \param[in] _entity The entity
  /// \return The pointer to the component (nullptr if the component doesn't
  /// exist). If the pointer isn't nullptr, the ignore flag of the component
  /// should be checked to see if this component is being used by _entity or
  /// not. Ignore is true if _entity isn't using this component (i.e., the
  /// component was added to _entity but later removed from _entity) Ignore is
  /// false if _entity is using this component (i.e., the component was added to
  /// _entity and has not been removed)
  /// \sa HasComponent
  private: template<typename ComponentTypeT>
           ComponentTypeT *Component(const Entity &_entity) const;

  /// \brief See if an entity has a list of valid component types
  /// \param[in] _entity The entity
  /// \param[in] _compTypes The types of components
  /// \return true if _entity has each type of component in _compTypes as valid
  /// references, false otherwise
  private: bool HasAllComponents(const Entity &_entity,
               const std::vector<ComponentTypeId> &_compTypes) const;

  /// \brief A map of an Entity to its components
  private: std::unordered_map<Entity,
            std::vector<std::shared_ptr<BaseComponent>>> entityComponents;

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
  // check to see if a component of type ComponentTypeT::typeId already exists
  // for _entity. There are a few cases to consider:
  // 1. A component of this type was added to _entity previously
  //    a. If this component wasn't removed, this component's ignore flag is
  //       false, so the request to add a new component should be ignored
  //    b. If this component was removed, this component's ignore flag is true,
  //       so this component should be modified directly to contain the data
  //       contained in _component
  // 2. A component of this type was never added to _entity previously. In this
  //    case, a new component needs to be created and added to _entity, and
  //    _entity may need to be added to views that have this component type

  if (this->HasComponent(_entity, ComponentTypeT::typeId))
  {
    auto existingCompPtr = this->Component<ComponentTypeT>(_entity);
    // case 1b
    if (existingCompPtr->ignore)
    {
      // update the component data and notify relevant views that _entity had a
      // component update in case _entity once again meets the component
      // requirements of the view
      *existingCompPtr = _component;
      existingCompPtr->ignore = false;
      for (auto &[compTypes, view] : this->views)
      {
        if (view->HasComponent(ComponentTypeT::typeId))
        {
          if (view->HasComponentData(_entity))
            view->NotifyComponentAddition(_entity, ComponentTypeT::typeId);
          else if (this->HasAllComponents(_entity, compTypes))
            view->AddNewEntity(_entity);
        }
      }
    }
    // (if the check above for case 1b failed, conditions for case 1a are met,
    // which means do nothing/ignore the add component request)
    return;
  }

  // case 2
  auto entityCompIter = this->entityComponents.find(_entity);
  auto vectorIdx = entityCompIter->second.size();
  entityCompIter->second.push_back(
      std::make_shared<ComponentTypeT>(_component));
  this->componentTypeIndex[_entity][ComponentTypeT::typeId] = vectorIdx;
  for (auto &[compTypes, view] : this->views)
  {
    //if (!view->HasEntity(_entity) && !view->HasNewEntity(_entity) &&
    if (!view->HasComponentData(_entity) && !view->HasNewEntity(_entity) &&
        this->HasAllComponents(_entity, compTypes))
      view->AddNewEntity(_entity);
  }
}

template<typename ComponentTypeT>
void ECM::RemoveComponent(const Entity &_entity)
{
  if (!this->HasComponent(_entity, ComponentTypeT::typeId))
    return;

  auto compPtr = this->Component<ComponentTypeT>(_entity);
  if (!compPtr->ignore)
  {
    compPtr->ignore = true;
    for (auto &[compTypes, view] : this->views)
    {
      // notify relevant views that a component has been removed from _entity
      // (_entity no longer meets the component requirements for views that
      // contain component data of type ComponentTypeT::typeId)
      if (view->HasComponent(ComponentTypeT::typeId) &&
          view->HasComponentData(_entity))
      {
        view->NotifyComponentRemoval(_entity, ComponentTypeT::typeId);
      }
    }
  }
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
  std::vector<ComponentTypeId> viewKey {ComponentTypeTs::typeId...};

  // does the view already exist?
  auto iter = this->views.find(viewKey);
  if (iter != this->views.end())
  {
    auto view = static_cast<View<ComponentTypeTs...>*>((iter->second).get());

    // add any new entities to the view before using it
    for (const auto &entity : view->NewEntities())
      view->AddEntity(entity, this->Component<ComponentTypeTs>(entity)...);
    view->RemoveNewEntities();

    return view;
  }

  // create a new view if one wasn't found
  View<ComponentTypeTs...> view;

  // only add entities to the view that have all of the components in viewKey
  for (const auto &entityCompData : this->entityComponents)
  {
    const auto entity = entityCompData.first;

    if (!this->HasAllComponents(entity, viewKey))
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

// TODO clean this up and turn it into something more useful
bool ECM::HasValidComponent(const Entity &_entity,
    const ComponentTypeId &_typeId) const
{
  auto typeMapIter = this->componentTypeIndex.find(_entity);
  return typeMapIter != this->componentTypeIndex.end() &&
         typeMapIter->second.find(_typeId) != typeMapIter->second.end() &&
         !this->entityComponents.at(_entity)
          .at(typeMapIter->second.find(_typeId)->second)->ignore;
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

bool ECM::HasAllComponents(const Entity &_entity,
    const std::vector<ComponentTypeId> &_compTypes) const
{
  for (const auto &type : _compTypes)
  {
    if (!this->HasValidComponent(_entity, type))
      return false;
  }

  return true;
}

#endif
