#include <chrono>
#include <iostream>
#include <thread>

#include "MouseControl.h"
#include "UltraleapPoller.h"

bool MouseActive = true;
LEAP_VECTOR PrevPos = {0, 0, 0};

void SetMouseActive(bool active)
{
	MouseActive = active;
}

int main()
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
	bool vertical = false;

	printf("Setting up..");
	UltraleapPoller ulp;
	ulp.SetOnPinkyPinchStartCallback([](eLeapHandType) { 
			printf("Fist start\n");
		SetMouseActive(false); });
	ulp.SetOnPinkyPinchStopCallback([](eLeapHandType) {
		printf("Fist stopped\n");
		SetMouseActive(true); });
	ulp.SetOnIndexPinchStartCallback([](eLeapHandType) { 
		printf("Pinch started\n");
		PrimaryDown(); });
	ulp.SetOnIndexPinchStopCallback([](eLeapHandType) {
		printf("Pinch stopped\n");
		PrimaryUp(); });
	ulp.SetOnMiddlePinchStartCallback([](eLeapHandType) {
			printf("V started\n");
		SecondaryClick();
    });

	ulp.SetPositionCallback([mouseSpeed, vertical](LEAP_VECTOR v){ 
		if ((PrevPos.x == 0 && PrevPos.y == 0 && PrevPos.z == 0) || !MouseActive)
		{
		    // We want to do relative updates so skip this one so we have sensible numbers
		}
        else
        {
            int xMove, yMove;
            xMove = static_cast<int>(mouseSpeed * (v.x - PrevPos.x));
            if (vertical)
            {
                yMove = -static_cast<int>(mouseSpeed * (v.y - PrevPos.y));
            }
            else
            {
                yMove = static_cast<int>(mouseSpeed * (v.z - PrevPos.z));
            }
                MoveMouse(xMove, yMove);
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
