#include "QuerySender.h"

using namespace std::placeholders;
using namespace SpatialOS;

QuerySender::QuerySender(worker::Dispatcher& dispatcher, worker::Connection& connection): dispatcher_(dispatcher), connection_(connection)
{
    const auto binder = std::bind(&QuerySender::CallbackDistributor, this, _1);
    dispatcherCallbackHandle = dispatcher_.OnEntityQueryResponse(binder);
}

QuerySender::~QuerySender()
{
	//ToDo: Remove this safely before dispatcher gets destroyed :)
    //dispatcher_.Remove(dispatcherCallbackHandle);
}

/*
 * Query for ids of all entities within the given radius.
 */
void QuerySender::Query(const improbable::Coordinates& position, double radius, OnQueryResult callback)
{
    // Construct a query to return all entity IDs within a given radius
    worker::query::SphereConstraint sphereConstraint;
    sphereConstraint.Radius = radius;
    sphereConstraint.X = position.x();
    sphereConstraint.Y = position.y();
    sphereConstraint.Z = position.z();

    const worker::query::EntityQuery& entity_query = {
        sphereConstraint,
        entityIdOnlyResult
    };

    auto request_id = connection_.SendEntityQueryRequest(entity_query, 0);
    requestIdCallbackMap_.emplace(request_id.Id, callback);
}

/*
 * Find and call the callback passed in for the request with this ID.
 */
void QuerySender::CallbackDistributor(const worker::EntityQueryResponseOp& op)
{
    if (requestIdCallbackMap_.find(op.RequestId.Id) == requestIdCallbackMap_.end())
    {
        return;
    }

    requestIdCallbackMap_[op.RequestId.Id](op);
    requestIdCallbackMap_.erase(op.RequestId.Id);
}

