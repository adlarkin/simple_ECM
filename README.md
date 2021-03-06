# simple_ECM

This repository contains a simple implementation of an entity component system.
Details about the implementation can be found in the [implementation and design Details](#implementation-and-design-details) section.

## Overview/Terminology

An entity can have 0 or more components.
This entity/component data is stored in an entity component manager (ECM).
Data is retrieved from the ECM through views.
Views are essentially data caches that allow for very fast data retrieval.

## Usage

### Requirements

`c++17` is required.
Benchmark tests are also available that compare the performance of the simple implementation in this repository to [EnTT](https://github.com/skypjack/entt) and the ECM in [ign-gazebo](https://github.com/ignitionrobotics/ign-gazebo).

If you'd like to run the benchmark tests against `ign-gazebo`, make sure that you have `ign-gazebo5` installed (instructions can be found [here](https://github.com/ignitionrobotics/ign-gazebo#binary-install)).

If you'd like to run the benchmark tests against `EnTT`, make sure that you enable the `entt` submodule (run the following command in the root of this repository):

```
git submodule update --init --recursive
```

### Building

Using [cmake](https://cmake.org/):

```
mkdir build
cd build
cmake ..
make
```

### Running

#### Simple Demo

```
# go to the build directory if you aren't there already
cd build

./ecm_demo
```

#### Benchmark test

The benchmark test checks how long it takes to call `Each(...)` for an ECM with a given number of entities and components.
As mentioned in the [requirements](#requirements) section, this benchmark test can also test `EnTT` and the ECM in `ign-gazebo` if the project was built with the proper dependencies.

Running the benchmark can be done as follows:

```
# go to the build directory if you aren't there already
cd build

# you need to specify how many entities to create. In this example, 100 entities are created
./benchmark_test 100
```

You can also specify the number of entities that should have a component removed and added back in.
This is useful for benchmarking `Each(...)` performance with entities whose number of components change frequently.
For example, the following command runs the benchmark test with 100 entities, where 50 of those 100 entities have a component removed and added back in:

```
./benchmark_test 100 50
```

#### Memory test

As discussed in the [implementation and design consequences section](#implementation-and-design-consequences), the view implementation proposed in this repository should result in faster component lookup time, but may require more memory usage.
A memory test can be run to compare the usage between the simple ECM and the ECM in `ign-gazebo`.
The ECM in `ign-gazebo` has some additional complexities that require more memory usage, but this is still a useful reference to make sure that the simple ECM does not incur a memory overhead that is significantly greater than the memory used for the `ign-gazebo` ECM.

The memory test involves creating some number of entities (1000 is the default, but the user can specify otherwise), with 10 components per entity.
Then, `Each(...)` is called 14 times, with a different set/order of components being used in every `Each(...)` call.
The simple ECM should create a new view for every call since the components used and order of the components matter, while the `ign-gazebo` ECM shouldn't create as many views since the order of components used in `ign-gazebo` views does not matter (for `ign-gazebo` (Edifice), the same view is used for the same set of components, regardless of the component order used when requesting data from the view).

An easy way to inspect memory usage is with [heaptrack](https://github.com/KDE/heaptrack).
Once heaptrack is installed, you can run the memory tests as follows:

```
# go to the build directory if you aren't there already
cd build

# the following runs the memory test for the simple ECM
heaptrack ./memory_test simpleECM

# the following runs the memory test for the ign-gazebo ECM
heaptrack ./memory_test ignGazeboECM

# you can also specify the number of entities used in the test
# (this would run the memory test with 500 entities)
heaptrack ./memory_test simpleECM 500
```

After running the memory test with `heaptrack`, you should see something at the end of the output like this:

```
Heaptrack finished! Now run the following to investigate the data: ...
```

Run the command specified to inspect memory usage.

## Implementation and Design Details

At the time of this writing, the ECM implemented in this repository is header-only.

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

### Implementation and Design Consequences

Mimicking a table for data storage allows for quick information retrieval, but requires more memory usage.
Since `std::tuple` is used, this means that the order of component types being requested when calling `ECM::Each` matters.
Consider the following scenario:

_A user has an ECM with entities that have `Position` and `Velocity` components.
At one point, the user calls `ECM::Each` with component types `<Position, Velocity>`, but later on calls `ECM::Each` with component types `<Velocity, Position>`.
Although the user is asking for the same data both times (entities with position and velocity components), two different views will be created.
One view will have component data stored in a `std::tuple<Position, Velocity>`, and the other view will use a `std::tuple<Velocity, Position>`._

In a perfect world, we would like to have only one view for both of the `ECM::Each` calls described in the scenario above, but this would probably result in views that don't provide `O(1)` lookup time.
