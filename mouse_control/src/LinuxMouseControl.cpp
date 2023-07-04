#include <iostream>

#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define PRIMARY_BUTTON 1
#define MIDDLE_BUTTON 2
#define SECONDARY_BUTTON 3

bool MoveMouse(int x, int y)
{
	Display *displayMain = XOpenDisplay(NULL);

	if (displayMain == NULL)
	{
		std::cout << "Could not open main display!" << std::endl;
		return false;
	}

	XWarpPointer(displayMain, None, None, 0, 0, 0, 0, x, y);

	XCloseDisplay(displayMain);

	return true;
}

bool DoClick(Display *display, int button)
{
	XEvent ev;
	memset(&ev, 0, sizeof(ev));
	ev.xbutton.button = button;
	ev.xbutton.same_screen = true;
	ev.xbutton.subwindow = DefaultRootWindow(display);

	while (ev.xbutton.subwindow)
	{
		ev.xbutton.window = ev.xbutton.subwindow;
		XQueryPointer (display, ev.xbutton.window,
			 &ev.xbutton.root, &ev.xbutton.subwindow,
			 &ev.xbutton.x_root, &ev.xbutton.y_root,
			 &ev.xbutton.x, &ev.xbutton.y,
			 &ev.xbutton.state);
	}

	ev.type = ButtonPress;

	if (0 == XSendEvent(
				display,
				PointerWindow,
				true,
				ButtonPressMask,
				&ev))
	{
			return false;
	}
	XFlush(display);

	ev.type = ButtonRelease;

	if (0 == XSendEvent(
				display,
				PointerWindow,
				true,
				ButtonReleaseMask,
				&ev))
	{
			return false;
	}
	XFlush(display);

	return true;
}

bool PrimaryClick()
{
	Display *displayMain = XOpenDisplay(NULL);
  bool ret = DoClick(displayMain, PRIMARY_BUTTON);
	XCloseDisplay(displayMain);
	return ret;
}

bool SecondaryClick()
{
	Display *displayMain = XOpenDisplay(NULL);
  bool ret = DoClick(displayMain, SECONDARY_BUTTON);
	XCloseDisplay(displayMain);
	return ret;
}

