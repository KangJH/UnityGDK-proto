#pragma once

#include <functional>
#include <memory>

namespace SpatialOS {

struct CommandResponseSummary;

class IRetryNotifier
{
public:
#if defined (_MSC_VER)
	virtual ~IRetryNotifier() = 0 {};
#else
	virtual ~IRetryNotifier() {};
#endif

    virtual void ProcessResponse(const CommandResponseSummary& response) = 0;
    virtual void OnNotifyRetry(const std::function<void()>& callback) = 0;
    virtual void OnNotifyCompleted(const std::function<void()>& callback) = 0;

    virtual std::unique_ptr<SpatialOS::IRetryNotifier> Clone() const = 0;
};

}