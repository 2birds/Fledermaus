#include <ApplicationServices/ApplicationServices.h>

// Move the mouse to specific coordinates on the screen
bool MoveMouse(int x, int y)
{
    CGEventRef move = CGEventCreateMouseEvent(
        NULL, kCGEventMouseMoved,
        CGPointMake(x, y),
        kCGMouseButtonLeft // ignored
    );

    CGEventPost(kCGHIDEventTap, move);
    return true;
}
bool SetMouse(int x, int y)
{
    return true;
}

int GetScreenWidth()
{
    return true;
}
int GetScreenHeight()
{
    return true;
}

bool PrimaryDown()
{
    return true;
}
bool PrimaryUp()
{
    return true;
}
// Issue click with the primary button
bool PrimaryClick()
{
    return true;
}

bool SecondaryDown()
{
    return true;
}
bool SecondaryUp()
{
    return true;
}
// Issue click with the secondary button
bool SecondaryClick()
{
    return true;
}

bool MiddleDown()
{
    return true;
}
bool MiddleUp()
{
    return true;
}

bool MiddleClick()
{
    return true;
}

bool VerticalScroll(int scrollAmt)
{
    return true;
}