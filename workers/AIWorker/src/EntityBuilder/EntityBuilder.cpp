#include "EntityBuilder.h"

using namespace SpatialOS;

EntityBuilder::EntityBuilder(): entity_(), write_perms_(), read_perms_()
{
}

EntityBuilder EntityBuilder::Begin()
{
	return EntityBuilder();
}

EntityBuilder& EntityBuilder::AddPosition(const improbable::Coordinates& coordinates,
	const std::string& writeRequirements)
{
	entity_.Add<improbable::Position>(improbable::PositionData(coordinates));
	SetWritePermissions<improbable::Position>(writeRequirements);
	return *this;
}

EntityBuilder& EntityBuilder::AddMetadata(const std::string& entityType, const std::string& writeRequirements)
{
	entity_.Add<improbable::Metadata>(improbable::MetadataData(entityType));
	SetWritePermissions<improbable::Metadata>(writeRequirements);
	return *this;
}

EntityBuilder& EntityBuilder::SetPersistence(bool persistence)
{
	if (persistence)
	{
		entity_.Add<improbable::Persistence>(improbable::PersistenceData());
	}

	return *this;
}

EntityBuilder& EntityBuilder::SetReadAcl(const std::list<std::string>& readRequirementList)
{
	read_perms_ = CreateRequirementSet(readRequirementList);
	return *this;
}

EntityBuilder& EntityBuilder::SetEntityAclComponentWriteAccess(const std::string& writeRequirements)
{
	SetWritePermissions<improbable::EntityAcl>(writeRequirements);
	return *this;
}

improbable::WorkerRequirementSet EntityBuilder::CreateRequirementSet(const std::string& attribute)
{
	improbable::WorkerAttributeSet attributeSet = improbable::WorkerAttributeSet({ attribute });
	improbable::WorkerRequirementSet requiremnetSet = improbable::WorkerRequirementSet({ attributeSet });
	return requiremnetSet;
}

improbable::WorkerRequirementSet EntityBuilder::CreateRequirementSet(const std::list<std::string>& attributeList)
{
	worker::List<improbable::WorkerAttributeSet> attributeSetList;
	for (auto& attribute : attributeList)
	{
		attributeSetList.emplace_back(worker::List<std::string>({ attribute }));
	}

	improbable::WorkerRequirementSet requiremnetSet = improbable::WorkerRequirementSet(attributeSetList);
	return requiremnetSet;
}
