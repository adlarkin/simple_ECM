#ifndef MEMORY_RUNNER_HH_
#define MEMORY_RUNNER_HH_

class MemoryRunner
{
  /// \brief Destructor
  public: virtual ~MemoryRunner();

  /// \brief Create an entity with components attached to it
  public: virtual void MakeEntityWithComponents() = 0;

  /// \brief Run a potentially memory-intensive operation
  public: virtual void Run() = 0;
};

MemoryRunner::~MemoryRunner()
{
}

#endif
