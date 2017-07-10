#include "logic/logic_state_serializer.h"

namespace engine
{
bool LogicStateSerializer::saveGame(SavedGameSlot& slot, Serializable* customDataWriter)
{
	B_ASSERT_NOT_IMPLEMENTED
	return true;
}

bool LogicStateSerializer::loadGame(const String& name, Serializable* customDataReader)
{
	B_ASSERT_NOT_IMPLEMENTED
	return true;
}

void LogicStateSerializer::loadSavedGames(const String& path)
{
	B_ASSERT_NOT_IMPLEMENTED
}

}