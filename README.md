# simple_ECM

A simple entity component management system.

## Overview

An entity can have 0 or more components.
This entity/component data is stored in an entity component manager (ECM).
Data is retrieved from the ECM through views.
Views are essentially data caches that allow for very fast data retrieval.

## Project Structure

```
.
├── CMakeLists.txt
├── Components.hpp      # Defines components that can be assigned to entities
├── each_benchmark.cc   # Benchmark test for ECM::Each
├── Ecm.hpp             # The entity component manager (ECM)
├── LICENSE
├── main.cc             # A demo of how to use the ECM
├── README.md
├── Types.hh            # Defines fundamental types like Entity and ComponentTypeId
└── View.hpp            # View implementation (used by the ECM)
```

## Usage

`c++17` is required.

Using [cmake](https://cmake.org/):

```
# build the project
mkdir build
cd build
cmake ..
make

# run the demo
./ecm_demo
```

### Benchmark test

There's also a benchmark test that checks how long it takes to call `ECM::Each` for an ECM with a given number of entities.
To run this benchmark test, build the project as instructed above, and then run the `benchmark` executable:

```
./benchmark
```

You can specify the number of entities to be used in the benchmark test through a command line argument.
For example, the following command runs the benchmark test with 500 entities:

```
./benchmark 500
```

There's a similar benchmark test done for the ECM in [ign-gazebo](https://github.com/ignitionrobotics/ign-gazebo) for comparison.
For more information about comparing the two implementations, take a look at the `ign_gazebo5_comparison` branch (https://github.com/adlarkin/simple_ECM/tree/ign_gazebo5_comparison).

## Implementation/Design Details

Since this is a simple ECM, there are a few limitations:
1. There's no support for entity/component removal (only adding entities and components are currently supported).
2. Views are not updated after they are created.
So, if you create an entity with a set of components _after_ creating a view with this set of components, this new entity is not added to the view.
In other words, the view only contains entity and component data that existed at the view's creation time.

The limitations above aren't difficult to address, but I have decided not to implement these features at the moment since this is a simple ECM.

### Components

Components must inherit from `BaseComponent`, and need to have a unique ID.
This unique ID is used to identify a component with a particular component type.

### Views

Views are templated based on the type of components stored in the view.
The view essentially stores data in a table, where the rows of the table are entities, and the columns are components.
This allows for O(1) component lookup time for a given entity - all that needs to be done is index the table at the entity's row.
In order to avoid having to cast components from `BaseComponent` to the proper derived class when retrieving component data from a view, a templated [std::tuple](https://en.cppreference.com/w/cpp/utility/tuple) is used to store the component data for an entity (the concept of a "table" is accomplished by mapping entities to tuples of component data).
The template arguments for the `std::tuple` are the `View` class template component types.

Another reason why `std::tuple` is used to store component data for an entity is because the tuple can be "unpacked" into a callback function, which makes retrieving and using this data from a view quick in the `ECM::Each` method.
Instead of having to find each individual component for an entity in a view, we can simply "slice" a row of the view's "table" at the entity index to get all of the component data at once, and then apply all of this data to a callback function
(see [std::apply](https://en.cppreference.com/w/cpp/utility/apply) for more information).

### Implementation/Design Consequences

Mimicking a table for data storage allows for quick information retrieval, but requires more memory usage.
Since `std::tuple` is used, this means that the order of component types being requested when calling `ECM::Each` matters.
Consider the following scenario:

_A user has an ECM with entities that have `Position` and `Velocity` components.
At one point, the user calls `ECM::Each` with component types `<Position, Velocity>`, but later on calls `ECM::Each` with component types `<Velocity, Position>`.
Although the user is asking for the same data both times (entities with position and velocity components), two different views will be created.
One view will have component data stored in a `std::tuple<Position, Velocity>`, and the other view will use a `std::tuple<Velocity, Position>`._

In a perfect world, we would like to have only one view for both of the `ECM::Each` calls described in the scenario above, but this would probably result in views that don't provide `O(1)` lookup time.
