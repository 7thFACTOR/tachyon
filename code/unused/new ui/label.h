#pragma once

#include <core/defines.h>
#include <core/types.h>
#include <gui/widget.h>
#include <gui/canvas.h>

namespace engine
{
class ENGINE_API Label : public Widget
{
public:
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;

protected:
	HorizontalTextAlignment horizAlign = HorizontalTextAlignment::Left;
	VerticalTextAlignment vertAlign = VerticalTextAlignment::Top;
};

}