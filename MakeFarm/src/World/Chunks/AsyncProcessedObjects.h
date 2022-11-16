#pragma once

#include "World/Chunks/ChunkContainer.h"
#include <future>
#include <list>
#include <vector>

/**
 * @brief  Checks if the thread has already finished its work.
 * @tparam T Type of the thread.
 * @param thread Thread to check.
 * @return True if the thread has completed its work, false otherwise.
 */
template<typename T>
bool hasTheThreadFinishedTheJob(const std::future<T>& thread)
{
    return thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

/**
 * @brief Asynchronous queue of processed objects is used for asynchronous work on lists of objects.
 * @tparam T The type managed by the queue.
 */
template<typename T>
struct AsyncProcessedObjects
{
    using Objects = std::list<T>;
    using ThreadOfProcessedObjects = std::future<Objects>;
    using ThreadsOfProcessedObjects = std::list<ThreadOfProcessedObjects>;

    /**
     * @brief  Creates an asynchronous process that processes the current objects to be processed by
     * the specified function.
     * @tparam Func Type of the function used to process the queue.
     * @param f Function used to process the queue.
     */
    template<typename Func>
    void processAsync(Func& f)
    {
        std::scoped_lock guard2(objectsToBeProcessedMutex);
        std::scoped_lock guard(processedObjectsThreadsMutex);
        if (!objectsToBeProcessed.empty())
        {
            processedObjectsThreads.emplace_back(
                std::async(std::launch::async, std::ref(f), std::move(objectsToBeProcessed)));

            objectsToBeProcessed.clear();
        }
    }

    /**
     * @brief  Creates an asynchronous process that processes the current objects to be processed by
     * the specified member function.
     * @tparam Owner The type of class that contains a given member function.
     * @tparam Func Type of the member function of the Owner class, designed to process the queue.
     * @param owner Pointer to the class object that contains a given member function.
     * @param func Member function of the Owner class, designed to process the queue.
     */
    template<typename Owner, typename Func>
    void processAsync(Owner* owner, Func func)
    {
        std::scoped_lock guard2(objectsToBeProcessedMutex);
        std::scoped_lock guard(processedObjectsThreadsMutex);
        if (!objectsToBeProcessed.empty())
        {
            processedObjectsThreads.emplace_back(
                std::async(std::launch::async, func, owner, std::move(objectsToBeProcessed)));

            objectsToBeProcessed.clear();
        }
    }

    /**
     * @brief Removes the process from the process list.
     * @param objectIterator The process iterator.
     */
    auto removeObject(typename ThreadsOfProcessedObjects::iterator&& objectIterator)
    {
        std::scoped_lock guard(processedObjectsThreadsMutex);
        return processedObjectsThreads.erase(objectIterator);
    }

    /**
     * @brief Removes an object from the list of objects to be processed.
     * @param object Object inside the list of the processed objects.
     */
    void removeObject(const T& object)
    {
        std::scoped_lock guard(objectsToBeProcessedMutex);
        objectsToBeProcessed.erase(
            std::remove(std::begin(objectsToBeProcessed), std::end(objectsToBeProcessed), object));
    }

    /**
     * @brief It checks if any process is running. That is, it checks if any objects are being
     * processed, or if their process has finished but has not been removed from the list yet.
     * @return True if any objects are being processed, or if their process has finished but has not
     * been removed from the list yet. False otherwise.
     */
    bool isAnyProcessRunning() const
    {
        std::unique_lock guard(processedObjectsThreadsMutex);
        return !processedObjectsThreads.empty();
    }

    /**
     * @brief It checks if no process is running. That is, it checks if no objects are being
     * processed, or if their process has finished and has been removed from the list.
     * @return True if no objects are being processed, or if their process has finished and has been
     * removed from the list. False otherwise.
     */
    bool isNoProcessRunning() const
    {
        std::unique_lock guard(processedObjectsThreadsMutex);
        return processedObjectsThreads.empty();
    }

    /**
     * @brief This function expects a lambda that can execute something on each thread getting an
     * iterator.
     * @tparam MutexType The type of mutex that should coordinateInGivenDirection this foor loop.
     * @param func A function that is called on each iterator of the thread object processing the
     * processed objects.
     */
    template<class MutexType>
    void forEveryThread(std::function<void(typename ThreadsOfProcessedObjects::iterator&)> func)
    {
        MutexType guard(processedObjectsThreadsMutex);
        for (auto chunkVectorIterator = processedObjectsThreads.begin(),
                  endChunkIterator = processedObjectsThreads.end();
             chunkVectorIterator != endChunkIterator; ++chunkVectorIterator)
        {
            func(chunkVectorIterator);
        }
    }

    mutable std::recursive_mutex objectsToBeProcessedMutex;
    mutable std::recursive_mutex processedObjectsThreadsMutex;
    Objects objectsToBeProcessed;
    ThreadsOfProcessedObjects processedObjectsThreads;
};