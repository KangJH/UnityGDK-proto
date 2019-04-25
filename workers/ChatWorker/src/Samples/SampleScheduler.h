#pragma once

#include <functional>
#include <queue>

#include "Samples/IScheduler.h"

namespace SpatialOSSamples {

class SampleScheduler : public SpatialOSSamples::IScheduler
{
public:
    SampleScheduler(unsigned long startTime);

    void Update(unsigned long deltaTime);

    virtual void Schedule(const std::function<void()>& action, int waitMillis) override;
    virtual void ScheduleRepeat(const std::function<void()>& action, int intervalMillis) override;

private:
	struct Action
	{
		Action(const std::function<void()>& action, unsigned long triggerMillis, unsigned long intervalMillis)
			: callback(action),
			triggerTime(triggerMillis),
			repeat(intervalMillis)
		{}

		bool operator>(const Action& other) const
		{
			return triggerTime > other.triggerTime;
		}

		std::function<void()> callback;
		unsigned long triggerTime;
		unsigned long repeat;
	};

    unsigned long currentTime;
    std::priority_queue<Action, std::vector<Action>, std::greater<Action>> scheduledActions;

    void RunScheduledActions();
};

}