#include "LeapC.h"

#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>

typedef std::function<void(LEAP_VECTOR)> position_callback_t;
typedef std::function<void(const int64_t, const LEAP_HAND&)> gesture_callback_t;

class UltraleapPoller
{
    public:
        UltraleapPoller();
        ~UltraleapPoller();
        
        void StartPoller();
        void StopPoller();

        // Fires on each update with a hand
        void SetPositionCallback(position_callback_t callback);
        void ClearPositionCallback();

        float distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const;

// This macro sets up all callback setters and getters, tests, and flags related to a particular gesture..
// EXCEPT the functions and values actually responsible for detecting the gesture.
#define AddGestureCallbackSetters(name) \
        public: \
        void SetOn##name##StartCallback(gesture_callback_t callback); \
        void SetOn##name##ContinueCallback(gesture_callback_t callback); \
        void SetOn##name##StopCallback(gesture_callback_t callback); \
        void ClearOn##name##StartCallback(); \
        void ClearOn##name##ContinueCallback(); \
        void ClearOn##name##StopCallback(); \
        private: \
        gesture_callback_t name##StartCallback_; \
        gesture_callback_t name##ContinueCallback_; \
        gesture_callback_t name##StopCallback_; \
        bool doing##name##_ = false; \
        void name##Checks(const int64_t timestamp, const LEAP_HAND* hand); \
        bool is##name(const LEAP_HAND* hand) const; \

        AddGestureCallbackSetters(AlmostPinch);
        AddGestureCallbackSetters(Pinch);
        AddGestureCallbackSetters(IndexPinch);
        AddGestureCallbackSetters(MiddlePinch);
        AddGestureCallbackSetters(RingPinch);
        AddGestureCallbackSetters(PinkyPinch);
        AddGestureCallbackSetters(Fist);
        AddGestureCallbackSetters(V);
        AddGestureCallbackSetters(AlmostRotate);
        AddGestureCallbackSetters(Rotate);

    private:
        void runPoller();
        LEAP_VECTOR difference(const LEAP_VECTOR first, const LEAP_VECTOR second) const;
        float dot(const LEAP_VECTOR first, const LEAP_VECTOR second) const;
        float magnitude(const LEAP_VECTOR vec) const;

        void handleDeviceMessage(const LEAP_DEVICE_EVENT *device_event);
        void handleTrackingMessage(const LEAP_TRACKING_EVENT *tracking_event);

    private:
        bool pollerRunning_ = false;
        const float pinchThreshold_ =  0.85f;
        const float indexPinchThreshold_  =  35.f;
        const float middlePinchThreshold_ =  35.f;
        const float ringPinchThreshold_   =  25.f;
        const float pinkyPinchThreshold_  =  35.f;
        const float fistThreshold_ =  0.5f;
        const float rotationThreshold_ = 20.f;

        position_callback_t positionCallback_;

        LEAP_CONNECTION lc_;
        std::thread pollingThread_;
        
        uint32_t activeHandID = 0;
};
