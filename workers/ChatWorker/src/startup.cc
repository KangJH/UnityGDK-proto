#include <improbable/worker.h>
#include <improbable/standard_library.h>
#include <iostream>
#include <chrono>
#include <improbable/view.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>

#include "Samples/SampleLogger.h"
#include "Samples/SampleScheduler.h"

#include "Samples/SpatialOSUtilitiesExamples.h"

//#include "EntityBuilder/EntityBuilder.h"
//#include "QuerySender/QuerySender.h"
#include <player/Chat.h>


using namespace improbable;

using Logging = SpatialOS::RequireExternal::ILogger;

// Use this to make a worker::ComponentRegistry.
// For example use worker::Components<improbable::Position, improbable::Metadata> to track these common components
using MyComponents = worker::Components<EntityAcl, Metadata, Persistence, Position, player::Chat>;

// Constants and parameters
const int ErrorExitStatus = 1;
const std::string kLoggerName = "startup.cc";
const std::uint32_t kGetOpListTimeoutInMilliseconds = 100;
const std::uint32_t kCommandTimeoutMilliseconds = 500;

void TransmitMessage(worker::Connection& connection, worker::EntityId senderID, worker::EntityId receiverID, std::string& message);

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
    worker::Option<const player::Chat::Data&> chat = entity.Get<player::Chat>();
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
			if (entity_type.compare("chatWorker") == 0)
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
		Logging::ApplicationLogger->Error("[ChatWorker] You put wrong arguments.");
		for (int i = 1; i < argc; i++)
		{
			std::string temp(*argv + i);
			Logging::ApplicationLogger->Error("Your Input: " + temp);
		}
		Logging::ApplicationLogger->Error("[ChatWorker] Please check spatial.ChatWorker.worker.json.");
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

	//Register callback for component
	view.OnCommandRequest<player::Chat::Commands::OutgoingMessage>([&](const worker::CommandRequestOp<player::Chat::Commands::OutgoingMessage>& op) {
		auto message_receiver = op.Request.receiver();
		auto sender = op.EntityId;
		auto message = op.Request.message();
		if (message_receiver.all())
		{
			for (auto& entityData : view.Entities)
			{
				auto entity_id = entityData.first;
				worker::Entity& entity = entityData.second;
				if (HasChatComponent(entity) && view.GetAuthority<player::Chat>(entity_id) == worker::Authority::kAuthoritative)
				{
					TransmitMessage(connection, sender, entity_id, message);
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
				TransmitMessage(connection, sender, message_receiver.single_target(), message);
			}
		}
		player::Chat::Commands::OutgoingMessage::Response ret = player::Chat::Commands::OutgoingMessage::Response::Create();
		connection.SendCommandResponse<player::Chat::Commands::OutgoingMessage>(op.RequestId, ret);
	});

    if (connection.IsConnected()) {
        Logging::ApplicationLogger->Debug("[local server] Connected successfully to SpatialOS, listening to ops... ");
    }
	else
	{
		return -1;
	}
    
	
    // Run the main worker loop
    while (connection.IsConnected()) {
        auto ops = connection.GetOpList(kGetOpListTimeoutInMilliseconds);
        view.Process(ops);
		
    }

    return 1;
}

void TransmitMessage(worker::Connection& connection, worker::EntityId senderID, worker::EntityId receiverID, std::string& message)
{
	player::Chat::Update update;
	player::MessageEvent event(senderID, message);
	update.add_incoming_message(event);
	connection.SendComponentUpdate<player::Chat>(receiverID, update);
}


