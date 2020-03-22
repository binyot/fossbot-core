#ifndef FOSSBOT_CORE_CONCURRENT_QUEUE_H
#define FOSSBOT_CORE_CONCURRENT_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <type_traits>


namespace nonstd {

template<typename T, typename Q>
class concurrent_queue
{
public:
  template<typename... Args>
  explicit concurrent_queue(Args &&... args)
    : queue(std::forward<Args...>(args...)), mutex(), push_cv() {}

  ~concurrent_queue() {}

  concurrent_queue(const concurrent_queue &other)
    : queue(other.queue), mutex(), push_cv() {}

  concurrent_queue &operator=(const concurrent_queue &other)
  {
    return *this = concurrent_queue(other);
  }

  concurrent_queue(concurrent_queue &&other) = delete;

  concurrent_queue &operator=(concurrent_queue &&other) = delete;

  auto push(const T &val) -> void
  {
    auto lock = std::unique_lock(mutex);
    queue.push(val);
    lock.unlock();
    push_cv.notify_one();
  }

  auto empty() const -> bool
  {
    auto lock = std::unique_lock(mutex);
    return queue.empty();
  }

  auto top() -> std::optional<T>
  {
    auto lock = std::unique_lock(mutex);
    if (queue.empty()) {
      return {};
    }
    return queue.top();
  }

  auto wait_top() -> const T &
  {
    auto lock = std::unique_lock(mutex);
    while (queue.empty()) {
      push_cv.wait(lock);
    }
    return queue.top();
  }

  template<typename D = std::chrono::milliseconds>
  auto wait_new_top_for(D timeout) -> std::optional<T>
  {
    auto lock = std::unique_lock(mutex);
    if (queue.empty()) {
      push_cv.wait_for(lock, timeout);
    }
    if (queue.empty()) {
      return {};
    } else {
      return queue.top();
    }
  }

  auto pop() -> std::optional<T>
  {
    auto lock = std::unique_lock(mutex);
    if (queue.empty()) {
      return {};
    }
    const auto val = queue.top();
    queue.pop();
    return val;
  }

  auto wait_pop() -> T
  {
    auto lock = std::unique_lock(mutex);
    while (queue.empty()) {
      push_cv.wait(lock);
    }
    const auto val = queue.top();
    queue.pop();
    return val;
  }

private:
  Q queue;// can be a uniqe_ptr to enable move ctors
  mutable std::mutex mutex;
  std::condition_variable push_cv;
};

}// namespace nonstd

#endif//FOSSBOT_CORE_CONCURRENT_QUEUE_H
