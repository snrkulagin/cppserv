#include "thread_pool.hpp"
#include <iostream>

void thread_pool::worker_function()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // Loop here in case of a spurious wakeup
            while (task_queue.empty())
            {
                if (should_stop)
                {
                    // Only return when queue is empty and should_stop
                    return;
                }
                // Wait until notified or woken up spuriously
                queue_notification.wait(lock);
            }
            task = std::move(task_queue.front());
            task_queue.pop();
        }
        task();
    }
}

thread_pool::thread_pool(std::size_t num_workers) : should_stop(false)
{
    // If you know the size beforehand, just reserve it directly
    workers.reserve(num_workers);
    try
    {
        for (auto i = 0; i < num_workers; i++)
        {
            workers.emplace_back(&thread_pool::worker_function, this);
        }
    }
    catch (...)
    { // catch all
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            should_stop = true;
        }
        queue_notification.notify_all();
        for (auto &worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
        throw; // rethrow the caught exception
    }
}

thread_pool::~thread_pool()
{
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        should_stop = true;
    }
    queue_notification.notify_all();
    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}