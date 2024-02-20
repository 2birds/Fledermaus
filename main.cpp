#include <chrono>
#include <iostream>
#include <thread>

#include "ConfigReader.h"
#include "MouseControl.h"
#include "UltraleapPoller.h"

bool MouseActive = true;
bool Scrolling = false;

LEAP_VECTOR PrevPos = {0, 0, 0};

void SetMouseActive(bool active)
{
	MouseActive = active;
}

void SetScrolling(bool scrolling)
{
	std::cout << "Setting scrolling to " << (Scrolling ? "true" : "false") << std::endl;
	Scrolling = scrolling;
}

bool GetScrolling()
{
	return Scrolling;
}

int main(int argc, char** argv)
{
	ConfigReader config;
	/*
	std::cout << "Moving mouse" << std::endl;

	for (int i = 0; i < 10; i++)
	{
		if (i % 2 == 1)
		{
			MoveMouse(100, 100);
			SecondaryClick();
		}
		else
		{
			MoveMouse(-100, -100);
			PrimaryClick();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	PrimaryClick();

	for (int i = 0; i < 10; i++)
	{
		if (i % 2 == 1)
		{
			VerticalScroll(120);
		}
		else
		{
			VerticalScroll(-120);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

  return 0;
  */


	// float mouseSpeed = 1.8f;
	// float scrollingSpeed = 3.;
	// bool vertical = true;

	for (int i = 0; i < argc; i++)
	{
			if (strcmp(argv[i], "speed") == 0)
			{
					if (i < (argc - 1))
					{
						try {
								config.SetSpeed(static_cast<float>(std::atof(argv[i + 1])));
						}
						catch (std::exception &e)
						{
								std::cout << "Could not set mouse speed, number must be a float" << std::endl;
								std::cout << "Error: " << e.what() << std::endl;
								return -1;
						}
					}
					else
					{
								std::cout << "Not enough arguments" << std::endl;
								return -1;
					}
			}
			else if (strcmp(argv[i], "scrolling") == 0)
			{
					if (i < (argc - 1))
					{
						try {
								config.SetScrollingSpeed(static_cast<float>(std::atof(argv[i + 1])));
						}
						catch (std::exception &e)
						{
								std::cout << "Could not set scrolling speed, number must be a float" << std::endl;
								std::cout << "Error: " << e.what() << std::endl;
								return -1;
						}
					}
					else
					{
								std::cout << "Not enough arguments" << std::endl;
								return -1;
					}
			}
			else if (strcmp(argv[i], "vertical") == 0)
			{
                config.SetVerticalOrientation(true);
			}
			else if (strcmp(argv[i], "horizontal") == 0)
			{
                config.SetVerticalOrientation(false);
			}
	}

	printf("Setting up..");
	UltraleapPoller ulp;
	ulp.SetOnFistStartCallback([](eLeapHandType) { 
			// printf("Fist start\n");
		SetMouseActive(false); });
	ulp.SetOnFistStopCallback([](eLeapHandType) {
		// printf("Fist stopped\n");
		SetMouseActive(true); });
	ulp.SetOnIndexPinchStartCallback([](eLeapHandType) { 
		// printf("Pinch started\n");
		PrimaryDown(); });
	ulp.SetOnIndexPinchStopCallback([](eLeapHandType) {
		// printf("Pinch stopped\n");
		PrimaryUp(); });
	ulp.SetOnRotateStartCallback([](eLeapHandType) {
			// printf("Rotate started\n");
		SecondaryDown();
    });
	ulp.SetOnRotateStopCallback([](eLeapHandType) {
			// printf("Rotate stopped\n");
		SecondaryUp();
    });
	ulp.SetOnVStartCallback([](eLeapHandType) {
		SetScrolling(true);
    });
	ulp.SetOnVStopCallback([](eLeapHandType) {
		SetScrolling(false);
    });

	ulp.SetPositionCallback([&config](LEAP_VECTOR v){ 
		if ((PrevPos.x == 0 && PrevPos.y == 0 && PrevPos.z == 0) || !MouseActive)
		{
		    // We want to do relative updates so skip this one so we have sensible numbers
		}
        else
        {

			int xMove = static_cast<int>(config.GetSpeed() * (v.x - PrevPos.x));
			float yMove = (v.y - PrevPos.y) * (config.IsVerticalOrientation() ? -1 : 1);

		    if (config.GetUseScrolling() && Scrolling)
			{
		            VerticalScroll(static_cast<int>(config.GetScrollingSpeed() * yMove));	     
			}
			else
			{
					MoveMouse(xMove, static_cast<int>(config.GetSpeed()* yMove));
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
