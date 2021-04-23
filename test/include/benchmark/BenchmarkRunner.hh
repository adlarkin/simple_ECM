#ifndef BENCHMARK_RUNNER_HH_
#define BENCHMARK_RUNNER_HH_

#include <chrono>
#include <iostream>

class BenchmarkRunner
{
  /// \brief Destructor
  public: virtual ~BenchmarkRunner();

  /// \brief Initialize components internal to the derived benchmark runner.
  /// Depending on the derived class implementation, this function may be empty
  public: virtual void Init() = 0;

  /// \brief Create an entity with components attached to it
  public: virtual void MakeEntityWithComponents() = 0;

  /// \brief Call the derived class' Each(...) implementation
  public: virtual void EachImplementation() = 0;

  /// \brief Record the start time of a time interval
  public: void StartTimer();

  /// \brief Record the end time of a time interval
  public: void StopTimer();

  /// \brief Display the time elapsed between the StartTimer and StopTimer
  /// calls, in milliseconds
  public: void DisplayElapsedTime() const;

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

void BenchmarkRunner::DisplayElapsedTime() const
{
  const std::chrono::duration<double, std::milli> durationMs =
    this->end - this->start;
  std::cout << durationMs.count() << " ms" << std::endl;
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
