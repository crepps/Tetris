#pragma once

#include "Include.h"

struct Input
{
	bool keyboard[256],
		keyDown[256];

	int key;

	Input();

	void Update();
};