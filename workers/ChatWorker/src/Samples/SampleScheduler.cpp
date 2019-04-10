#include "SampleScheduler.h"

using namespace SpatialOSSamples;

struct SampleScheduler::Action
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

SampleScheduler::SampleScheduler(unsigned long startTime) : currentTime(startTime)
{

}

void SampleScheduler::Update(unsigned long deltaTime)
{
    currentTime += deltaTime;

    RunScheduledActions();
}

void SampleScheduler::Schedule(const std::function<void()>& action, int waitMillis)
{
    scheduledActions.emplace(action, currentTime + waitMillis, 0);
}

void SampleScheduler::ScheduleRepeat(const std::function<void()>& action, int intervalMillis)
{
    scheduledActions.emplace(action, currentTime + intervalMillis, intervalMillis);
}

void SampleScheduler::RunScheduledActions()
{
    while (!scheduledActions.empty() && scheduledActions.top().triggerTime <= currentTime)
    {
        scheduledActions.top().callback();

        if (scheduledActions.top().repeat > 0)
        {
            Action repeat(scheduledActions.top());
            repeat.triggerTime += repeat.repeat;
            scheduledActions.emplace(repeat);
        }

        scheduledActions.pop();
    }
}
