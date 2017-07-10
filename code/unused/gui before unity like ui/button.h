#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "gui/widget.h"
#include "render/resources/texture_atlas_resource.h"

namespace engine
{
enum class ButtonTextAlignment
{
	Top,
	Middle,
	Bottom,
	Left,
	Right
};

class ENGINE_API Button : public Widget
{
public:
	Button();
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;
	inline void setTextAlignment(ButtonTextAlignment align) { textAlignment = align; }

protected:
	TextureAtlasItem* image = nullptr;
	ButtonTextAlignment textAlignment = ButtonTextAlignment::Middle;
};

}