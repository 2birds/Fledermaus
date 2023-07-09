#include <Windows.h>
#include "MouseControl.h"

bool MoveMouse(int x, int y)
{
	POINT cursorPos;
	if (!GetCursorPos(&cursorPos))
	{
		return false;
	}

	if (!SetCursorPos(cursorPos.x + x, cursorPos.y + y))
	{
		return false;
	}

	return true;
}

bool PrimaryClick()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return true;
}

bool SecondaryClick()
{
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	return true;
}
