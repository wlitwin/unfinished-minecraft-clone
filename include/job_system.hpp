#pragma once

#include "job.hpp"
#include <thread>
#include <queue>
#include <vector>
#include <functional>
#include <memory>

using JobQueue = std::priority_queue<std::shared_ptr<IJob>>;

class IJobSystem
{
};

class JobSystem : public IJobSystem
{
public:
    JobSystem();
    void Init();
    void AddJob(std::shared_ptr<IJob> job);
    void Start();
    void Stop();
private:
    JobQueue m_queue;
    std::vector<std::thread> m_threads;
};
