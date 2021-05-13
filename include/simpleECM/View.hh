#ifndef VIEW_HH_
#define VIEW_HH_

#include <cstddef>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "simpleECM/Types.hh"

class BaseView
{
  /// \brief Get the entities that are stored in the view
  /// \return The entities in the view
  public: const std::unordered_set<Entity> &Entities() const
  {
    return this->entities;
  }

  public: virtual bool HasComponentData(const Entity &_entity) = 0;

  /// \brief Check if an entity is a part of the view
  /// \param[in] _entity The entity
  /// \return true if _entity is a part of the view, false otherwise
  public: bool HasEntity(const Entity &_entity) const
  {
    return this->entities.find(_entity) != this->entities.end();
  }

  /// \brief Check if an entity is marked as an entity to be added to the view
  /// \param[in] _entity The entity
  /// \return true if _entity is to be added to the view, false otherwise
  public: bool HasNewEntity(const Entity &_entity) const
  {
    return this->newEntities.find(_entity) != this->newEntities.end();
  }

  /// \brief Remove an entity from the view, whether it's an entity that already
  /// exists in the view or is a new entity to be added to the view
  /// \param[in] _entity The entity
  public: virtual void RemoveEntity(const Entity &_entity) = 0;

  /// \brief Get all of the new entities that should be added to the view
  /// \return The entities
  public: const std::unordered_set<Entity> &NewEntities() const
  {
    return this->newEntities;
  }

  /// \brief Add a new entity to the view. This entity's component data should
  /// be added to the view the next time the view is being used. It is assumed
  /// that this new entity isn't already associated with the view
  /// \param[in] _entity The new entity
  /// \sa HasEntity HasNewEntity
  public: void AddNewEntity(const Entity &_entity)
  {
    this->newEntities.insert(_entity);
  }

  /// \brief Remove all of the new entities from the view. This should be called
  /// after all of the new entity component data has been added to the view
  public: void RemoveNewEntities()
  {
    this->newEntities.clear();
  }

  /// \brief See if the view holds data of a particular component type
  /// \param[in] _typeId The component type
  /// \return true if the view has component data of type _typeId, false
  /// otherwise
  public: bool HasComponent(const ComponentTypeId &_typeId) const
  {
    return this->compTypes.find(_typeId) != this->compTypes.end();
  }

  /// \brief Update the internal data in the view because a component has been
  /// added to an entity. It is assumed that the entity already is a part of the
  /// view.
  /// \param[in] _entity The entity
  /// \param[in] _typeId The type of component that was added to _entity
  public: virtual void NotifyComponentAddition(const Entity &_entity,
              const ComponentTypeId &_typeId) = 0;

  /// \brief Update the internal data in the view because a component has been
  /// removed to an entity. It is assumed that the entity already is a part of
  /// the view.
  /// \param[in] _entity The entity
  /// \param[in] _typeId The type of component that was removed from _entity
  public: virtual void NotifyComponentRemoval(const Entity &_entity,
              const ComponentTypeId &_typeId) = 0;

  /// \brief Destructor
  public: virtual ~BaseView()
  {
  };

  /// \brief New entities to be added to the view
  protected: std::unordered_set<Entity> newEntities;

  /// \brief The entities in the view
  protected: std::unordered_set<Entity> entities;

  /// \brief The component types in the view
  protected: std::unordered_set<ComponentTypeId> compTypes;

  /// \brief A map that keeps track of which components for entities in
  /// invalidData need to be added back to the entity in order to move the
  /// entity back to validData.
  ///
  /// \sa invalidData
  protected: std::unordered_map<Entity, std::unordered_set<ComponentTypeId>>
             missingCompTracker;
};

template<typename ...ComponentTypeTs>
class View : public BaseView
{
  private: using ComponentData = std::tuple<Entity, ComponentTypeTs*...>;

  /// \brief Constructor
  public: View()
  {
    this->compTypes = {ComponentTypeTs::typeId...};
  }

  public: bool HasComponentData(const Entity &_entity)
  {
    return this->validData.find(_entity) != this->validData.end() ||
      this->invalidData.find(_entity) != this->invalidData.end();
  }

  /// \brief Get an entity and its component data. It is assumed that the
  /// entity being requested exists in the view
  /// \param[in] _entity The entity
  /// \return The entity and its component data
  // TODO return a reference here instead of a copy?
  public: ComponentData EntityComponentData(const Entity &_entity) const
  {
    return this->validData.at(_entity);
  }

  /// \brief Add an entity with its component data to the view. It is assunmed
  /// that the entity to be added does not already exist in the view
  /// \param[in] _entity The entity
  /// \param[in] _compPtrs Pointers to the entity's components
  public: void AddEntity(const Entity &_entity, ComponentTypeTs*... _compPtrs)
  {
    this->validData[_entity] = std::make_tuple(_entity, _compPtrs...);
    this->entities.insert(_entity);
  }

  /// \brief Documentation inherited
  public: void RemoveEntity(const Entity &_entity)
  {
    this->newEntities.erase(_entity);
    this->entities.erase(_entity);
    this->validData.erase(_entity);
  }

  /// \brief Documentation inherited
  public: void NotifyComponentAddition(const Entity &_entity,
              const ComponentTypeId &_typeId)
  {
    auto missingCompsIter = this->missingCompTracker.find(_entity);
    missingCompsIter->second.erase(_typeId);
    if (missingCompsIter->second.empty())
    {
      auto nh = this->invalidData.extract(_entity);
      this->validData.insert(std::move(nh));
      this->entities.insert(_entity);
      this->missingCompTracker.erase(_entity);
    }
  };

  /// \brief Documentation inherited
  public: void NotifyComponentRemoval(const Entity &_entity,
              const ComponentTypeId &_typeId)
  {
    // if the component being removed is the first component that causes _entity
    // to be invalid for this view, move _entity from validData to invalidData
    // and make sure _entity isn't considered a part of the view
    auto it = this->validData.find(_entity);
    if (it != this->validData.end())
    {
      auto nh = this->validData.extract(it);
      this->invalidData.insert(std::move(nh));
      this->entities.erase(_entity);
    }
    this->missingCompTracker[_entity].insert(_typeId);
  };

  /// \brief A map of entities to their component data
  private: std::unordered_map<Entity, ComponentData> validData;

  /// \brief A map of invalid entities to their component data. The difference
  /// between invalidData and validData is that the entities in invalidData were
  /// once in validData, but they had a component removed, so the entity no
  /// longer meets the component requirements of the view. If the missing
  /// component data is ever added back to an entitiy in invalidData, then this
  /// entity will be moved back to validData
  ///
  /// The reason for moving entities with missing components to invalidData
  /// instead of completely deleting them is because tuple creation can be
  /// costly, so this approach is used instead to maintain performance (the
  /// tradeoff of mainting performance is increased complexity and memory usage)
  ///
  /// \sa missingCompTracker
  private: std::unordered_map<Entity, ComponentData> invalidData;
};

#endif
