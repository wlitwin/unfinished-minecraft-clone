#pragma once
#include <vector>

class IJob
{
public:
    virtual void Run() = 0;
    virtual int Priority() = 0;
};
