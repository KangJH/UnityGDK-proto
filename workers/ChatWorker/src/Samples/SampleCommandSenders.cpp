#include "SampleCommandSenders.h"

std::unique_ptr<SpatialOS::CommandSender> SpatialOSSamples::SampleCommandSenders::Default = nullptr;
std::unique_ptr<SpatialOS::CommandSender> SpatialOSSamples::SampleCommandSenders::LowPriority = nullptr;