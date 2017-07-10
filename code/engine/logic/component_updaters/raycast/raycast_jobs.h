#pragma once
#include "base/job.h"
#include "base/defines.h"
#include "core/defines.h"
#include "base/range.h"
#include "base/math/ray.h"

namespace base
{
struct Task;
}

namespace engine
{
struct TransformComponent;
using namespace base;

class RaycastJob : public Job
{
public:
	void execute(Task& task) override;
	void raycastWorld(const TransformComponent& oldXform);
};

}