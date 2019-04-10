#pragma once

#include <functional>
#include <vector>

#include "improbable/worker.h"

#include "CommandId.h"
#include "ICommandStatusReporter.h"
#include "CommandResponseObject.h"


namespace SpatialOS {

template<typename TCommand>
class CommandResponse
{
public:
    CommandResponse(CommandId commandId, std::shared_ptr<ICommandStatusReporter> statusReporter)
        : command(commandId),
        status(statusReporter)
    {}

    ~CommandResponse()
    {
		//todo : this is rubbish and crashes on exit
       // status->RemoveCallbacks(command);
    }

    void OnSuccess(const std::function<void(const typename TCommand::Response&)>& callback)
    {
        StartListeningForResult();
        successCallbacks.emplace_back(callback);
    }
    void OnFailure(const std::function<void(worker::StatusCode, const std::string&)>& callback)
    {
        StartListeningForResult();
        failureCallbacks.emplace_back(callback);
    }

private:
    CommandId command;
	std::shared_ptr<ICommandStatusReporter> status;
    std::vector<std::function<void(const typename TCommand::Response&)>> successCallbacks;
    std::vector<std::function<void(worker::StatusCode, const std::string&)>> failureCallbacks;

    void ProcessResponse(IResponseObject& responseObject)
    {
        if (responseObject.Success())
        {
            for (auto& callback : successCallbacks)
            {
                CommandResponseObject<TCommand>& concreteResponse = dynamic_cast<CommandResponseObject<TCommand>&>(responseObject);
                callback(concreteResponse.Data());
            }
        }
        else
        {
            for (auto& callback : failureCallbacks)
            {
                callback(responseObject.StatusCode(), responseObject.ErrorMessage());
            }
        }
    }

    void StartListeningForResult()
    {
        if (!AlreadyListening())
        {
            status->OnCompletion(command, std::bind(&CommandResponse<TCommand>::ProcessResponse, this, std::placeholders::_1));
        }
    }

    bool AlreadyListening()
    {
        return successCallbacks.size() > 0 || failureCallbacks.size() > 0;
    }
};

}