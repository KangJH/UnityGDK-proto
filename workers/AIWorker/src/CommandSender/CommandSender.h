#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

#include "improbable/worker.h"

#include "CommandSender/CommandResponse.h"
#include "CommandSender/ICommandStatusReporter.h"
#include "CommandSender/IRetryNotifier.h"
#include "CommandSender/CommandResponseSummary.h"
#include "RequireExternal/ILogger.h"

using Logging = SpatialOS::RequireExternal::ILogger;

namespace SpatialOS {

class IResponseObject;

struct RequestTrackingData
{
    std::unique_ptr<IRetryNotifier> retryNotifier;
    std::function<void(IResponseObject&)> completeCallback;

    unsigned int networkRequestId;
    worker::Option<worker::Dispatcher::CallbackKey> dispatcherCallbackHandle;
};

//CommandSender MUST handle retries - retries should outlive any temp response objects
class CommandSender : public ICommandStatusReporter
{
public:
    CommandSender(std::unique_ptr<IRetryNotifier>&& commandRetryNotifier, worker::Connection& connection, worker::Dispatcher& dispatcher);

    template<typename TCommand>
    CommandResponse<TCommand> Send(worker::EntityId target, const typename TCommand::Request& payload, int timeoutMillis = 1000);

    virtual void OnCompletion(CommandId commandId, const std::function<void(IResponseObject&)>& responseHandler) override;
    virtual void RemoveCallbacks(CommandId commandId) override;

private:
    worker::Connection& spatialConnection;
    worker::Dispatcher& spatialDispatcher;

    CommandId nextId;
    std::unique_ptr<IRetryNotifier> retryNotifier;
    
    //CommandId != RequestId - we generate a handle that persists across multiple requests (command retries)
    std::unordered_map<CommandId, RequestTrackingData> requests;

    
    template<typename TCommand>
    void DoSend(worker::EntityId target, const typename TCommand::Request& payload, int timeoutMillis, CommandId commandId);

    template<typename TCommand>
    void OnResponse(CommandId tracker, const worker::CommandResponseOp<TCommand>& response);
    
    template<typename TCommand>
    void CommandCompleted(CommandId commandId, const worker::CommandResponseOp<TCommand>& response);
};

template<typename TCommand>
CommandResponse<TCommand> CommandSender::Send(worker::EntityId target, const typename TCommand::Request& payload, int timeoutMillis)
{
    CommandId commandId = nextId++;
    
    std::unique_ptr<IRetryNotifier> commandRetryNotifier = retryNotifier->Clone();
    commandRetryNotifier->OnNotifyRetry(std::bind(&CommandSender::DoSend<TCommand>, this, target, payload, timeoutMillis, commandId));

    RequestTrackingData tracker;
    tracker.retryNotifier = std::move(commandRetryNotifier);
    tracker.dispatcherCallbackHandle =
        spatialDispatcher.OnCommandResponse<TCommand>(std::bind(&CommandSender::OnResponse<TCommand>, this, commandId, std::placeholders::_1));

    requests.emplace(commandId, std::move(tracker));

    DoSend<TCommand>(target, payload, timeoutMillis, commandId);

    return CommandResponse<TCommand>(commandId, std::shared_ptr<ICommandStatusReporter>(this));
}

template<typename TCommand>
void CommandSender::DoSend(worker::EntityId target, const typename TCommand::Request& payload, int timeoutMillis, CommandId commandId)
{
    auto request = spatialConnection.SendCommandRequest<TCommand>(target, payload, timeoutMillis);
    requests[commandId].networkRequestId = request.Id;

    Logging::ApplicationLogger->Debug("Sending command: CommandId = " + std::to_string(commandId) + " ; RequestId = " + std::to_string(request.Id));
}

template<typename TCommand>
void CommandSender::OnResponse(CommandId commandId, const worker::CommandResponseOp<TCommand>& response)
{
    if (response.RequestId.Id != requests[commandId].networkRequestId)
    {
        return;
    }

    requests[commandId].retryNotifier->OnNotifyCompleted(std::bind(&CommandSender::CommandCompleted<TCommand>, this, commandId, response));
    requests[commandId].retryNotifier->ProcessResponse(CommandResponseSummary{ response.StatusCode, response.Message });
}

template<typename TCommand>
void CommandSender::CommandCompleted(CommandId commandId, const worker::CommandResponseOp<TCommand>& response)
{
    auto& requestTracking = requests[commandId];
    if (requestTracking.completeCallback)
    {
        requestTracking.completeCallback(CommandResponseObject<TCommand>(response));
    }

    spatialDispatcher.Remove(*(requestTracking.dispatcherCallbackHandle));
    requests.erase(commandId);
}

}