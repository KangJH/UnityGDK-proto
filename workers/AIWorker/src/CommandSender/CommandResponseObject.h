#pragma once

#include <string>

#include "improbable/worker.h"

#include "IResponseObject.h"


namespace SpatialOS {

template<typename TCommand>
class CommandResponseObject : public IResponseObject
{
public:
    CommandResponseObject(const worker::CommandResponseOp<TCommand>& commandResponse)
        : response(commandResponse)
    {
    }

    virtual bool Success() override { return response.StatusCode == worker::StatusCode::kSuccess; }
    virtual worker::StatusCode StatusCode() override { return response.StatusCode; }
    virtual const std::string& ErrorMessage() override { return response.Message; }

    const typename TCommand::Response& Data()
    {
        return *(response.Response);
    }

private:
    const worker::CommandResponseOp<TCommand>& response;
};

}