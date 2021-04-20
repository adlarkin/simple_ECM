#ifndef TYPES_HH_
#define TYPES_HH_

#include <cstdint>

/// \brief An entity, which can have 0 or more components
using Entity = std::uint64_t;

/// \brief An identifier that specifies a component type
using ComponentTypeId = std::uint64_t;

/// \brief An invalid component type
const ComponentTypeId kInvalidComponent{0};

#endif
