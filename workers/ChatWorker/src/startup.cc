#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include <chrono>
#include <improbable/view.h>
#include <improbable/ship/ShipControls.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include "improbable/core/Rotation.h"
#include "improbable/player/CommandsTest.h"
#include "improbable/test/TestComponent.h"

#include "Samples/SampleLogger.h"
#include "Samples/SampleScheduler.h"

#include "Samples/SpatialOSUtilitiesExamples.h"

//#include "SpatialOSUtilities/EntityBuilder/EntityBuilder.h"
//#include "SpatialOSUtilities/QuerySender/QuerySender.h"
#include <improbable/chat/Chat.h>


using namespace improbable;

using Logging = SpatialOS::RequireExternal::ILogger;

// Use this to make a worker::ComponentRegistry.
// For example use worker::Components<improbable::Position, improbable::Metadata> to track these common components
using MyComponents = worker::Components<EntityAcl, Metadata, Persistence, Position, chat::Chat>;

// Constants and parameters
const int ErrorExitStatus = 1;
const std::string kLoggerName = "startup.cc";
const std::uint32_t kGetOpListTimeoutInMilliseconds = 100;
const std::uint32_t kCommandTimeoutMilliseconds = 500;

void TransmitMessage(worker::Connection& connection, worker::EntityId receiverID, std::string& message);

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

bool HasChatComponent(const worker::Entity& entity)
{
    worker::Option<const chat::Chat::Data&> chat = entity.Get<chat::Chat>();
    return !chat.empty();
}

worker::EntityId GetChatManager(const worker::View& dispatcher)
{
	worker::EntityId ret = 0;
	for (auto& entityData : dispatcher.Entities)
	{
		auto entity_id = entityData.first;
		auto& entity = entityData.second;
		worker::Option<const MetadataData&> metadata = entity.Get<Metadata>();
		if (!metadata.empty())
		{
			std::string entity_type = (*metadata).entity_type();
			if (entity_type.compare("chatManager") == 0)
			{
				ret = entity_id;
				break;
			}
		}
		else
		{
			Logging::ApplicationLogger->Debug("NO Meta");
		}
	}
	return ret;
}

void PrintUsage()
{
    Logging::ApplicationLogger->Debug("Usage: Managed receptionist <hostname> <port> <worker_id>");
    Logging::ApplicationLogger->Debug("\n");
    Logging::ApplicationLogger->Debug("Connects to SpatialOS");
    Logging::ApplicationLogger->Debug("    <hostname>      - hostname of the receptionist or locator to connect to.");
    Logging::ApplicationLogger->Debug("    <port>          - port to use if connecting through the receptionist.");
    Logging::ApplicationLogger->Debug("    <logfile>       - log file to use.");
    Logging::ApplicationLogger->Debug("    <worker_id>     - (optional) name of the worker assigned by SpatialOS.");
    Logging::ApplicationLogger->Debug("\n");
}

worker::Connection GetConnection(const std::vector<std::string>& inArguments, bool inManualConnection = false)
{
	worker::ConnectionParameters parameters;
	parameters.WorkerType = "Server";
	parameters.Network.ConnectionType = worker::NetworkConnectionType::kTcp;
	parameters.Network.UseExternalIp = false;

	// When running as an external worker using 'spatial local worker launch'
	// The WorkerId isn't passed, so we generate a random one
	std::string workerId = inManualConnection ? parameters.WorkerType + "_" + GetRandomCharacters(4) : inArguments[2];

	Logging::ApplicationLogger->Debug("[local] Connecting to SpatialOS as " + workerId + "...");

	return worker::Connection::ConnectAsync(MyComponents{}, inArguments[0], atoi(inArguments[1].c_str()), workerId, parameters).Get();
}

// Entry point
int main(int argc, char** argv) {
    srand(time(nullptr));
	SpatialOS::RequireExternal::ILogger::ApplicationLogger = std::make_unique<SpatialOSSamples::SampleLogger>("worker.log");

    Logging::ApplicationLogger->Debug("[local] Worker started.");
    

    // if no arguments are supplied, use the defaults for a local deployment
	std::vector<std::string> arguments = argc == 1 ? std::vector<std::string>{ "localhost", "7777", "Server" } : std::vector<std::string>(argv + 1, argv + argc);
    
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

	//Register callback for component
	view.OnCommandRequest<chat::Chat::Commands::OutgoingMessage>([&](const worker::CommandRequestOp<chat::Chat::Commands::OutgoingMessage>& op) {
		auto message_receiver = op.Request.receiver();
		auto message = op.Request.message();
		if (message_receiver.all())
		{
			for (auto& entityData : view.Entities)
			{
				auto entity_id = entityData.first;
				worker::Entity& entity = entityData.second;
				if (HasChatComponent(entity) && view.GetAuthority<chat::Chat>(entity_id) == worker::Authority::kAuthoritative)
				{
					TransmitMessage(connection, entity_id, message);
				}
			}
		}
		else
		{
			if (message_receiver.group_id() != -1)
			{
				// To do
			}
			else
			{
				TransmitMessage(connection, message_receiver.single_target(), message);
			}
		}
		Logging::ApplicationLogger->Debug("[remote] Sender: " + std::to_string(op.EntityId));
		Logging::ApplicationLogger->Debug("[remote] Receiver: " + std::to_string(op.Request.receiver().single_target()));
		Logging::ApplicationLogger->Debug("[remote] Msg : " + op.Request.message());
		chat::Chat::Commands::OutgoingMessage::Response ret = chat::Chat::Commands::OutgoingMessage::Response::Create();
		connection.SendCommandResponse<chat::Chat::Commands::OutgoingMessage>(op.RequestId, ret);
	});

    if (connection.IsConnected()) {
        Logging::ApplicationLogger->Debug("[local server] Connected successfully to SpatialOS, listening to ops... ");
    }
	else
	{
		return -1;
	}
    
    auto lastTime = std::chrono::system_clock::now().time_since_epoch();
    std::shared_ptr<SpatialOSSamples::SampleScheduler> scheduler = std::make_shared<SpatialOSSamples::SampleScheduler>(0);
	
	
	
	SpatialOSSamples::SetupDefaultCommandSenders(connection, view, scheduler);

	
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

void TransmitMessage(worker::Connection& connection, worker::EntityId receiverID, std::string& message)
{
	chat::Chat::Update update;
	chat::MessageEvent event{ message };
	update.add_incoming_message(event);
	connection.SendComponentUpdate<chat::Chat>(receiverID, update);	
}


