#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "PxPhysics.h"
#include "PxCooking.h"
#include "PxControllerManager.h"
#include "PxCapsuleController.h"
#include "PxExtended.h"
#include "physx_stream.h"

namespace engine
{
enum EPhysXGroups
{
	ePhysXGroup_Collidable,
	ePhysXGroup_NonCollidable,
	ePhysXGroup_CollidablePushable
};
}