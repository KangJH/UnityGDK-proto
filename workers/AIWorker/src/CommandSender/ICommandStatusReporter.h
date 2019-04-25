#pragma once

#include <functional>

#include "CommandId.h"


namespace SpatialOS {

class IResponseObject;

class ICommandStatusReporter
{
public:
#if defined (_MSC_VER)
	virtual ~ICommandStatusReporter() = 0 {};
#else
	virtual ~ICommandStatusReporter() {};
#endif
    

    virtual void OnCompletion(CommandId commandId, const std::function<void (IResponseObject&)>& responseHandler) = 0;
    virtual void RemoveCallbacks(CommandId commandId) = 0;
};

}