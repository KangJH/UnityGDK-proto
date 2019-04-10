#pragma once

#include <functional>
#include <memory>

namespace SpatialOS {

struct CommandResponseSummary;

class IRetryNotifier
{
public:
    virtual ~IRetryNotifier() = 0 {}

    virtual void ProcessResponse(const CommandResponseSummary& response) = 0;
    virtual void OnNotifyRetry(const std::function<void()>& callback) = 0;
    virtual void OnNotifyCompleted(const std::function<void()>& callback) = 0;

    virtual std::unique_ptr<SpatialOS::IRetryNotifier> Clone() const = 0;
};

}