#pragma once

#include <iostream>

#include <improbable/standard_library.h>
#include <improbable/view.h>
#include <improbable/worker.h>

namespace SpatialOS
{

class AuthorityDelegator
{
public:
	AuthorityDelegator(worker::Connection&, worker::View&);
	~AuthorityDelegator() = default;
	void operator=(const AuthorityDelegator&) = delete;
	AuthorityDelegator(const AuthorityDelegator&) = delete;

	// Function for changing write permissions on a specified component for the given entity.
	// Use as follows:
	// ACLWrapper aclWrapper(connection, view);
	// aclWrapper.ChangeComponentWriteAccess<SomeComponent>(entity_id, attribute_name);
	// 
	// Can also set the write permission to the current worker like this:
	// aclWrapper.ChangeComponentWriteAccess<SomeComponent>(entity_id, "workerId:" + worker_id);
	// (Note that you have to append a "workerId:" before giving the worker id)!)
	// (Also note that the worker must have write access over the specific entity's ACL component for it to be able to successfully change permissions on this entity's permissions)
	template <typename TComponent>
	void Delegate(const worker::EntityId& entityId, const std::string& attribute)
	{
		// Setting the 
		improbable::WorkerAttributeSet attribute_set = improbable::WorkerAttributeSet({ attribute });
		improbable::WorkerRequirementSet new_requirements = improbable::WorkerRequirementSet({ attribute_set });

		worker::Option<worker::Entity&> entityFindResult = FindEntityFromId(entityId);

		if (entityFindResult)
		{
			worker::Entity& entity = *entityFindResult;

			auto aclMapping = entity.Get<improbable::EntityAcl>()->component_write_acl();
			aclMapping[TComponent::ComponentId] = new_requirements;

			improbable::EntityAcl::Update component_acl_update;
			component_acl_update.set_component_write_acl(aclMapping);

			connection_.SendComponentUpdate<improbable::EntityAcl>(entityId, component_acl_update);
		}
		else
		{
			std::cerr << "Could not find entity with ID " << entityId << std::endl;
		}
	}

private:
	worker::Connection& connection_;
	worker::View& view_;

	worker::Option<worker::Entity&> FindEntityFromId(const worker::EntityId entity_id);
};

}