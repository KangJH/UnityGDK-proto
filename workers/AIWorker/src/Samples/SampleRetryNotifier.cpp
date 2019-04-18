#include "SampleRetryNotifier.h"

#include "Samples/IScheduler.h"

#include "CommandSender/CommandResponseSummary.h"
#include "RequireExternal/ILogger.h"

using namespace SpatialOS;
using namespace SpatialOSSamples;

using Logging = SpatialOS::RequireExternal::ILogger;

SampleRetryNotifier::SampleRetryNotifier(const RetryStrategyConfig& retryConfig, const std::shared_ptr<IScheduler>& retryScheduler)
    : scheduler(retryScheduler),
    config(retryConfig),
    startingRetries(retryConfig.maxRetriesPerCommand),
    retriesLeft(retryConfig.maxRetriesPerCommand),
    backoffGenerator(6)
{

}

void SampleRetryNotifier::OnNotifyRetry(const std::function<void()>& callback)
{
    retryCallback = callback;
}

void SampleRetryNotifier::OnNotifyCompleted(const std::function<void()>& callback)
{
    completionCallback = callback;
}

void SampleRetryNotifier::ProcessResponse(const CommandResponseSummary& response)
{
    if (response.status == worker::StatusCode::kSuccess || !CanRetry(response.status))
    {
        std::string retries = startingRetries == retriesLeft ?
            " without needing to retry" :
            " after " + std::to_string(startingRetries - retriesLeft) + " out of " + std::to_string(startingRetries) + "retries";

        Logging::ApplicationLogger->Debug(response.status == worker::StatusCode::kSuccess ?
            "\t RetryStrategy: Command Succeeded" + retries :
            "\t RetryStrategy: Command Failed" + retries
        );
        completionCallback();
    }
    else
    {
        ScheduleNextRetry(GetStategy(response.status));
    }
}

std::unique_ptr<IRetryNotifier> SampleRetryNotifier::Clone() const
{
    return std::make_unique<SampleRetryNotifier>(config, scheduler);
}

//Decide if we can retry the command based on last received reply & retry history
bool SampleRetryNotifier::CanRetry(worker::StatusCode lastReply)
{
    return retriesLeft > 0 &&
        GetStategy(lastReply) != RetryStrategy::Never;
}

//Decide what the next strategy should be based on last received reply - never, immediate or backoff
RetryStrategy SampleRetryNotifier::GetStategy(worker::StatusCode lastReply)
{
    return config.StrategyForStatus.at(lastReply);
}

//Schedule the next retry following given strategy
void SampleRetryNotifier::ScheduleNextRetry(RetryStrategy strategy)
{
    int delayRetryMs = DelayFor(strategy);
    if (delayRetryMs == 0)
    {
        retryCallback();
    }
    else
    {
        scheduler->Schedule(retryCallback, delayRetryMs);
    }

    Logging::ApplicationLogger->Debug("\t RetryStrategy: Scheduled retry in " + std::to_string(delayRetryMs) + "ms.");

    retriesLeft--;
}

//How long should we wait before sending the next retry from 
int SampleRetryNotifier::DelayFor(RetryStrategy strategy)
{
    return strategy == RetryStrategy::Immediate ?
        0 :
        backoffGenerator.NextWaitMs();
}
