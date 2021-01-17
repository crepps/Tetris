#include "Input.h"

Input::Input()
{
	for (int i = 0; i < 256; ++i)
		keyboard[i] = keyDown[i] = false;
}

void Input::Update()
{
	for (int i = 0; i < 256; ++i)
	{
		if (keyDown[i] && !keyboard[i])
			keyDown[i] = false;
	}

	//Sleep(1);
}