#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/class_registry.h"

namespace engine
{
using namespace base;

class PhysicsJoint;
class PhysicsBody;
class PhysicsSpace;

//! This enum is used for the quality of the physics simulation
enum class PhysicsQuality
{
	High,
	Medium,
	Low
};

struct PhysicsSettings
{
	PhysicsQuality quality = PhysicsQuality::High;
	u32 iterations = 1;
};

class E_API Physics
{
public:
	Physics() {}
	virtual ~Physics() {}
	virtual bool initialize() { return true; };
	virtual void shutdown() {}
	virtual PhysicsSpace* createPhysicsSpace() { return nullptr; }
	const Array<PhysicsSpace*>& getSpaces() const { return spaces; }
	PhysicsSpace* getCurrentSpace() const { return currentSpace; }
	virtual void setCurrentSpace(PhysicsSpace* space) { currentSpace = space; }
	virtual void removePhysicsSpace(PhysicsSpace* space) {}
	virtual void setPhysicsSettings(const PhysicsSettings& value) {}
	virtual void simulate(f32 step) {}

protected:
	Array<PhysicsSpace*> spaces;
	PhysicsSpace* currentSpace = nullptr;
};

}