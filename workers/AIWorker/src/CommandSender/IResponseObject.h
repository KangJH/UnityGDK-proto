#pragma once

#include <string>

#include "improbable/worker.h"


namespace SpatialOS {

class IResponseObject
{
public:
#if defined (_MSC_VER)
	virtual ~IResponseObject() = 0 {};
#else
	virtual ~IResponseObject() {};
#endif    
	virtual bool Success() = 0;
    virtual worker::StatusCode StatusCode() = 0;
    virtual const std::string& ErrorMessage() = 0;
};

}