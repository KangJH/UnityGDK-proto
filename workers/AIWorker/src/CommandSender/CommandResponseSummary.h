#pragma once

#include <string>

#include "improbable/worker.h"


namespace SpatialOS {

struct CommandResponseSummary
{
    worker::StatusCode status;
    std::string message;
};

}