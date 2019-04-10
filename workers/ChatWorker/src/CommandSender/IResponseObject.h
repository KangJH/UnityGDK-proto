#pragma once

#include <string>

#include "improbable/worker.h"


namespace SpatialOS {

class IResponseObject
{
public:
    virtual ~IResponseObject() = 0 {};

    virtual bool Success() = 0;
    virtual worker::StatusCode StatusCode() = 0;
    virtual const std::string& ErrorMessage() = 0;
};

}