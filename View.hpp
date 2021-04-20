#ifndef VIEW_HPP_
#define VIEW_HPP_

#include <set>
#include <tuple>
#include <unordered_map>

#include "Types.hh"

class BaseView
{
  /// \brief Get the entities that are stored in the view
  /// \return The entities in the view
  public: std::set<Entity> Entities() const
  {
    return this->entities;
  }

  /// \brief The entities in the view
  protected: std::set<Entity> entities;
};

template<typename ...ComponentTypeTs>
class View : public BaseView
{
  private: using ComponentData = std::tuple<Entity, ComponentTypeTs*...>;

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

  /// \brief A map of entities to their component data
  private: std::unordered_map<Entity, ComponentData> data;
};

#endif
