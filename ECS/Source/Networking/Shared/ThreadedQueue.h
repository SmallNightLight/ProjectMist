#pragma once

#include <mutex>
#include <queue>
#include <list>

//Mutex-guarded queue
template<typename T> class ThreadedQueue
{
public:
    void push(T value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(value);
    };

    //Get top message in the queue
    //Note: not exception-safe (will lose the message)
    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        T value;
        std::swap(value, queue.front());
        queue.pop();
        return value;
    };

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

private:
    std::mutex mutex;
    std::queue<T> queue;
};