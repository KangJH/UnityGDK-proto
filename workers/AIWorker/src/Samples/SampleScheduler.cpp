#include "SampleScheduler.h"

using namespace SpatialOSSamples;



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
