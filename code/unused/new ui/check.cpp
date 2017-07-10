#include "gui/check.h>
#include "gui/canvas.h>
#include "gui/theme.h>
#include "gui/event.h>

namespace horus
{
Check::Check()
{
	flags.focusable = 1;
}

void Check::draw(Canvas& canvas)
{
	ThemeWidgetSkin* skin = theme->skins["Button"];
	ThemeWidgetState* state = nullptr;

	if (flags.enabled)
	{
		if (flags.hovered)
		{
			if (flags.pressed)
			{
				state = &skin->states["hovered_pressed"];
			}
			else
			{
				state = &skin->states["hovered"];
			}
		}
		else
		{
			if (flags.pressed)
			{
				state = &skin->states["pressed"];
			}
			else
			{
				state = &skin->states["normal"];
			}
		}
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
		f32 bulletSize = state->elements["body"].getParam("bulletSize", 14.0f);
		f32 bulletCheckScale = state->elements["body"].getParam("bulletCheckScale", 0.8f);

		Rect bulletRc(absRect.x, absRect.center().y - bulletSize / 2.0f, bulletSize, bulletSize);
		Point bulletRcCenter = bulletRc.center();

		canvas.setBlendMode(BlendMode::Opaque);
		canvas.beginBatch(theme->atlas);
		canvas.setColor(state->elements["body"].backColor);
		canvas.draw(state->elements["body"].item, bulletRc);
		Rect rcChk = bulletRc;

		if (checked)
		{
			rcChk.inflate(-bulletRc.width * bulletCheckScale, -bulletRc.height * bulletCheckScale);
			canvas.setColor(Color::white);
			canvas.draw(state->elements["body"].item, rcChk);
		}

		canvas.endBatch();

		Point offs;

		if (flags.pressed)
		{
			offs.set(1, 1);
		}

		canvas.setFont(state->elements["body"].font);

		Color colorText = state->elements["body"].foreColor.scaled(.8f);

		if (flags.hovered || flags.pressed || checked)
		{
			colorText = Color::white;
		}

		canvas.draw(
			text,
			Point(
				bulletRc.right() + state->elements["body"].getParam("bulletTextSpacing", 5.0f),
				bulletRc.center().y) + offs,
			HorizontalTextAlignment::Right,
			VerticalTextAlignment::Center);
	}
	else if (theme->engine == ThemeEngine::Custom)
	{
		theme->customEngine->drawWidget(WidgetType::Check, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Check::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::KeyDown:
		if (flags.focused && (e.key == KeyCode::Enter || e.key == KeyCode::Space))
		{
			flags.pressed = 1;
			flags.captured = 1;
		}
		break;
	case WidgetEventType::KeyUp:
		if (flags.focused && (e.key == KeyCode::Enter || e.key == KeyCode::Space))
		{
			checked = !checked;

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
			if (absRect.contains(e.windowPoint))
			{
				checked = !checked;

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