#pragma once

#include <functional>

namespace SpatialOSSamples {

class IScheduler
{
public:
    virtual ~IScheduler() = 0 {};

    virtual void Schedule(const std::function<void()>& action, int waitMillis) = 0;
    virtual void ScheduleRepeat(const std::function<void()>& action, int intervalMillis) = 0;
};

}