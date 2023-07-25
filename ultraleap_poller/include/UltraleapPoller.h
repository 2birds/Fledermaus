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

		void SetOnIndexPinchStartCallback(gesture_callback_t callback);
		void SetOnIndexPinchContinueCallback(gesture_callback_t callback);
		void SetOnIndexPinchStopCallback(gesture_callback_t callback);

		void SetOnMiddlePinchStartCallback(gesture_callback_t callback);
		void SetOnMiddlePinchContinueCallback(gesture_callback_t callback);
		void SetOnMiddlePinchStopCallback(gesture_callback_t callback);

		void SetOnRingPinchStartCallback(gesture_callback_t callback);
		void SetOnRingPinchContinueCallback(gesture_callback_t callback);
		void SetOnRingPinchStopCallback(gesture_callback_t callback);

		void SetOnPinkyPinchStartCallback(gesture_callback_t callback);
		void SetOnPinkyPinchContinueCallback(gesture_callback_t callback);
		void SetOnPinkyPinchStopCallback(gesture_callback_t callback);

		void SetOnFistStartCallback(gesture_callback_t callback);
		void SetOnFistContinueCallback(gesture_callback_t callback);
		void SetOnFistStopCallback(gesture_callback_t callback);

		void SetOnVStartCallback(gesture_callback_t callback);
		void SetOnVContinueCallback(gesture_callback_t callback);
		void SetOnVStopCallback(gesture_callback_t callback);

		void SetOnRotateStartCallback(gesture_callback_t callback);
		void SetOnRotateContinueCallback(gesture_callback_t callback);
		void SetOnRotateStopCallback(gesture_callback_t callback);

		void ClearPositionCallback();

		void ClearOnPinchStartCallback();
		void ClearOnPinchContinueCallback();
		void ClearOnPinchStopCallback();

		void ClearOnIndexPinchStartCallback();
		void ClearOnIndexPinchContinueCallback();
		void ClearOnIndexPinchStopCallback();

		void ClearOnMiddlePinchStartCallback();
		void ClearOnMiddlePinchContinueCallback();
		void ClearOnMiddlePinchStopCallback();

		void ClearOnRingPinchStartCallback();
		void ClearOnRingPinchContinueCallback();
		void ClearOnRingPinchStopCallback();

		void ClearOnPinkyPinchStartCallback();
		void ClearOnPinkyPinchContinueCallback();
		void ClearOnPinkyPinchStopCallback();

		void ClearOnFistStartCallback();
		void ClearOnFistContinueCallback();
		void ClearOnFistStopCallback();

		void ClearOnVStartCallback();
		void ClearOnVContinueCallback();
		void ClearOnVStopCallback();

		void ClearOnRotateStartCallback();
		void ClearOnRotateContinueCallback();
		void ClearOnRotateStopCallback();

	private:
        void runPoller();
		float distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const;

		bool isPinching(const LEAP_HAND* hand) const;
		bool isIndexPinching(const LEAP_HAND* hand) const;
		bool isMiddlePinching(const LEAP_HAND *hand) const;
		bool isRingPinching(const LEAP_HAND *hand) const;
		bool isPinkyPinching(const LEAP_HAND *hand) const;
		bool isFist(const LEAP_HAND *hand) const;
		bool isV(const LEAP_HAND* hand) const;
		bool isRotated(const LEAP_HAND* hand) const;

		void handleDeviceMessage(const LEAP_DEVICE_EVENT *device_event);
		void handleTrackingMessage(const LEAP_TRACKING_EVENT *tracking_event);
		
		void pinchChecks(const LEAP_HAND* hand);
		void indexPinchChecks(const LEAP_HAND* hand);
		void middlePinchChecks(const LEAP_HAND* hand);
		void ringPinchChecks(const LEAP_HAND* hand);
		void pinkyPinchChecks(const LEAP_HAND* hand);
		void fistChecks(const LEAP_HAND* hand);
		void VChecks(const LEAP_HAND* hand);
		void rotateChecks(const LEAP_HAND* hand);

	private:
		bool pollerRunning_ = false;
		const float pinchThreshold_ =  0.85f;
		const float indexPinchThreshold_  =  25.f;
		const float middlePinchThreshold_ =  35.f;
		const float ringPinchThreshold_   =  25.f;
		const float pinkyPinchThreshold_  =  35.f;
		const float fistThreshold_ =  0.9f;
		const float rotationThreshold_ = 20.f;

		position_callback_t positionCallback_;

		gesture_callback_t pinchStartCallback_;
		gesture_callback_t pinchContinueCallback_;
		gesture_callback_t pinchStopCallback_;

		gesture_callback_t indexPinchStartCallback_;
		gesture_callback_t indexPinchContinueCallback_;
		gesture_callback_t indexPinchStopCallback_;

		gesture_callback_t middlePinchStartCallback_;
		gesture_callback_t middlePinchContinueCallback_;
		gesture_callback_t middlePinchStopCallback_;

		gesture_callback_t ringPinchStartCallback_;
		gesture_callback_t ringPinchContinueCallback_;
		gesture_callback_t ringPinchStopCallback_;

		gesture_callback_t pinkyPinchStartCallback_;
		gesture_callback_t pinkyPinchContinueCallback_;
		gesture_callback_t pinkyPinchStopCallback_;

		gesture_callback_t fistStartCallback_;
		gesture_callback_t fistContinueCallback_;
		gesture_callback_t fistStopCallback_;

		gesture_callback_t VStartCallback_;
		gesture_callback_t VContinueCallback_;
		gesture_callback_t VStopCallback_;

		gesture_callback_t rotateStartCallback_;
		gesture_callback_t rotateContinueCallback_;
		gesture_callback_t rotateStopCallback_;

		LEAP_CONNECTION lc_;
		std::thread pollingThread_;
		
		uint32_t activeHandID = 0;

		bool doingPinch_ = false;
		bool doingIndexPinch_ = false;
		bool doingMiddlePinch_ = false;
		bool doingRingPinch_ = false;
		bool doingPinkyPinch_ = false;
		bool doingFist_ = false;
		bool doingV_ = false;
		bool doingRotate_ = false;
};
