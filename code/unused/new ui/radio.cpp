#include "gui/radio.h>
#include "gui/canvas.h>
#include "gui/theme.h>
#include "gui/event.h>

namespace horus
{
void Radio::draw(Canvas& canvas)
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
		canvas.setBlendMode(BlendMode::Opaque);
		canvas.beginBatch(theme->atlas);
		canvas.setColor(state->elements["body"].backColor);

		Rect bulletRc(absRect.x, absRect.center().y - bulletSize / 2.0f, bulletSize, bulletSize);
		Point bulletRcCenter = bulletRc.center();
		Point pts[4] =
		{
			{ bulletRc.x, bulletRcCenter.y },
			{ bulletRcCenter.x, bulletRc.y },
			{ bulletRc.right(), bulletRcCenter.y },
			{ bulletRcCenter.x, bulletRc.bottom() }
		};

		canvas.drawQuad(pts[0], pts[1], pts[2], pts[3]);

		if (selected)
		{
			canvas.setColor(state->elements["body"].foreColor);
			pts[0] = { bulletRc.x + bulletRc.width * bulletCheckScale, bulletRcCenter.y };
			pts[1] = { bulletRcCenter.x, bulletRc.y + bulletRc.height * bulletCheckScale };
			pts[2] = { bulletRc.right() - bulletRc.width * bulletCheckScale, bulletRcCenter.y };
			pts[3] = { bulletRcCenter.x, bulletRc.bottom() - bulletRc.height * bulletCheckScale };
			canvas.drawQuad(pts[0], pts[1], pts[2], pts[3]);
		}

		canvas.endBatch();

		Point offs;

		if (flags.pressed)
		{
			offs.set(1, 1);
		}

		canvas.setFont(state->elements["body"].font);
		Color colorText = state->elements["body"].foreColor.scaled(.8f);

		if (flags.hovered || flags.pressed || selected)
		{
			colorText = Color::white;
		}

		canvas.setColor(colorText);
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
		theme->customEngine->drawWidget(WidgetType::Radio, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Radio::handleEvent(WidgetEvent& e)
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
			if (group)
			{
				for (auto w : group->getWidgets())
				{
					w->selected = false;
				}
			}

			selected = true;

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
				if (group)
				{
					for (auto w : group->getWidgets())
					{
						w->selected = false;
					}
				}

				selected = true;

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