#include <chrono>
#include <iostream>
#include <thread>

#include "ConfigReader.h"
#include "MouseControl.h"
#include "UltraleapPoller.h"

#define SECONDS_TO_MICROSECONDS(seconds) seconds * 1000000
#define METERS_TO_MILLIMETERS(meters) meters * 1000
#define MILLIMETERS_TO_METERS(millimeteres) millimeteres * 0.001

bool MouseActive = true;
bool Scrolling = false;

int64_t PrevFistTimestamp;
int64_t FistStartTimestamp;
LEAP_VECTOR FistStartPosition;
bool CancelFistRecentering = false;
const float FIST_RECENTER_HOLD_TIME_SECONDS = 1.0f;
const float FIST_RECENTER_DEADZONE_DISTANCE_METERS = 0.03f;

bool CursorDeadzoneEnabled;
LEAP_VECTOR CursorDeadzoneStartPosition;
const float CURSOR_DEADZONE_THRESHOLD_METERS = 0.03f;

LEAP_VECTOR PrevPos = {0, 0, 0};


float lerp(float a, float b, float t)
{
	return (1.0f - t) * a + b * t;
}

float inverse_lerp(float a, float b, float v)
{
	return (v - a) / (b - a);
}

float remap(float iMin, float iMax, float oMin, float oMax, float v)
{
	float t = inverse_lerp(iMin, iMax, v);
	return lerp(oMin, oMax, t);
}

void SetMouseActive(bool active)
{
	MouseActive = active;
}

void SetScrolling(bool scrolling)
{
	Scrolling = scrolling;
}

bool GetScrolling()
{
	return Scrolling;
}

bool ParseCommandLine(ConfigReader &config, int argc, char **argv)
{
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--speed") == 0)
		{
			if (i < (argc - 1))
			{
				try
				{
					config.SetSpeed(static_cast<float>(std::atof(argv[i + 1])));
				}
				catch (std::exception &e)
				{
					std::cout << "Could not set mouse speed, number must be a float" << std::endl;
					std::cout << "Error: " << e.what() << std::endl;
					return false;
				}
			}
			else
			{
				std::cout << "Not enough arguments" << std::endl;
				return false;
			}
		}
		else if (strcmp(argv[i], "--scrolling-speed") == 0)
		{
			if (i < (argc - 1))
			{
				try
				{
					config.SetScrollingSpeed(static_cast<float>(std::atof(argv[i + 1])));
				}
				catch (std::exception &e)
				{
					std::cout << "Could not set scrolling speed, number must be a float" << std::endl;
					std::cout << "Error: " << e.what() << std::endl;
					return false;
				}
			}
			else
			{
				std::cout << "Not enough arguments" << std::endl;
				return false;
			}
		}
		else if (strcmp(argv[i], "--orientation") == 0)
		{
			if (i < (argc - 1))
			{
				try
				{
					if (strcmp(argv[i+1], "vertical") == 0)
					{
						config.SetVerticalOrientation(true);
					}
					else if (strcmp(argv[i+1], "horizontal") == 0)
					{
						config.SetVerticalOrientation(false);
					}
					else
					{
						std::cout << "Orientation must be set to either \"vertical\" or \"horizontal\"" << std::endl;
						return false;
					}
				}
				catch (std::exception &e)
				{
					std::cout << "Could not set scrolling speed, number must be a float" << std::endl;
					std::cout << "Error: " << e.what() << std::endl;
					return false;
				}
			}
			else
			{
				std::cout << "Not enough arguments" << std::endl;
				return false;
			}
		}
		else if (strcmp(argv[i], "--allow-scrolling") == 0)
		{
			if (i < (argc - 1))
			{
				if (strcmp(argv[i + 1], "true") == 0)
				{
					config.SetScrollingActive(true);
				}
				else if (strcmp(argv[i + 1], "false") == 0)
				{
					config.SetScrollingActive(false);
				}
				else
				{
					std::cout << "Set --allow-scrolling to either \"true\" or \"false\"" << std::endl;
					return false;
				}
			}
		}
		else if (strcmp(argv[i], "--use-fist-to-lift-mouse") == 0)
		{
			if (i < (argc - 1))
			{
				if (strcmp(argv[i + 1], "true") == 0)
				{
					config.SetFistToLiftActive(true);
				}
				else if (strcmp(argv[i + 1], "false") == 0)
				{
					config.SetFistToLiftActive(false);
				}
				else
				{
					std::cout << "Set --use-fist-to-lift-mouse to either \"true\" or \"false\"" << std::endl;
					return false;
				}
			}
		}
		else if (strcmp(argv[i], "--lock-mouse-on-scroll") == 0)
		{
			if (i < (argc - 1))
			{
				if (strcmp(argv[i + 1], "true") == 0)
				{
					config.SetLockMouseOnScroll(true);
				}
				else if (strcmp(argv[i + 1], "false") == 0)
				{
					config.SetLockMouseOnScroll(false);
				}
				else
				{
					std::cout << "Set --lock-mouse-on-scroll to either \"true\" or \"false\"" << std::endl;
					return false;
				}
			}
		}
		else if (strcmp(argv[i], "--right-click-active") == 0)
		{
			if (i < (argc - 1))
			{
				if (strcmp(argv[i + 1], "true") == 0)
				{
					config.SetRightClickActive(true);
				}
				else if (strcmp(argv[i + 1], "false") == 0)
				{
					config.SetRightClickActive(false);
				}
				else
				{
					std::cout << "Set --right-click-active to either \"true\" or \"false\"" << std::endl;
					return false;
				}
			}
		}
	}

	return true;
}

