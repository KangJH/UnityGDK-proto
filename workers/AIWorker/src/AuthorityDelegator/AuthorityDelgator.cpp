#include "AuthorityDelgator.h"
#include <iostream>

using namespace SpatialOS;

AuthorityDelegator::AuthorityDelegator(worker::Connection& connection, worker::View& view): connection_(connection), view_(view)
{
}

worker::Option<worker::Entity&> AuthorityDelegator::FindEntityFromId(const worker::EntityId entity_id)
{
    auto entry = view_.Entities.find(entity_id);
    if (entry != view_.Entities.end())
    {
        return view_.Entities[entity_id];
    }

	return worker::Option<worker::Entity&>();
}
