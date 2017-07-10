#pragma once

#include "gui/defines.h>
#include "gui/types.h>
#include "gui/widget.h>
#include "gui/group.h>

namespace horus
{
class HORUS_API Radio : public Widget
{
public:
	void handleEvent(WidgetEvent& e) override;
	void draw(Canvas& canvas) override;

	void setSelected(bool value) { selected = value; }
	bool isSelected() const { return selected; }
	void setGroup(Group<Radio>* grp) { group = grp; }

protected:
	bool selected = false;
	Group<Radio>* group = nullptr;
};

}