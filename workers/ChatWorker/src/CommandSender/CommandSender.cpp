#include "CommandSender.h"

using namespace SpatialOS;

CommandSender::CommandSender(std::unique_ptr<IRetryNotifier>&& commandRetryNotifier, worker::Connection& connection, worker::Dispatcher& dispatcher)
    : retryNotifier(std::move(commandRetryNotifier)),
    nextId{ 0 },
    spatialConnection(connection),
    spatialDispatcher(dispatcher)
{
}

void CommandSender::OnCompletion(CommandId commandId, const std::function<void(IResponseObject&)>& responseHandler)
{	
    requests[commandId].completeCallback = responseHandler;
}

void CommandSender::RemoveCallbacks(CommandId commandId)
{
    if (requests.find(commandId) == requests.end())
    {
        return;
    }

    requests[commandId].completeCallback = std::function<void(IResponseObject&)>();
}