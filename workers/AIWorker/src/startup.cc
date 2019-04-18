#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include <improbable/view.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>

#include "Samples/SampleLogger.h"
#include "Samples/SampleScheduler.h"

#include "Samples/SpatialOSUtilitiesExamples.h"

#include <npc/State.h>
#include "AIWorker.h"

using namespace improbable;
using namespace npc;

using Logging = SpatialOS::RequireExternal::ILogger;

// Use this to make a worker::ComponentRegistry.
// For example use worker::Components<improbable::Position, improbable::Metadata> to track these common components
using MyComponents = worker::Components<EntityAcl, Metadata, Persistence, Position, State>;

// Constants and parameters
const std::string kLoggerName = "startup.cc";
const std::uint32_t kGetOpListTimeoutInMilliseconds = 100;

std::string GetRandomCharacters(size_t count) {
    const auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const auto max_index = sizeof(charset) - 1;
        return charset[rand() % max_index];
    };
    std::string str(count, 0);
    std::generate_n(str.begin(), count, randchar);
    return str;
}

worker::Connection GetConnection(const std::vector<std::string>& inArguments, bool inManualConnection = false)
{
	worker::ConnectionParameters parameters;
	parameters.WorkerType = inArguments[2];
	parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
	parameters.Network.UseExternalIp = false;

	// When running as an external worker using 'spatial local worker launch'
	// The WorkerId isn't passed, so we generate a random one
	std::string workerId = inManualConnection ? parameters.WorkerType + "_" + GetRandomCharacters(4) : inArguments[3];

	Logging::ApplicationLogger->Debug("[local] Connecting to SpatialOS as " + workerId + "...");

	return worker::Connection::ConnectAsync(MyComponents{}, inArguments[0], atoi(inArguments[1].c_str()), workerId, parameters).Get();
}

// Entry point
int main(int argc, char** argv) {
    srand(time(nullptr));
	SpatialOS::RequireExternal::ILogger::ApplicationLogger = std::make_unique<SpatialOSSamples::SampleLogger>("worker.log");

    Logging::ApplicationLogger->Debug("[local] Worker started.");
	std::vector<std::string> arguments;
	if (argc < 4)
	{
		Logging::ApplicationLogger->Error("[AIWorker] You put wrong arguments.");
		for (int i = 1; i < argc; i++)
		{
			std::string temp(*argv + i);
			Logging::ApplicationLogger->Error("Your Input: " + temp);
		}
		Logging::ApplicationLogger->Error("[AIWorker] Please check spatial.AIWorker.worker.json.");
		return -1;
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			std::string temp(argv[i]);
			arguments.push_back(temp);
		}
	}
    
	worker::Connection connection = IsDebuggerPresent() ? GetConnection(arguments, true) : GetConnection(arguments);
	
    // Create a view
    worker::View view{ MyComponents{} };

    view.OnDisconnect([&](const worker::DisconnectOp& op) {
        Logging::ApplicationLogger->Error("[disconnect] " + op.Reason);
    });

    // Print log messages received from SpatialOS
    view.OnLogMessage([&](const worker::LogMessageOp& op) {
        if (op.Level == worker::LogLevel::kFatal) {
            Logging::ApplicationLogger->Error("Fatal error: " + op.Message);
            std::terminate();
        }
        Logging::ApplicationLogger->Debug("[remote] " + op.Message);
    });


    if (connection.IsConnected()) {
        Logging::ApplicationLogger->Debug("[local server] Connected successfully to SpatialOS, listening to ops... ");
    }
	else
	{
		return -1;
	}

	//Register callback for component
	AIWorker& aiWorker = AIWorker::GetInstance();
	aiWorker.Initialize(view, connection);
	auto lastTime = std::chrono::system_clock::now().time_since_epoch();
	std::shared_ptr<SpatialOSSamples::SampleScheduler> scheduler = std::make_shared<SpatialOSSamples::SampleScheduler>(0);
	
	scheduler->ScheduleRepeat(&(AIWorker::Process), 10000);

    // Run the main worker loop
    while (connection.IsConnected()) {
        auto ops = connection.GetOpList(kGetOpListTimeoutInMilliseconds);
        view.Process(ops);
		auto currentTime = std::chrono::system_clock::now().time_since_epoch();
		auto deltaTime = currentTime - lastTime;
		std::chrono::milliseconds deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime);
		lastTime = currentTime;
		scheduler->Update(deltaMs.count());
    }

    return 1;
}


