#pragma once

#include "gui/defines.h>
#include "gui/types.h>
#include "gui/widget.h>

namespace horus
{
class HORUS_API Panel : public Widget
{
public:
	Panel();
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;
};

}