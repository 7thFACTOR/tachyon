#include "gui/slider.h>
#include "gui/canvas.h>
#include "gui/theme.h>
#include "gui/event.h>
#include "gui/font.h>

namespace horus
{
void Slider::draw(Canvas& canvas)
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
		canvas.setBlendMode(BlendMode::Opaque);
		canvas.beginBatch(theme->atlas);
		f32 barWeight = state->elements["handle"].getParam("barWeight", 6.0f);
		f32 handleSize = state->elements["handle"].getParam("handleSize", 6.0f);
		f32 selTime = 1.0f - (maxValue - currentValue) / (maxValue - minValue);
		Rect barRect = Rect(absRect.x + handleSize / 2.0f, absRect.center().y - barWeight / 2.0f, absRect.width - handleSize, barWeight);
		Rect selBarRect = Rect(barRect.x, absRect.center().y - barWeight / 2.0f, barRect.width * selTime, barWeight);
		
		canvas.setColor(state->elements["body"].backColor.scaled(0.3f));
		canvas.draw(state->elements["body"].item, barRect);

		canvas.setColor(Color::white);
		canvas.draw(state->elements["body"].item, selBarRect);
		
		handleRect = Rect(
			selBarRect.right() - handleSize / 2.0f,
			absRect.center().y - handleSize * 2 / 2.0f,
			handleSize, handleSize * 2);

		canvas.setColor(state->elements["body"].backColor.scaled(0.2f));
		canvas.draw(state->elements["body"].item, handleRect);
		canvas.endBatch();

		canvas.beginBatch(&state->elements["body"].font->getAtlas());
		char val[25] = { 0 };
		sprintf(val, "%.2f", currentValue);
		Point lblPos(absRect.right() + 5, absRect.center().y);
		canvas.setColor(Color::white);
		canvas.setFont(state->elements["body"].font);
		canvas.draw(val, lblPos, HorizontalTextAlignment::Right, VerticalTextAlignment::Center);
		canvas.endBatch();
	}
	else if (theme->engine == ThemeEngine::Custom)
	{
		theme->customEngine->drawWidget(WidgetType::Slider, this, canvas);
	}
	else if (theme->engine == ThemeEngine::Skinned)
	{
		//TODO
	}
}

void Slider::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::KeyDown:
		if (flags.focused)
		{
			if (e.key == KeyCode::ArrowLeft || e.key == KeyCode::ArrowDown)
			{
				currentValue -= keyboardStep;
			}
			else if (e.key == KeyCode::ArrowRight || e.key == KeyCode::ArrowUp)
			{
				currentValue += keyboardStep;
			}
			else if (e.key == KeyCode::Home || e.key == KeyCode::PgDown)
			{
				currentValue = 0.0f;
			}
			else if (e.key == KeyCode::End || e.key == KeyCode::PgUp)
			{
				currentValue = 1.0f;
			}

			if (actionCallback)
			{
				actionCallback(this, e, WidgetActionType::Changed);
			}

			clampValue();
			flags.pressed = 1;
			flags.captured = 1;
		}
		break;
	case WidgetEventType::KeyUp:
		if (flags.focused)
		{
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

			movingHandle = true;
			computeValue(e.windowPoint);
		}
		break;
	case WidgetEventType::MouseButtonUp:
		movingHandle = false;

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
	case WidgetEventType::MouseMove:
		if (movingHandle)
		{
			computeValue(e.windowPoint);

			if (actionCallback)
			{
				actionCallback(this, e, WidgetActionType::Changed);
			}
		}
		break;
	default:
		break;
	}
}

void Slider::computeValue(const Point& mousePos)
{
	currentValue = (mousePos.x - (absRect.x + handleRect.width / 2.0f)) / (absRect.width - handleRect.width);
	clampValue();
	currentValue = minValue + currentValue * (maxValue - minValue);
}

void Slider::clampValue()
{
	if (currentValue < 0.0f)
	{
		currentValue = 0.0f;
	}

	if (currentValue > 1.0f)
	{
		currentValue = 1.0f;
	}
}

}