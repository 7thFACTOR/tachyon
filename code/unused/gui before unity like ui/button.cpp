#include "gui/button.h"
#include "gui/canvas.h"
#include "gui/theme.h"
#include "gui/event.h"
#include "base/util.h"

namespace engine
{
const u32 buttonClassId = hashStringSbdm("Button");

Button::Button()
{
	flags.focusable = 1;
}

void Button::draw(Canvas& canvas)
{
	ThemeWidgetSkin* skin = theme->skins[buttonClassId];
	ThemeWidgetState* state = nullptr;

	if (flags.enabled)
	{
		if (flags.hovered)
		{
			if (flags.pressed)
			{
				state = &skin->states[hoveredPressedStateId];
			}
			else
			{
				state = &skin->states[hoveredStateId];
			}
		}
		else
		{
			if (flags.pressed)
			{
				state = &skin->states[pressedStateId];
			}
			else
			{
				state = &skin->states[normalStateId];
			}
		}
	}
	else
	{
		state = &skin->states[disabledStateId];
	}

	if (!state)
	{
		return;
	}

	if (theme->engine == ThemeEngine::Metro)
	{
		auto& bodyElem = state->elements[bodyElementId];
		canvas.beginBatch(theme->atlas);
		canvas.draw(
			bodyElem.item,
			transform.worldRect,
			bodyElem.backColor, AtlasItemSizingPolicy::Stretch);
		Rect rcBtm = transform.worldRect;
		rcBtm.y = transform.worldRect.bottom() - 3;
		rcBtm.height = 3;
		canvas.draw(
			bodyElem.item,
			rcBtm,
			bodyElem.backColor.getScaled(0.7f),
			AtlasItemSizingPolicy::Stretch);
		canvas.endBatch();

		Vec2 offs;

		if (flags.pressed)
		{
			offs.set(1, 1);
		}

		Rect rcText = transform.worldRect;

		HorizontalTextAlignment hta;
		VerticalTextAlignment vta;

		switch (textAlignment)
		{
		case ButtonTextAlignment::Top:
			hta = HorizontalTextAlignment::Center;
			vta = VerticalTextAlignment::Top;
			break;
		case ButtonTextAlignment::Middle:
			hta = HorizontalTextAlignment::Center;
			vta = VerticalTextAlignment::Center;
			break;
		case ButtonTextAlignment::Bottom:
			hta = HorizontalTextAlignment::Center;
			vta = VerticalTextAlignment::Bottom;
			break;
		case ButtonTextAlignment::Left:
			hta = HorizontalTextAlignment::Left;
			vta = VerticalTextAlignment::Center;
			break;
		case ButtonTextAlignment::Right:
			hta = HorizontalTextAlignment::Right;
			vta = VerticalTextAlignment::Center;
			break;
		default:
			break;
		}

		canvas.drawText(bodyElem.font, text, transform.worldRect + offs, bodyElem.foreColor, hta, vta);
	}
	else if (theme->engine == ThemeEngine::Custom)
	{
		theme->customEngine->drawWidget(WidgetType::Button, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Button::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::KeyDown:
		if (flags.focused && (e.key == InputKey::Enter || e.key == InputKey::Space))
		{
			flags.pressed = 1;
			flags.captured = 1;
		}
		break;
	case WidgetEventType::KeyUp:
		if (flags.focused && (e.key == InputKey::Enter || e.key == InputKey::Space))
		{
			if (actionCallback)
			{
				actionCallback(this, e, WidgetActionType::Clicked);
			}

			flags.pressed = 0;
			flags.captured = 0;
		}
		break;
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
			if (only2D)
			{
				//TODO: have a function to check for both 2D and 3D is point is inside worldRect with worldMatrix (for3D)
				if (transform.worldRect.contains(e.windowPoint))
				{
					if (actionCallback)
					{
						actionCallback(this, e, WidgetActionType::Clicked);
					}
				}
			}
			else
			{
				//TODO: 3D
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