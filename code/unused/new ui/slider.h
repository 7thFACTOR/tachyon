#pragma once

#include "gui/defines.h>
#include "gui/types.h>
#include "gui/widget.h>
#include "gui/group.h>

namespace horus
{
class HORUS_API Slider : public Widget
{
public:
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;
	void setMinMax(f32 minVal, f32 maxVal) { minValue = minVal; maxValue = maxVal; }
	f32 getValue() const { return currentValue; }

protected:
	void computeValue(const Point& mousePos);
	void clampValue();

	f32 keyboardStep = 0.1f;
	f32 minValue = 0.0f;
	f32 maxValue = 1.0f;
	f32 currentValue = 0.0f;
	Rect handleRect;
	bool movingHandle = false;
};

}