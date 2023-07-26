#include <chrono>
#include <iostream>
#include <thread>

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
	Scrolling = scrolling;
}

int main(int argc, char** argv)
{
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
	float mouseSpeed = 1.8f;
	float scrollingSpeed = 3.;
	bool vertical = true;

	for (int i = 0; i < argc; i++)
	{
			if (strcmp(argv[i], "speed") == 0)
			{
					if (i < (argc - 1))
					{
						try {
								mouseSpeed = static_cast<float>(std::atof(argv[i + 1]));
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
								scrollingSpeed = static_cast<float>(std::atof(argv[i + 1]));
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
                vertical = true;
			}
			else if (strcmp(argv[i], "horizontal") == 0)
			{
                vertical = false;
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

	ulp.SetPositionCallback([mouseSpeed, scrollingSpeed, vertical](LEAP_VECTOR v){ 
		if ((PrevPos.x == 0 && PrevPos.y == 0 && PrevPos.z == 0) || !MouseActive)
		{
		    // We want to do relative updates so skip this one so we have sensible numbers
		}
        else
        {

			int xMove = static_cast<int>(mouseSpeed * (v.x - PrevPos.x));
			float yMove = (v.y - PrevPos.y) * (vertical ? -1 : 1);

		    if (Scrolling)
			{
		            VerticalScroll(static_cast<int>(scrollingSpeed * yMove));	     
			}
			else
			{
					MoveMouse(xMove, static_cast<int>(mouseSpeed * yMove));
			}
		}
		PrevPos = v;    
	});
	
	ulp.StartPoller();
	
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
