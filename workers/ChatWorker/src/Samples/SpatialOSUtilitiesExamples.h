#pragma once

#include <memory>

namespace worker
{
    class Connection;
    class Dispatcher;
    class View;
}

namespace SpatialOSSamples {

class IScheduler;

void SetupDefaultCommandSenders(worker::Connection& connection, worker::Dispatcher& dispatcher, const std::shared_ptr<SpatialOSSamples::IScheduler>& scheduler);


void QueryExample(worker::Connection& connection, worker::Dispatcher& dispatcher);

void SpawnChatManager(worker::Connection& connection, worker::Dispatcher& dispatcher);

}
