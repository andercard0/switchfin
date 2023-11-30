#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <list>
#include <atomic>
#ifdef BOREALIS_USE_STD_THREAD
#include <thread>
#else
#include <pthread.h>
#endif
#include <borealis/core/singleton.hpp>

class ThreadPool : public brls::Singleton<ThreadPool> {
public:
    using Task = std::function<void()>;

    explicit ThreadPool();
    virtual ~ThreadPool();

    template <typename Fn, typename... Args>
    void submit(Fn&& fn, Args&&... args) {
        {
            std::lock_guard<std::mutex> locker(this->taskMutex);
            this->tasks.push_back(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        }
        this->taskCond.notify_one();
    }

    /// @brief 创建线程
    void start(size_t num);

    size_t size() const { return this->threads.size(); }

    /// @brief 停止所有线程
    void stop();

    static size_t max_thread_num;

private:
    static void* task_loop(void*);

#ifdef BOREALIS_USE_STD_THREAD
    typedef std::shared_ptr<std::thread> Thread;
#else
    typedef pthread_t Thread;
#endif

    std::list<Thread> threads;
    std::mutex threadMutex;
    std::list<Task> tasks;
    std::mutex taskMutex;
    std::condition_variable taskCond;
    std::atomic_bool isStop;
};