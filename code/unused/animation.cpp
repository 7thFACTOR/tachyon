#include "gui/animation.h"

namespace engine
{
void GuiStateTransition::update(f32 deltaTime)
{
	if (stopped)
	{
		return;
	}

	if (!easeFunc)
	{
		return;
	}

	if (time >= 1.0f)
	{
		stopped = true;
		currentValue = keys[reverse ? 0 : 1];
		return;
	}

	time += deltaTime * speed;

	f32 t = easeFunc(time, 0.0f, 1.0f, 1.0f);
	int index0 = 0, index1 = 1;

	if (reverse)
	{
		index0 = 1;
		index1 = 0;
	}

	GuiStateTransitionKey& key0 = keys[index0];
	GuiStateTransitionKey& key1 = keys[index1];

	if (animateColor)
	{
		currentValue.color = key0.color + (key1.color - key0.color) * t;
	}

	if (animateRect)
	{
		currentValue.rect.x = key0.rect.x + (key1.rect.x - key0.rect.x) * t;
		currentValue.rect.y = key0.rect.y + (key1.rect.y - key0.rect.y) * t;
		currentValue.rect.width = key0.rect.width + (key1.rect.width - key0.rect.width) * t;
		currentValue.rect.height = key0.rect.height + (key1.rect.height - key0.rect.height) * t;
	}
}

void GuiStateTransition::start(bool reversed)
{
	time = 0.0f;
	reverse = reversed;
	stopped = false;
}

}