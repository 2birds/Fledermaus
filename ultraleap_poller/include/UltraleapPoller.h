#include "LeapC.h"

#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>

typedef std::function<void(LEAP_VECTOR)> position_callback_t;
typedef std::function<void(eLeapHandType)> gesture_callback_t;
class UltraleapPoller
{
	public:
		UltraleapPoller();
		~UltraleapPoller();
		
		void StartPoller();
		void StopPoller();

		// Fires on each update with a hand
		void SetPositionCallback(position_callback_t callback);

		void SetOnPinchStartCallback(gesture_callback_t callback);
		void SetOnPinchContinueCallback(gesture_callback_t callback);
		void SetOnPinchStopCallback(gesture_callback_t callback);

		void SetOnFistStartCallback(gesture_callback_t callback);
		void SetOnFistContinueCallback(gesture_callback_t callback);
		void SetOnFistStopCallback(gesture_callback_t callback);

		void SetOnVStartCallback(gesture_callback_t callback);
		void SetOnVContinueCallback(gesture_callback_t callback);
		void SetOnVStopCallback(gesture_callback_t callback);

		void ClearPositionCallback();

		void ClearOnPinchStartCallback();
		void ClearOnPinchContinueCallback();
		void ClearOnPinchStopCallback();

		void ClearOnFistStartCallback();
		void ClearOnFistContinueCallback();
		void ClearOnFistStopCallback();

		void ClearOnVStartCallback();
		void ClearOnVContinueCallback();
		void ClearOnVStopCallback();

	private:
        void runPoller();
		float distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const;

		bool isPinching(const LEAP_HAND* hand) const;
		bool isMiddlePinching(const LEAP_HAND *hand) const;
		bool isRingPinching(const LEAP_HAND *hand) const;
		bool isFist(const LEAP_HAND *hand) const;
		bool isV(const LEAP_HAND* hand) const;

		void handleDeviceMessage(const LEAP_DEVICE_EVENT *device_event);
		void handleTrackingMessage(const LEAP_TRACKING_EVENT *tracking_event);
		
		void pinchChecks(const LEAP_HAND* hand);
		void fistChecks(const LEAP_HAND* hand);
		void VChecks(const LEAP_HAND* hand);

	private:
		bool pollerRunning_ = false;
		const float pinchThreshold_ =  0.85f;
		const float middlePinchThreshold_ =  15.f;
		const float ringPinchThreshold_ =  15.f;
		const float fistThreshold_ =  0.9f;

		position_callback_t positionCallback_;

		gesture_callback_t pinchStartCallback_;
		gesture_callback_t pinchContinueCallback_;
		gesture_callback_t pinchStopCallback_;

		gesture_callback_t fistStartCallback_;
		gesture_callback_t fistContinueCallback_;
		gesture_callback_t fistStopCallback_;

		gesture_callback_t VStartCallback_;
		gesture_callback_t VContinueCallback_;
		gesture_callback_t VStopCallback_;

		LEAP_CONNECTION lc_;
		std::thread pollingThread_;
		
		uint32_t activeHandID = 0;

		bool doingPinch_ = false;
		bool doingFist_ = false;
		bool doingV_ = false;
};
