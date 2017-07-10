#pragma once

#include <core/defines.h>
#include <core/types.h>
#include <gui/widget.h>

namespace engine
{
class ENGINE_API Check : public Widget
{
public:
	Check();
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;

	void setChecked(bool value) { checked = value; }
	bool isChecked() const { return checked; }

protected:
	bool checked = false;
};

}