#include "LeapC.h"

#include <algorithm>
#include <mutex>
#include <thread>

class UltraleapPoller
{
	public:
		UltraleapPoller();
		~UltraleapPoller();

		// Fires on each update with a hand
		void SetPositionCallback(LEAP_HAND* hands, int nhands);

		void SetOnPinchStartCallback(eLeapHandType chirality);
		void SetOnPinchContinueCallback(eLeapHandType chirality);
		void SetOnPinchEnd(eLeapHandType chirality);

		void SetOnFistStartCallback(eLeapHandType chirality);
		void SetOnFistContinueCallback(eLeapHandType chirality);
		void SetOnFistEnd(eLeapHandType chirality);

		void SetOnVStartCallback(eLeapHandType chirality);
		void SetOnVContinueCallback(eLeapHandType chirality);
		void SetOnVEnd(eLeapHandType chirality);

	private:
		const float pinchThreshold_ =  0.7f;
		const float fistThreshold_ =  0.7f;

		float distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const;

		bool isPinching(const LEAP_HAND* hand) const;
		bool isFist(const LEAP_HAND* hand) const;
		bool isV(const LEAP_HAND* hand) const;

	private:
		LEAP_CONNECTION lc_;
		std::thread pollingThread_;

		bool leftPinch_ = false;
		bool rightPinch_ = false;

		bool leftFist_ = false;
		bool rightFist_ = false;

		bool leftV_ = false;
		bool rightV_ = false;
};