int main(int argc, char** argv)
{
	ConfigReader config;
	if (!ParseCommandLine(config, argc, argv))
	{
		std::cout << "Failed to parse command line properly. Some values may be defaults." << std::endl;
	}

    config.print();

	printf("Setting up..\n");
	UltraleapPoller ulp;

	ulp.indexPinchThreshold = config.GetIndexPinchThreshold();
	ulp.boundsLeftM = config.GetBoundsLeftMeters();
	ulp.boundsRightM = config.GetBoundsRightMeters();
	ulp.boundsLowerM = config.GetBoundsLowerMeters();
	ulp.boundsUpperM = config.GetBoundsUpperMeters();
	ulp.boundsNearM = config.GetBoundsNearMeters();
	ulp.boundsFarM = config.GetBoundsFarMeters();
	ulp.limitTrackingToWithinBounds = config.GetLimitTrackingToWithinBounds();

	if (config.GetFistToLiftActive())
	{
		ulp.SetOnFistStartCallback(
			[](const int64_t timestamp, const LEAP_HAND &hand)
			{
				SetMouseActive(false);

				FistStartTimestamp = timestamp;
				FistStartPosition = hand.palm.position;
				CancelFistRecentering = false;

				// Double fist to recenter.
				/*int64_t timeBetweenFists = timestamp - PrevFistTimestamp;
				if (timeBetweenFists < SECONDS_TO_MICROSECONDS(1))
				{
					// Recenter the mouse
					int w = GetScreenWidth();
					int h = GetScreenHeight();
					SetMouse(w / 2, h / 2);
				}

				PrevFistTimestamp = timestamp;*/
			}
		);
		ulp.SetOnFistContinueCallback(
			[&ulp](const int64_t timestamp, const LEAP_HAND& hand)
			{
				if (CancelFistRecentering)
				{
					return;
				}

				float distance = ulp.distance(FistStartPosition, hand.palm.position);
				if (distance > METERS_TO_MILLIMETERS(FIST_RECENTER_DEADZONE_DISTANCE_METERS))
				{
					CancelFistRecentering = true;
				}

				int64_t timeSinceFistStart = timestamp - FistStartTimestamp;
				if (timeSinceFistStart > SECONDS_TO_MICROSECONDS(FIST_RECENTER_HOLD_TIME_SECONDS))
				{
					CancelFistRecentering = true;

					int w = GetScreenWidth();
					int h = GetScreenHeight();
					SetMouse(w / 2, h / 2);
				}
			}
		);
		ulp.SetOnFistStopCallback([](const int64_t timestamp, const LEAP_HAND &)
								  { SetMouseActive(true); });
	}
	
	// The following interferes with fist.
	// The AlmostPinchStop callback fires 
	// ulp.SetOnAlmostPinchStartCallback([](const LEAP_HAND&) { 
	// 	SetMouseActive(false);
	// 	});
	// ulp.SetOnAlmostPinchStopCallback([](const LEAP_HAND&) {
	// 	SetMouseActive(true);
	// 	});
	ulp.SetOnIndexPinchStartCallback([](const int64_t timestamp, const LEAP_HAND &hand) {
		PrimaryDown();
		CursorDeadzoneStartPosition = hand.palm.position;
		CursorDeadzoneEnabled = true;
	});
	ulp.SetOnIndexPinchStopCallback([](const int64_t timestamp, const LEAP_HAND&) {
		PrimaryUp();
		CursorDeadzoneEnabled = false;
	});
	ulp.SetOnAlmostRotateStartCallback([](const int64_t timestamp, const LEAP_HAND&) {
        SetMouseActive(false);
    });
	ulp.SetOnAlmostRotateStopCallback([](const int64_t timestamp, const LEAP_HAND&) {
        SetMouseActive(true);
    });

	if (config.GetRightClickActive())
	{
		ulp.SetOnRotateStartCallback([&config](const int64_t timestamp, const LEAP_HAND &)
									 {
			// printf("Rotate started\n");
		SecondaryDown(); });
		ulp.SetOnRotateStopCallback([](const int64_t timestamp, const LEAP_HAND &)
									{
			// printf("Rotate stopped\n");
		SecondaryUp(); });
	}

	if (config.GetScrollingActive())
	{
		ulp.SetOnVStartCallback([](const int64_t timestamp, const LEAP_HAND &) {
			SetScrolling(true);
		});
		ulp.SetOnVContinueCallback(
			[&config](const int64_t timestamp, const LEAP_HAND &h)
			{
				float palmToFingertipDist = h.middle.distal.next_joint.y - h.palm.position.y;

				float move = config.GetScrollingSpeed();
				float threshold = config.GetScrollThreshold();

				if (palmToFingertipDist > threshold)
				{
					VerticalScroll(static_cast<int>(move));
				}
				else if (palmToFingertipDist < -threshold)
				{
					VerticalScroll(static_cast<int>(-move));
				}
			}
		);
		ulp.SetOnVStopCallback([](const int64_t timestamp, const LEAP_HAND &) {
			SetScrolling(false);
		});
	}

	ulp.SetPositionCallback([&ulp, &config](LEAP_VECTOR v) {
		if ((PrevPos.x == 0 && PrevPos.y == 0 && PrevPos.z == 0) || !MouseActive)
		{
			// We want to do relative updates so skip this one so we have sensible numbers
		}
		else
		{
			if (CursorDeadzoneEnabled)
			{
				float deadzoneDistance = ulp.distance(CursorDeadzoneStartPosition, v);
				if (deadzoneDistance > METERS_TO_MILLIMETERS(CURSOR_DEADZONE_THRESHOLD_METERS))
				{
					CursorDeadzoneEnabled = false;
				}
				return;
			}

			int xMove = static_cast<int>(config.GetSpeed() * (v.x - PrevPos.x));
			float yMove = (v.y - PrevPos.y) * (config.GetVerticalOrientation() ? -1 : 1);

			// if (config.GetUseScrolling() && Scrolling)
			if (Scrolling && config.GetLockMouseOnScroll())
			{
				// VerticalScroll(static_cast<int>(config.GetScrollingSpeed() * yMove));
			}
			else
			{
				if (config.GetUseAbsoluteMousePosition())
				{
					int w = GetScreenWidth();
					int h = GetScreenHeight();

					float boundsLower = config.GetBoundsLowerMeters();
					float boundsUpper = config.GetBoundsUpperMeters();
					float boundsLeft = config.GetBoundsLeftMeters();
					float boundsRight = config.GetBoundsRightMeters();

					int mouseX = remap(-boundsLeft, boundsRight, 0, w, MILLIMETERS_TO_METERS(v.x));
					int mouseY = remap(boundsLower, boundsUpper, h, 0, MILLIMETERS_TO_METERS(v.y));

					//printf("W: %f pos vs %i/%i pixels\n", MILLIMETERS_TO_METERS(v.x), mouseX, w);
					//printf("H: %f pos vs %i/%i pixels\n\n", MILLIMETERS_TO_METERS(v.y), mouseY, h);

					SetMouse(mouseX, mouseY);
				}
				else
				{
					MoveMouse(xMove, static_cast<int>(config.GetSpeed() * yMove));
				}
			}
		}

		PrevPos = v;
	});
	
	ulp.StartPoller();
	
	std::cout << "Press \"x\" to quit." << std::endl;
	
	while (true)
	{
		char c;
		std::cin >> c;
		if (c == 'x')
		{
			break;
		}
	}
	printf("Quitting\n");

	ulp.StopPoller();
	return 0;
}
