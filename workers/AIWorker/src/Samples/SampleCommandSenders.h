#pragma once

#include <memory>

#include "CommandSender/CommandSender.h"

namespace SpatialOSSamples {

class SampleCommandSenders
{
public:
    static std::unique_ptr<SpatialOS::CommandSender> Default;
    static std::unique_ptr<SpatialOS::CommandSender> LowPriority;
};

}