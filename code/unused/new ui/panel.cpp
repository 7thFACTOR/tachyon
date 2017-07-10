#include "gui/panel.h>
#include "gui/canvas.h>
#include "gui/theme.h>

namespace horus
{
Panel::Panel()
	: Widget()
{
	flags.focusable = 0;
}

void Panel::draw(Canvas& canvas)
{
	ThemeWidgetSkin* skin = theme->skins["Button"];
	ThemeWidgetState* state = nullptr;

	if (flags.enabled)
	{
		state = &skin->states["normal"];
	}
	else
	{
		state = &skin->states["disabled"];
	}

	if (!state)
	{
		return;
	}

	if (theme->engine == ThemeEngine::Metro)
	{
		canvas.setBlendMode(BlendMode::Opaque);
		canvas.beginBatch(theme->atlas);
		canvas.setColor(state->elements["body"].backColor.scaled(0.5f));
		canvas.draw(state->elements["body"].item, absRect);
		canvas.endBatch();
	}
	else if (theme->engine == ThemeEngine::Custom)
	{
		theme->customEngine->drawWidget(WidgetType::Panel, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Panel::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::MouseButtonDown:
		if (actionCallback)
		{
			actionCallback(this, e, WidgetActionType::Clicked);
		}
		break;
	default:
		break;
	}
}

}