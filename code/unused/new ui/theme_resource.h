#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "base/math/color.h"
#include "base/util.h"

namespace engine
{
using namespace base;

const u32 themeResourceVersion = 1;

typedef u32 ThemeWidgetElementId;
typedef u32 WidgetElementId;
typedef u32 WidgetStateId;
typedef u32 WidgetTypeId;

const u32 hoveredPressedStateId = hashStringSbdm("hovered_pressed");
const u32 hoveredStateId = hashStringSbdm("hovered");
const u32 pressedStateId = hashStringSbdm("pressed");
const u32 normalStateId = hashStringSbdm("normal");
const u32 disabledStateId = hashStringSbdm("disabled");

const u32 bodyElementId = hashStringSbdm("body");

struct ThemeWidgetElement
{
	TextureAtlasResource* atlas = nullptr;
	TextureAtlasItem* item = nullptr;
	Font* font = nullptr;
	Color backColor = Color::white;
	Color foreColor = Color::white;
	Map<String, f32> params;

	f32 getParam(const String& name, f32 defaultValue)
	{
		auto iter = params.find(name);
		
		if (iter != params.end())
		{
			return iter->value;
		}

		return defaultValue;
	}
};

struct GuiThemeWidgetState
{
	Map<GuiWidgetElementId, ThemeWidgetElement> elements;
};

struct GuiThemeWidgetSkin
{
	Map<GuiWidgetStateId, GuiThemeWidgetState> states;
};

enum class WidgetType
{
	Button,
	Check,
	Radio,
	Slider,
	Edit,
	Label,
	Panel,
	Custom,

	Count
};

struct GuiTheme
{
	ResourceRef<TextureAtlasResource> atlas = invalidResourceId;
	ResourceId texture = invalidResourceId;
	Array<ResourceRef<GuiFont>> fonts;
	Map<WidgetTypeId, GuiThemeWidgetSkin*> skins;
};

}