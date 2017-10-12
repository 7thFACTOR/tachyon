// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "input/window.h"
#include "base/array.h"

namespace engine
{
class E_API Application
{
public:
	Application();
	virtual ~Application();

	void run();
	virtual void update();

	bool stopped = false;
	InputQueue inputQueue;

protected:
	void processMessages();
};

}