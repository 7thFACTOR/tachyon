#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/class_registry.h"
#include "game/types.h"
#include "game/entity_factory.h"
#include "base/math/vec3.h"
#include "base/math/quat.h"
#include "base/math/util.h"

namespace engine
{

//TODO: this should be DEPRECATED, entity factory from JSON editor files. not code
class StdEntitiesFactory: public EntityFactory
{
public:
	StdEntitiesFactory();
	virtual ~StdEntitiesFactory();
	bool composeEntity(EntityId entity, const String& templateName);

protected:
	void composeCamera(EntityId entity);
	void composeLight(EntityId entity);
	void composeBoxTrigger(EntityId entity);
	void composeEllipsoidTrigger(EntityId entity);
	void composeFenceTrigger(EntityId entity);
	void composeModel(EntityId entity);
	void composeSpawnPoint(EntityId entity);
};
}