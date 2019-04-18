#include "SpatialOSUtilitiesExamples.h"

#include "improbable/worker.h"
#include "improbable/standard_library.h"
#include "QuerySender/QuerySender.h"
#include <iostream>

#include "Samples/SampleLogger.h"

using Logging = SpatialOS::RequireExternal::ILogger;

std::unique_ptr<SpatialOS::QuerySender> query;

void  SpatialOSSamples::QueryExample(worker::Connection& connection, worker::Dispatcher& dispatcher)
{
// 	query = std::make_unique<SpatialOS::QuerySender>(dispatcher, connection);
// 
// 	query->Query<improbable::npc::Movement>([](const worker::EntityQueryResponseOp& op)
// 	{
// 		std::string logPrefix = "[QUERY][Find entities with movement component]";
// 		if (op.StatusCode != worker::StatusCode::kSuccess)
// 		{
// 			Logging::ApplicationLogger->Info("");
// 			Logging::ApplicationLogger->Warn(logPrefix + " failed with error: " + op.Message);
// 			return;
// 		}
// 
// 		Logging::ApplicationLogger->Info("");
// 		Logging::ApplicationLogger->Info(logPrefix + " found " + std::to_string(op.ResultCount) + " entities");
// 
// 
// 		for (auto& result : op.Result)
// 		{
// 			worker::EntityId id = result.first;
// 			auto positionData = result.second.Get<improbable::Position>();
// 			auto componentData = *(result.second.Get<improbable::npc::Movement>());
// 			Logging::ApplicationLogger->Info(logPrefix + " Entity " + std::to_string(result.first) + " angle = " + std::to_string(componentData.angle()));
// 		}
// 	});
}