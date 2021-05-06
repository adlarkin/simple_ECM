#ifndef BENCHMARK_RUNNER_HH_
#define BENCHMARK_RUNNER_HH_

#include <cstddef>
#include <chrono>
#include <iostream>
#include <string>

class BenchmarkRunner
{
  /// \brief Destructor
  public: virtual ~BenchmarkRunner();

  /// \brief Initialize components internal to the derived benchmark runner,
  /// including the number of entities that should have components added/removed
  /// \param[in] _numEntitiesToModify The number of entities that should have
  /// components added/removed
  public: virtual void Init(const std::size_t _numEntitesToModify) = 0;

  /// \brief Create an entity with components attached to it
  public: virtual void MakeEntityWithComponents() = 0;

  /// \brief Call the derived class' Each(...) implementation
  public: virtual void EachImplementation() = 0;

  /// \brief Remove a single component from a number of entities, which was
  /// defined when calling Init. This should be called before AddAComponent
  public: virtual void RemoveAComponent() = 0;

  /// \brief Add a single component to a number of entities, which was defined
  /// when calling Init. This should be called after RemoveAComponent
  public: virtual void AddAComponent() = 0;

  /// \brief Record the start time of a time interval
  public: void StartTimer();

  /// \brief Record the end time of a time interval
  public: void StopTimer();

  /// \brief Display the time elapsed between the StartTimer and StopTimer
  /// calls, in milliseconds
  /// \param[in] _infoMsg An informational message to display along with the
  /// elapsed time. Useful for providing context about the time being displayed
  public: void DisplayElapsedTime(const std::string &_infoMsg = "") const;

  /// \brief Verify that the latest EachImplementation call iterated over the
  /// correct number of entities
  /// \param[in] _targetEntityCount The number of entities that should have
  /// been iterated over
  /// \return true if the latest EachImplementation call iterated over
  /// _targetEntityCount, false otherwise
  public: bool Valid(const int &_targetEntityCount) const;

  /// \brief Internal counter that should be used by derived classes to keep
  /// track of how many entities were iterated over in EachImplementation
  protected: int entityCount{0};

  /// \brief The number of entities that should have a component removed/added
  protected: std::size_t numEntitiesToModify{0};

  /// \brief A time point representing the start of a time interval
  private: std::chrono::time_point<std::chrono::steady_clock> start;

  /// \brief A time point representing the end of a time interval
  private: std::chrono::time_point<std::chrono::steady_clock> end;
};

BenchmarkRunner::~BenchmarkRunner()
{
}

void BenchmarkRunner::StartTimer()
{
  this->start = std::chrono::steady_clock::now();
}

void BenchmarkRunner::StopTimer()
{
  this->end = std::chrono::steady_clock::now();
}

void BenchmarkRunner::DisplayElapsedTime(const std::string &_infoMsg) const
{
  const std::chrono::duration<double, std::milli> durationMs =
    this->end - this->start;
  std::cout << _infoMsg << durationMs.count() << " ms" << std::endl;
}

bool BenchmarkRunner::Valid(const int &_targetEntityCount) const
{
  if (this->entityCount != _targetEntityCount)
  {
    std::cerr << "Internal error: " << _targetEntityCount
      << " entities should have been iterated over, but " << this->entityCount
      << " were iterated over instead" << std::endl;
    return false;
  }
  return true;
}

#endif
