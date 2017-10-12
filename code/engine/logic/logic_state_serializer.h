// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/types.h"
#include "base/string.h"
#include "core/defines.h"
#include "base/serialize.h"
#include "base/array.h"

namespace engine
{
using namespace base;

struct SavedGameSlot
{
	String name;
	String level;
	f64 levelTime;
};

class E_API LogicStateSerializer
{
public:
	bool saveGame(SavedGameSlot& slot, Serializable* customDataWriter = nullptr);
	bool loadGame(const String& name, Serializable* customDataReader = nullptr);
	void loadSavedGames(const String& path);

protected:
	String savePath;
	Array<SavedGameSlot> savedGames;
};

}