#pragma once
#include "base/array.h"
#include "base/map.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "core/resource.h"
#include "game/types.h"
#include "game/game_system.h"

namespace engine
{
class Flowgraph;

//! The flow graph manager keeps all flow graphs in one place and process them per frame,
//! triggering the events when needed
class E_API FlowgraphSystem : public GameSystem
{
public:
	enum { GameSystemType = GameSystemType::Flowgraph };

	B_DECLARE_DERIVED_CLASS(FlowgraphSystem, GameSystem);

	FlowgraphSystem();
	virtual ~FlowgraphSystem();
	void update();

protected:
};

}