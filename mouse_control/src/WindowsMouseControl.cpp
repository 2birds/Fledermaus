#include <Windows.h>
#include "MouseControl.h"

bool MoveMouse(int x, int y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dx = x;
	input.mi.dy = y;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
	return true;
}

bool IssueClick(DWORD flags)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.time = 0;
	input.mi.mouseData = 0;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.dwFlags = flags;
	input.mi.mouseData = 0;
	input.mi.dwExtraInfo = NULL;

	SendInput(1, &input, sizeof(input));
	return true;
}

bool PrimaryDown()
{
	return IssueClick(MOUSEEVENTF_LEFTDOWN);
}

bool PrimaryUp()
{
	return IssueClick(MOUSEEVENTF_LEFTDOWN);
}

bool PrimaryClick()
{
	return IssueClick(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP);
}

bool SecondaryDown()
{
	return IssueClick(MOUSEEVENTF_RIGHTDOWN);
}

bool SecondaryUp()
{
	return IssueClick(MOUSEEVENTF_RIGHTUP);
}

bool SecondaryClick()
{
	return IssueClick(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP);
}

bool MiddleClick()
{
        return IssueClick(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP);
}

bool MiddleDown()
{
        return IssueClick(MOUSEEVENTF_MIDDLEDOWN);
}

bool MiddleUp()
{
        return IssueClick(MOUSEEVENTF_MIDDLEUP);
}

bool VerticalScroll(int wheelAmt)
{
        DWORD amt = static_cast<DWORD>(wheelAmt);
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.time = 0;
	input.mi.mouseData = 0;
	input.mi.dx = 0;
	input.mi.dy = 0;
	input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	input.mi.mouseData = amt;
	input.mi.dwExtraInfo = NULL;

	SendInput(1, &input, sizeof(input));
	return true;
}
