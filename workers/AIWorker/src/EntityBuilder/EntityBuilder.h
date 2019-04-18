#pragma once
#include "improbable/worker.h"
#include "improbable/standard_library.h"
#include "improbable/collections.h"

/**
*  Helper class for building a SpatialOS worker::Entity.
*
*  The first method called on the helper must be Begin(), and the last one must be Build(), the rest of the methods can be called in any order.
*
*  Example:
*  worker::Entity entityBuilderEntity = EntityBuilder::Begin()
*  ->AddPosition(improbable::Coordinates(10, 10, 10), someRequirementAttribute)
*  ->AddMetadata("builder_entity", someRequirementAttribute)
*  ->AddComponent<improbable::CustomComponent>(improbable::ship::CustomComponent(), someRequirementAttribute)
*  ->SetPersistence(true)
*  ->SetReadAcl(someRequirementAttributeList)
*  ->SetEntityAclComponentWriteAccess(someRequirementAttribute)
*  ->Build();
*/

namespace SpatialOS {

class EntityBuilder
{
public:
	void operator=(const EntityBuilder&) = delete;
	EntityBuilder(const EntityBuilder&) = delete;
	EntityBuilder(EntityBuilder&&) = default;

	static EntityBuilder Begin();

	EntityBuilder& AddPosition(const improbable::Coordinates& coordinates, const std::string& writeRequirements);
	EntityBuilder& AddMetadata(const std::string& entityType, const std::string& writeRequirements);

	 // Specify whether you want this entity to be captured in snapshots.
	EntityBuilder& SetPersistence(bool persistence);
	 // Specify which workers will get updates about this entity.
	EntityBuilder& SetReadAcl(const std::list<std::string>& readRequirementList);
	// Sets write access over the ACLs for this entity.
	// Whoever has the write access will be able to change the write access over the rest of the components and read access over the entity.
	EntityBuilder& SetEntityAclComponentWriteAccess(const std::string& writeRequirements);

	template <class TComponent>
	EntityBuilder& AddComponent(const typename TComponent::Data& data, const std::string& writeRequirements)
	{
		entity_.Add<TComponent>(data);
		SetWritePermissions<TComponent>(writeRequirements);
		return *this;
	}

	worker::Entity Build()
	{
		entity_.Add<improbable::EntityAcl>(improbable::EntityAcl::Data(read_perms_, write_perms_));
		return entity_;
	}

private:

	worker::Entity entity_;
	worker::Map<uint32_t, improbable::WorkerRequirementSet> write_perms_;
	improbable::WorkerRequirementSet read_perms_;

	EntityBuilder();

	improbable::WorkerRequirementSet CreateRequirementSet(const std::string& attribute);
	improbable::WorkerRequirementSet CreateRequirementSet(const std::list<std::string>& attributeList);

	template <class TComponent>
	void SetWritePermissions(const std::string& writeRequirement)
	{
		write_perms_.emplace(TComponent::ComponentId, CreateRequirementSet(writeRequirement));
	}
};

}