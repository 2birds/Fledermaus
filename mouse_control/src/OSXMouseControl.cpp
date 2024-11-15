#include <ApplicationServices/ApplicationServices.h>
#include <iostream>

std::pair<int,int> lastMousePos = std::make_pair<int,int>(0,0);

// Move the mouse to specific coordinates on the screen
bool MoveMouse(int x, int y)
{
    CGEventRef mousePosEvent = CGEventCreate(NULL);
    CGPoint currentMousePos = CGEventGetLocation(mousePosEvent);
    CFRelease(mousePosEvent);

    CGEventRef move = CGEventCreateMouseEvent(
        NULL, kCGEventMouseMoved,
        CGPointMake(currentMousePos.x + x, currentMousePos.y + y),
        kCGMouseButtonLeft // ignored
    );

    lastMousePos.first = currentMousePos.x + x;
    lastMousePos.second = currentMousePos.y + y;

    CGEventPost(kCGHIDEventTap, move);
    CFRelease(move);
    return true;
}
bool SetMouse(int x, int y)
{
    return true;
}

// Untested
int GetScreenWidth()
{
    CGDirectDisplayID id = CGMainDisplayID();
    return static_cast<size_t>(CGDisplayPixelsWide(id));
}

// Untested
int GetScreenHeight()
{
    CGDirectDisplayID id = CGMainDisplayID();
    return static_cast<size_t>(CGDisplayPixelsHigh(id));
}

bool PrimaryDown()
{
    CGEventRef primaryDownEvent = CGEventCreateMouseEvent(
        NULL, kCGEventLeftMouseDown,
        CGPointMake(lastMousePos.first, lastMousePos.second),
        kCGMouseButtonLeft
    );

    CGEventPost(kCGHIDEventTap, primaryDownEvent);
    CFRelease(primaryDownEvent);
    return true;
}
bool PrimaryUp()
{
    CGEventRef primaryUpEvent= CGEventCreateMouseEvent(
        NULL, kCGEventLeftMouseUp,
        CGPointMake(lastMousePos.first, lastMousePos.second),
        kCGMouseButtonLeft
    );

    CGEventPost(kCGHIDEventTap, primaryUpEvent);
    CFRelease(primaryUpEvent);
    return true;
}
// Issue click with the primary button
bool PrimaryClick()
{
    return PrimaryDown() && PrimaryUp();
}

bool SecondaryDown()
{
    CGEventRef secondaryDownEvent = CGEventCreateMouseEvent(
        NULL, kCGEventRightMouseDown,
        CGPointMake(lastMousePos.first, lastMousePos.second),
        kCGMouseButtonRight
    );

    CGEventPost(kCGHIDEventTap, secondaryDownEvent);
    CFRelease(secondaryDownEvent);
    return true;
}

bool SecondaryUp()
{
    CGEventRef secondaryUpEvent = CGEventCreateMouseEvent(
        NULL, kCGEventRightMouseUp,
        CGPointMake(lastMousePos.first, lastMousePos.second),
        kCGMouseButtonRight
    );

    CGEventPost(kCGHIDEventTap, secondaryUpEvent);
    CFRelease(secondaryUpEvent);
    return true;
}
// Issue click with the secondary button
bool SecondaryClick()
{
    return SecondaryDown() && SecondaryUp();
}

/* Not supported by OSX mouse by default, what to do? */
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
/* END */

bool VerticalScroll(int scrollAmt)
{
    // Let's work with one
    uint32_t numberofScrollWheels = 1;

    CGEventRef scrollWheelEvent = CGEventCreateScrollWheelEvent(NULL,
                                  kCGScrollEventUnitPixel,
                                  numberofScrollWheels,
                                  static_cast<uint32_t>(scrollAmt));
    CGEventPost(kCGHIDEventTap, scrollWheelEvent);
    CFRelease(scrollWheelEvent);
    return true;
}