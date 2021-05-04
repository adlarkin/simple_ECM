#ifndef VIEW_HH_
#define VIEW_HH_

#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "simpleECM/Types.hh"

class BaseView
{
  /// \brief Get the entities that are stored in the view
  /// \return The entities in the view
  public: std::unordered_set<Entity> Entities() const
  {
    return this->entities;
  }

  /// \brief Remove an entity from the view, whether it's an entity that already
  /// exists in the view or is a new entity to be added to the view
  /// \param[in] _entity The entity
  public: virtual void RemoveEntity(const Entity &_entity) = 0;

  /// \brief Get all of the new entities that should be added to the view
  /// \return The entities
  public: std::unordered_set<Entity> NewEntities() const
  {
    return this->newEntities;
  }

  /// \brief Add a new entity to the view. This entity's component data should
  /// be added to the view the next time the view is being used. If the new
  /// entity being added already exists in the view, then this new entity is
  /// ignored
  /// \param[in] _entity The new entity
  public: void AddNewEntity(const Entity &_entity)
  {
    if (this->entities.find(_entity) == this->entities.end())
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
  public: bool virtual HasComponent(const ComponentTypeId &_typeId) const
  {
    return this->compTypes.find(_typeId) != this->compTypes.end();
  }

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

  /// \brief Get an entity and its component data. It is assumed that the
  /// entity being requested exists in the view
  /// \param[in] _entity The entity
  /// \return The entity and its component data
  public: ComponentData EntityComponentData(const Entity &_entity) const
  {
    return this->data.at(_entity);
  }

  /// \brief Add an entity with its component data to the view. It is assunmed
  /// that the entity to be added does not already exist in the view
  /// \param[in] _entity The entity
  /// \param[in] _compPtrs Pointers to the entity's components
  public: void AddEntity(const Entity &_entity, ComponentTypeTs*... _compPtrs)
  {
    this->data[_entity] = std::make_tuple(_entity, _compPtrs...);
    this->entities.insert(_entity);
  }

  /// \brief Documentation inherited
  public: void RemoveEntity(const Entity &_entity)
  {
    this->newEntities.erase(_entity);
    this->entities.erase(_entity);
    this->data.erase(_entity);
  }

  /// \brief A map of entities to their component data
  private: std::unordered_map<Entity, ComponentData> data;
};

#endif
