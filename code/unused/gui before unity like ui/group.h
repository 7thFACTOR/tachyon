#pragma once

#include <core/defines.h>
#include <core/types.h>
#include <base/array.h>

namespace engine
{
class Widget;

template<typename _WidgetClass>
class Group
{
public:
	void addWidget(_WidgetClass* widget)
	{
		widgets.append(widget);
		widget->setGroup(this);
	}

	void removeWidget(_WidgetClass* widget)
	{
		widget->setGroup(nullptr);
		auto iter = widgets.find(widget);

		if (iter == widgets.end())
		{
			return;
		}

		widgets.erase(iter);
	}

	const Array<_WidgetClass*>& getWidgets() const { return widgets; }

protected:
	Array<_WidgetClass*> widgets;
};

typedef Group<Widget> WidgetGroup;

}