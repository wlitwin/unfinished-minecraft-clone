#pragma once

#include "job.hpp"

class ChunkGenerationJob : public IJob
{
public:
    ChunkGenerationJob();
    void Run() override;
    int Priority() override;
};
