#include "core/application.h"
#include "graphics/graphics.h"
#include "core/globals.h"
#include "input/window.h"
#include "input/types.h"
#include "base/platform.h"
#include "base/logger.h"

namespace engine
{
Application::Application()
	: stopped(false)
{}

Application::~Application()
{}

void Application::run()
{
	while (!stopped)
	{
		processMessages();

		if (stopped)
			break;

		update();
		yield();
	}
}

void Application::processMessages()
{
	InputEvent ev;

	if (getGraphics().getWindow()->pollEvent(ev))
	{
		inputQueue.addEvent(ev);
	}
}

void Application::update()
{}

}