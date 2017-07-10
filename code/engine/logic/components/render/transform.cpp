#include "transform.h"

namespace engine
{
using namespace base;

E_BEGIN_PROPERTIES(TransformComponent)
	E_PROPERTY(useParentTransform, "Use the parent's transform matrix", true, PropertyFlags::None);
	E_PROPERTY(translation, "World or parent local translation", BigVec3(), PropertyFlags::None);
	E_PROPERTY(cameraRelativeTranslation, "", Vec3(), PropertyFlags::None);
	E_PROPERTY(scale, "The scale of the transform", Vec3(1, 1, 1), PropertyFlags::None);
	E_PROPERTY(rotation, "", Quat(), PropertyFlags::None);
	E_PROPERTY(renderLayerIndex, "", 0, PropertyFlags::None);
	E_PROPERTY(levelOfDetail, "", 0, PropertyFlags::None);
E_END_PROPERTIES

}