#ifndef FOSSBOT_CORE_CONCURRENT_PRIORITY_QUEUE_H
#define FOSSBOT_CORE_CONCURRENT_PRIORITY_QUEUE_H

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <type_traits>

namespace nonstd {

template<typename T, typename S = std::vector<T>, typename C = std::greater<T>>
class concurrent_priority_queue
{
public:
  template<typename... Args>
  explicit concurrent_priority_queue(Args &&... args)
    : queue(std::forward<Args...>(args...)), mutex(), push_cv(), new_top_cv() {}

  ~concurrent_priority_queue() {}

  concurrent_priority_queue(const concurrent_priority_queue &other)
    : queue(other.queue), mutex(), push_cv() {}

  concurrent_priority_queue &operator=(const concurrent_priority_queue &other)
  {
    return *this = concurrent_priority_queue(other);
  }

  concurrent_priority_queue(concurrent_priority_queue &&other) = delete;

  concurrent_priority_queue &operator=(concurrent_priority_queue &&other) = delete;


  auto push(const T &val) -> void
  {
    auto lock = std::unique_lock(mutex);
    if (queue.empty()) {
        queue.push(val);
        lock.unlock();
        new_top_cv.notify_one();
        push_cv.notify_one();
    } else {
        const auto old_top = queue.top();
        queue.push(val);
        const auto new_top = queue.top();
        lock.unlock();
        if (old_top != new_top) {
            new_top_cv.notify_one();
        }
        push_cv.notify_one();
    }
  }

  auto pop() -> std::optional<T>
  {
    auto lock = std::unique_lock(mutex);
    if (queue.empty()) {
      return {};
    } else {
      const auto top = queue.top();
      queue.pop();
      return top;
    }
  }

  auto wait_pop() -> T
  {
    auto lock = std::unique_lock(mutex);
    while (queue.empty()) {
      push_cv.wait();
    }
    const auto item = queue.top();
    queue.pop();
    return item;
  }

  auto top() -> std::optional<T>
  {
    auto lock = std::unique_lock(mutex);
    if (queue.empty()) {
      return {};
    } else {
      return queue.top();
    }
  }

  auto wait_top() -> T
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
    } else {
      new_top_cv.wait_for(lock, timeout);
    }
    if (queue.empty()) {
      return {};
    } else {
      return queue.top();
    }
  }

private:
  std::priority_queue<T, S, C> queue;
  mutable std::mutex mutex;
  std::condition_variable push_cv;
  std::condition_variable new_top_cv;
};

}// namespace nonstd


#endif//FOSSBOT_CORE_CONCURRENT_PRIORITY_QUEUE_H
