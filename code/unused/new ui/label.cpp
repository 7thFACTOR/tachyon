#include "gui/label.h>
#include "gui/canvas.h>
#include "gui/theme.h>
#include "gui/event.h>

namespace horus
{
void Label::draw(Canvas& canvas)
{
	ThemeWidgetSkin* skin = theme->skins["Label"];
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
		Color color = state->elements["body"].foreColor;

		if (!currentTransition.empty())
		{
			color = transitions[currentTransition].currentValue.color;
		}

		canvas.setFont(state->elements["body"].font);
		canvas.setColor(color);
		canvas.draw(
			text,
			absRect,
			horizAlign,
			vertAlign);
	}
	else if (theme->engine == ThemeEngine::Custom)
	{
		theme->customEngine->drawWidget(WidgetType::Label, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Label::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::MouseButtonDown:
		if (e.button == MouseButton::Left)
		{
			flags.pressed = 1;
			flags.focused = 1;
			flags.captured = 1;
		}
		break;
	case WidgetEventType::MouseButtonUp:
		if (e.button == MouseButton::Left)
		{
			if (absRect.contains(e.windowPoint))
			{
				if (actionCallback)
				{
					actionCallback(this, e, WidgetActionType::Clicked);
				}
			}

			flags.pressed = 0;
			flags.captured = 0;
		}
		break;
	default:
		break;
	}
}

}