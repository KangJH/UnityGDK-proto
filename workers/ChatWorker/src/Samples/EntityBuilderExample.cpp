#include "Samples/SpatialOSUtilitiesExamples.h"

#include <set>
#include <cstdlib>
#include "improbable/standard_library.h"
#include "improbable/worker.h"
#include "EntityBuilder/EntityBuilder.h"
#include "Samples/SampleLogger.h"
#include <player/Chat.h>

using Logging = SpatialOS::RequireExternal::ILogger;

std::set<uint32_t> createRequests;
// Sends a reserve entity id request, the actually entity creation request takes place in the OnReserveEntityIdResponse callback
void OnEntityCreatedResponse(const worker::CreateEntityResponseOp& op)
{
    if (createRequests.find(op.RequestId.Id) == createRequests.end())
    {
        //this wasn't sent by us
        return;
    }

    if (op.StatusCode == worker::StatusCode::kSuccess)
    {
        Logging::ApplicationLogger->Info("Create entity with ID " + std::to_string(*(op.EntityId)));
    }
    else
    {
        Logging::ApplicationLogger->Info("Create entity failed with error: " + op.Message);
    }
    createRequests.erase(op.RequestId.Id);
}

void SpatialOSSamples::SpawnChatManager(worker::Connection& connection, worker::Dispatcher& dispatcher)
{
	static auto dispatcherCallbackHandle = dispatcher.OnCreateEntityResponse(&OnEntityCreatedResponse);

	std::string metadataName = "chatWorker";
	std::string managedAttributeName = "Player";
	std::string workerAttributeName = "ChatWorker";
	std::list<std::string> readAttributeList = { managedAttributeName, workerAttributeName };

	worker::Entity entity = SpatialOS::EntityBuilder::Begin()
		.AddPosition(improbable::Coordinates(rand() % 101, rand() % 101, rand() % 101), managedAttributeName)
		.AddMetadata(metadataName, workerAttributeName)
		.AddComponent<player::Chat>({}, workerAttributeName)
		.SetPersistence(false)
		.SetReadAcl(readAttributeList)
		.SetEntityAclComponentWriteAccess(workerAttributeName)
		.Build();

	// Create the entity
	worker::RequestId<worker::CreateEntityRequest> createRequest = connection.SendCreateEntityRequest(entity, worker::Option<worker::EntityId>(), 5000);
	createRequests.emplace(createRequest.Id);
}
