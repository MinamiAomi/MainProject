#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(size_t threads = 0);
    ~ThreadPool();

    void PushTask(std::function<void()> task);
    void WaitForAll();

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> taskQueue_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::condition_variable completionCondition_;
    bool stop_;
    size_t activeTasks_;
};
