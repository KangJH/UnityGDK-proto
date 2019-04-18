#pragma once
#include <unordered_map>

#include "improbable/standard_library.h"
#include "improbable/worker.h"

namespace SpatialOS {

class QuerySender
{

public:
    QuerySender(worker::Dispatcher&, worker::Connection&);
    ~QuerySender();
    QuerySender(const QuerySender&) = delete;
    void operator=(const QuerySender&) = delete;

    typedef void (*OnQueryResult)(const worker::EntityQueryResponseOp&);

    void Query(const improbable::Coordinates& position, double radius, OnQueryResult callBack);

    /*
     * Query for the specified component on an entity with the given ID.
     */
    template <typename TComponent>
    void Query(worker::EntityId entityId, OnQueryResult callback)
    {
        const worker::query::SnapshotResultType result = SpecificComponentResult<TComponent>();

        // Construct a query passing in entity ID and getting the specified component in return     
        const worker::query::EntityQuery& entityQuery = {
            worker::query::EntityIdConstraint{ entityId },
            result
        };

        auto request_id = connection_.SendEntityQueryRequest(entityQuery, 0);
        requestIdCallbackMap_.emplace(request_id.Id, callback);
    }

    /*
     * Query for ids of all entities with the given component on them (queries the entire world).
     */
    template <typename TComponent>
    void Query(OnQueryResult callback)
    {
        // Construct a query to return IDs for all entities with the given component
        worker::query::ComponentConstraint componentConstraint = worker::query::ComponentConstraint{};
        componentConstraint.ComponentId = TComponent::ComponentId;

        const worker::query::SnapshotResultType result = SpecificComponentResult<TComponent>();

        const worker::query::EntityQuery& entity_query = {
            componentConstraint,
            result
        };

        auto requestId = connection_.SendEntityQueryRequest(entity_query, 0);
        requestIdCallbackMap_.emplace(requestId.Id, callback);
    }

    /*
     * Query for the all entities with the specific component within the specified radius.
     * Returns entity ids and component values.
     */
    template <typename TComponent>
    void Query(const improbable::Coordinates& position, double radius, OnQueryResult callback)
    {
        const worker::query::SnapshotResultType result = SpecificComponentResult<TComponent>();

        // Construct a query to return IDs for all entities with the given component
        worker::query::ComponentConstraint componentConstraint = worker::query::ComponentConstraint{};
        componentConstraint.ComponentId = TComponent::ComponentId;

        // Construct a query to return all entity IDs within a given radius
        worker::query::SphereConstraint sphereConstraint;
        sphereConstraint.Radius = radius;
        sphereConstraint.X = position.x();
        sphereConstraint.Y = position.y();
        sphereConstraint.Z = position.z();

        // Join these constraints together.
        worker::query::AndConstraint radiusAndCoponentConstraint = worker::query::AndConstraint{ sphereConstraint, componentConstraint };

        const worker::query::EntityQuery& entity_query = {
            radiusAndCoponentConstraint,
            result
        };

        auto request_id = connection_.SendEntityQueryRequest(entity_query, 0);
        requestIdCallbackMap_.emplace(request_id.Id, callback);
    }

private:
    worker::Dispatcher& dispatcher_;
    worker::Connection& connection_;
    
    uint64_t dispatcherCallbackHandle;

    // Snapshot query entityIdOnlyResult returning only entity IDs.
    const worker::query::SnapshotResultType entityIdOnlyResult = worker::query::SnapshotResultType{ { {} } };

    // Mapping between query request ID and the according callback
    std::unordered_map<uint32_t, OnQueryResult> requestIdCallbackMap_;

    // Function calling the appropriate callback when query response is received
    void CallbackDistributor(const worker::EntityQueryResponseOp&);

    template <typename TComponent>
    worker::query::SnapshotResultType SpecificComponentResult()
    {
        // Component list containing only the specified component.
        worker::Option<worker::List<unsigned>> componentList = { { TComponent::ComponentId } };
        const worker::query::SnapshotResultType result = worker::query::SnapshotResultType{ componentList };
        return result;
    }

};

}