#pragma once

struct keyPressedInfo
{
    int key = 64;
	int velocity = 64;
	bool keyPressed = false;

	signal timeSincePressed;
	signal timeSinceReleased;
};