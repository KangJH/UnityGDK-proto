#pragma once

#include <functional>
#include <memory>

#include "improbable/worker.h"

#include "Samples/ExponentialBackoff.h"

#include "CommandSender/IRetryNotifier.h"
#include "CommandSender/RetryStrategyConfig.h"

namespace SpatialOS {

struct CommandResponseSummary;

}

namespace SpatialOSSamples {

class IScheduler;

class SampleRetryNotifier : public SpatialOS::IRetryNotifier
{
public:
    SampleRetryNotifier(const SpatialOS::RetryStrategyConfig& retryConfig,
        const std::shared_ptr<IScheduler>& retryScheduler);

    virtual void OnNotifyRetry(const std::function<void()>& callback) override;
    virtual void OnNotifyCompleted(const std::function<void()>& callback) override;
    virtual void ProcessResponse(const SpatialOS::CommandResponseSummary& response) override;

    virtual std::unique_ptr<SpatialOS::IRetryNotifier> Clone() const override;

private:
    ExponentialBackoff backoffGenerator;

    std::function<void()> retryCallback;
    std::function<void()> completionCallback;

    std::shared_ptr<IScheduler> scheduler;
    SpatialOS::RetryStrategyConfig config;
    std::function<void(const SpatialOS::CommandResponseSummary &)> callback;

    const int startingRetries;
    int retriesLeft;

    bool CanRetry(worker::StatusCode lastReply);
    SpatialOS::RetryStrategy GetStategy(worker::StatusCode lastReply);
    void ScheduleNextRetry(SpatialOS::RetryStrategy strategy);
    int DelayFor(SpatialOS::RetryStrategy strategy);
};

}