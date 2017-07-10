#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "logic/components/ui/font.h"
#include "core/types.h"
#include "logic/component.h"

namespace engine
{
using namespace base;

struct Font;
struct FontTextSize;

enum class HorizontalTextAlignment
{
	Left,
	Center,
	Right
};

enum class VerticalTextAlignment
{
	Top,
	Center,
	Bottom
};

enum class HorizontalTextOverflow
{
	Overflow,
	Wrap
};

enum class VerticalTextOverflow
{
	Truncate,
	Overflow
};

struct TextComponent : Component
{
	static const ComponentTypeId typeId = (ComponentTypeId)StdComponentTypeId::Text;
	
	String text;
	Font* font = nullptr;
	FontStyle fontStyle = FontStyle::Normal;
	i32 fontSize = 10;
	f32 lineSpacing = 1;
	bool richText = false;
	Color color = Color::white;
	ResourceId material = nullResourceId;
	HorizontalTextAlignment horizontalAlign = HorizontalTextAlignment::Left;
	VerticalTextAlignment verticalAlign = VerticalTextAlignment::Top;
	HorizontalTextOverflow horizontalOverflow = HorizontalTextOverflow::Overflow;
	VerticalTextOverflow verticalOverflow = VerticalTextOverflow::Overflow;
}

}