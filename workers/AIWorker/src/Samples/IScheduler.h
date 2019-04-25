#pragma once

#include <functional>

namespace SpatialOSSamples {

class IScheduler
{
public:
#if defined (_MSC_VER)
	virtual ~IScheduler() = 0 {};
#else
	virtual ~IScheduler() {};
#endif
    virtual void Schedule(const std::function<void()>& action, int waitMillis) = 0;
    virtual void ScheduleRepeat(const std::function<void()>& action, int intervalMillis) = 0;
};

}