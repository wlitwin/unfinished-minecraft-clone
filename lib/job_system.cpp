#include "job_system.hpp"

JobSystem::JobSystem() = default;

void JobSystem::Init()
{
}

void JobSystem::AddJob(std::shared_ptr<IJob> job)
{

}

void JobSystem::Start()
{
    const auto hw_threads = std::thread::hardware_concurrency(); 
    for (int i = 0; i < hw_threads; ++i)
    {
        std::thread th([i]() {
            printf("Starting thread %d\n", i);
        });
        m_threads.push_back(std::move(th));
    }
}

void JobSystem::Stop()
{

}
