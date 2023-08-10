#include "UltraleapPoller.h"

#include <cmath>
#include <string>

char* errno_to_string(eLeapRS rs)
{
	switch (rs)
	{
	case eLeapRS_Success:
		return "eLeapRS_Success";
	case eLeapRS_UnknownError:
		return "eLeapRS_UnknownError";
	case eLeapRS_InvalidArgument:
		return "eLeapRS_InvalidArgument";
	case eLeapRS_InsufficientResources:
		return "eLeapRS_InsufficientResources";
	case eLeapRS_InsufficientBuffer:
		return "eLeapRS_InsufficientBuffer";
	case eLeapRS_Timeout:
		return "eLeapRS_Timeout";
	case eLeapRS_NotConnected:
		return "eLeapRS_NotConnected";
	case eLeapRS_HandshakeIncomplete:
		return "eLeapRS_HandshakeIncomplete";
	case eLeapRS_BufferSizeOverflow:
		return "eLeapRS_BufferSizeOverflow";
	case eLeapRS_ProtocolError:
		return "eLeapRS_ProtocolError";
	case eLeapRS_InvalidClientID:
		return "eLeapRS_InvalidClientID";
	case eLeapRS_UnexpectedClosed:
		return "eLeapRS_UnexpectedClosed";
	case eLeapRS_UnknownImageFrameRequest:
		return "eLeapRS_UnknownImageFrameRequest";
	case eLeapRS_UnknownTrackingFrameID:
		return "eLeapRS_UnknownTrackingFrameID";
	case eLeapRS_RoutineIsNotSeer:
		return "eLeapRS_RoutineIsNotSeer";
	case eLeapRS_TimestampTooEarly:
		return "eLeapRS_TimestampTooEarly";
	case eLeapRS_ConcurrentPoll:
		return "eLeapRS_ConcurrentPoll";
	case eLeapRS_NotAvailable:
		return "eLeapRS_NotAvailable";
	case eLeapRS_NotStreaming:
		return "eLeapRS_NotStreaming";
	case eLeapRS_CannotOpenDevice:
		return "eLeapRS_CannotOpenDevice";
	case eLeapRS_Unsupported:
		return "eLeapRS_Unsupported";
	default:
		return "Unrecognised error code";
	}
}

UltraleapPoller::UltraleapPoller()
{

	gesturePredicates_[PINCH]        = UltraleapPoller::&isPinching;
	gesturePredicates_[INDEXPINCH]  = UltraleapPoller::&isIndexPinching;
	gesturePredicates_[MIDDLEPINCH] = UltraleapPoller::&isMiddlePinching;
	gesturePredicates_[RINGPINCH]   = UltraleapPoller::&isRingPinching;
	gesturePredicates_[PINKYPINCH]  = UltraleapPoller::&isPinkyPinching;
	gesturePredicates_[FIST]         = UltraleapPoller::&isFist;
	gesturePredicates_[V]            = UltraleapPoller::&isV;
	gesturePredicates_[ROTATE]       = UltraleapPoller::&isRotated;

	gestureStates_[PINCH]        = false;
	gestureStates_[INDEXPINCH]  = false;
	gestureStates_[MIDDLEPINCH] = false;
	gestureStates_[RINGPINCH]   = false;
	gestureStates_[PINKYPINCH]  = false;
	gestureStates_[FIST]         = false;
	gestureStates_[V]            = false;
	gestureStates_[ROTATE]       = false;

	eLeapRS res;
    res = LeapCreateConnection(nullptr, &lc_);
    if (res != eLeapRS_Success)
	{
		printf("Could not create connection. Failed with error: %s", errno_to_string(res));
		exit(EXIT_FAILURE);
	}

    res = LeapOpenConnection(lc_);
    if (res != eLeapRS_Success)
	{
		printf("Could not open connection. Failed with error: %s", errno_to_string(res));
		exit(EXIT_FAILURE);
	}
}

UltraleapPoller::~UltraleapPoller()
{
	StopPoller();
	if (lc_ != nullptr)
	{
		LeapCloseConnection(lc_);
		lc_ = nullptr;
	}	
}

void UltraleapPoller::StartPoller()
{
	pollerRunning_ = true;
    pollingThread_ = std::thread(&UltraleapPoller::runPoller, this);
}

void UltraleapPoller::StopPoller()
{
	if (pollerRunning_)
	{
		pollerRunning_ = false;
		pollingThread_.join();
	}
}

void UltraleapPoller::handleDeviceMessage(const LEAP_DEVICE_EVENT* device_event)
{
	LEAP_DEVICE dev;
	eLeapRS res = LeapOpenDevice(device_event->device, &dev);

	if (res != eLeapRS_Success)
	{
		printf("Could not open device %s.\n", errno_to_string(res));
		return;
	}

	// Create a struct to hold the device properties, we have to provide a buffer for the serial string
	LEAP_DEVICE_INFO deviceProperties = {sizeof(deviceProperties)};
	// Start with a length of 1 (pretending we don't know a priori what the length is).
	// Currently device serial numbers are all the same length, but that could change in the future
	deviceProperties.serial_length = 1;
	deviceProperties.serial = reinterpret_cast<char *>(malloc(deviceProperties.serial_length));
	// This will fail since the serial buffer is only 1 character long
	//  But deviceProperties is updated to contain the required buffer length
	res = LeapGetDeviceInfo(dev, &deviceProperties);
	if (res == eLeapRS_InsufficientBuffer)
	{
		// try again with correct buffer size
		deviceProperties.serial = reinterpret_cast<char *>(realloc(deviceProperties.serial, deviceProperties.serial_length));
		res = LeapGetDeviceInfo(dev, &deviceProperties);
		if (res != eLeapRS_Success)
		{
			printf("Failed to get device info %s.\n", errno_to_string(res));
			free(deviceProperties.serial);
			return;
		}
	}
	printf("Device found: %s", deviceProperties.serial);

  free(deviceProperties.serial);
  LeapCloseDevice(dev);
}

void UltraleapPoller::doChecks(const UltraleapPollerGestures gesture, const LEAP_HAND* hand)
{
  if (gesturePredicates_.find(gesture)->second(hand))
  {
		if (gestureStates.find(gesture)->second)
		{
			states_callback_t::iterator it = gestureContinueCallbacks_.find(gesture);
			if (it != gestureContinueCallbacks_.end())
			{
				it->second(hand->type);
			}
		}
		else
		{
			states_callback_t::iterator it = gestureStartCallbacks_.find(gesture);
			if (it != gestureStartCallbacks_.end())
			{
				it->second(hand->type);
			}
			gestureStates.find(gesture).second_ = true;
		}
  }
  else
  {
		if (gestureStates.find(gesture).second_)
		{
			states_callback_t::iterator it = gestureStopCallbacks_.find(gesture);
			if (it != gestureStopCallbacks_.end())
			{
				it->second(hand->type);
			}
			gestureStates.find(gesture).second_ = false;
		}
  }
}

void pinchChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::PINCH, hand);
}

void UltraleapPoller::indexPinchChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::INDEXPINCH, hand);
}

void UltraleapPoller::middlePinchChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::MIDDLEPINCH, hand);
}

void UltraleapPoller::ringPinchChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::RINGPINCH, hand);
}

void UltraleapPoller::pinkyPinchChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::PINKYPINCH, hand);
}

void UltraleapPoller::fistChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::FIST, hand);
}

void UltraleapPoller::VChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::V, hand);
}

void UltraleapPoller::rotateChecks(const LEAP_HAND* hand)
{
	doChecks(UltraleapPollerGestures::ROTATE, hand);
}

void UltraleapPoller::handleTrackingMessage(const LEAP_TRACKING_EVENT* tracking_event)
{
  if (tracking_event->nHands)
  {
		for (uint8_t h = 0; h < tracking_event->nHands; h++)
		{
			LEAP_HAND hand = tracking_event->pHands[h];
			if (activeHandID != 0)
			{
				if (hand.id != activeHandID)
				{
					continue;
				}
				else
				{
					// Do hand stuff.
					if (positionCallback_)
					{
						positionCallback_(hand.palm.position);
					}
					
					pinchChecks(&hand);
					indexPinchChecks(&hand);
					middlePinchChecks(&hand);
					ringPinchChecks(&hand);
					pinkyPinchChecks(&hand);
					fistChecks(&hand);
					VChecks(&hand);
					rotateChecks(&hand);
				}
			}
			else
			{
				activeHandID = hand.id;
			}
		}
  }
  else // Can this happen?? A tracking message that just called to say hi?
  {
		activeHandID = 0;
  }
}

void UltraleapPoller::runPoller()
{
	LEAP_CONNECTION_MESSAGE msg;
	
	while (pollerRunning_)
	{
	    if (eLeapRS_Success != LeapPollConnection(lc_, 0, &msg))
		{
            continue;
		}	
		
		switch (msg.type)
		{
			case eLeapEventType_Device:
				handleDeviceMessage(msg.device_event);
				break;
			case eLeapEventType_Tracking:
				handleTrackingMessage(msg.tracking_event);
				break;
			default:
			    printf("No intention of supporting this\n");
				break;
		}
	}
}

void UltraleapPoller::SetPositionCallback(position_callback_t callback)
{
	positionCallback_ = callback;
}

void UltraleapPoller::ClearPositionCallback()
{
	positionCallback_ = nullptr;
} \

#define SET_AND_CLEAR_CALLBACKS(GESTURE) \
void UltraleapPoller::SetOn##GESTURE##StartCallback(gesture_callback_t callback) \
{ \
	gestureStartCallbacks_[UltraleapPollerGestures::## normalize(GESTURE)] = callback; \
} \
void UltraleapPoller::SetOn##GESTURE##ContinueCallback(gesture_callback_t callback) \
{ \
	gestureStartContinue_[UltraleapPollerGestures::## normalize(GESTURE)] = callback; \
} \
void UltraleapPoller::SetOn##GESTURE##StopCallback(gesture_callback_t callback) \
{ \
	gestureStopContinue_[UltraleapPollerGestures::## normalize(GESTURE)] = callback; \
} \
void UltraleapPoller::ClearOn##GESTURE##StartCallback() \
{ \
	gestureStartCallbacks_[UltraleapPollerGestures::## normalize(GESTURE)] = nullptr; \
} \
void UltraleapPoller::ClearOn##GESTURE##ContinueCallback() \
{ \
	gestureStartContinue_[UltraleapPollerGestures::## normalize(GESTURE)] = nullptr; \
} \
void UltraleapPoller::ClearOn##GESTURE##StopCallback() \
{ \
	gestureStopContinue_[UltraleapPollerGestures::## normalize(GESTURE)] = callback; \
}

SET_AND_CLEAR_CALLBACKS(Pinch);
SET_AND_CLEAR_CALLBACKS(IndexPinch);
SET_AND_CLEAR_CALLBACKS(MiddlePinch);
SET_AND_CLEAR_CALLBACKS(RingPinch);
SET_AND_CLEAR_CALLBACKS(PinkyPinch);
SET_AND_CLEAR_CALLBACKS(Fist);
SET_AND_CLEAR_CALLBACKS(V);
SET_AND_CLEAR_CALLBACKS(Rotate);

float UltraleapPoller::distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const
{
	return static_cast<float>(std::sqrt(std::pow(second.x - first.x, 2) +
										std::pow(second.y - first.y, 2) +
										std::pow(second.z - first.z, 2)));
}

bool UltraleapPoller::isPinching(const LEAP_HAND* hand) const
{
	return hand->pinch_strength > pinchThreshold_;
}

bool UltraleapPoller::isIndexPinching(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  < indexPinchThreshold_ &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) > indexPinchThreshold_ &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   > indexPinchThreshold_ &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  > indexPinchThreshold_;
}


bool UltraleapPoller::isMiddlePinching(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  > middlePinchThreshold_ &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) < middlePinchThreshold_ &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   > middlePinchThreshold_ &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  > middlePinchThreshold_;
}

bool UltraleapPoller::isRingPinching(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  > ringPinchThreshold_ &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) > ringPinchThreshold_ &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   < ringPinchThreshold_ &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  > ringPinchThreshold_;
}

bool UltraleapPoller::isPinkyPinching(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  > pinkyPinchThreshold_ &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) > pinkyPinchThreshold_ &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   > pinkyPinchThreshold_ &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  < pinkyPinchThreshold_;
}

bool UltraleapPoller::isFist(const LEAP_HAND* hand) const
{
	return hand->grab_strength > fistThreshold_;
}

bool UltraleapPoller::isV(const LEAP_HAND* hand) const
{
     return distance(hand->index.distal.next_joint, hand->palm.position)  > 70.f &&
            distance(hand->middle.distal.next_joint, hand->palm.position) > 70.f &&
            distance(hand->ring.distal.next_joint, hand->palm.position)   < 40.f &&
            distance(hand->pinky.distal.next_joint, hand->palm.position)  < 40.f;
}

bool UltraleapPoller::isRotated(const LEAP_HAND* hand) const
{
     return std::sqrt(std::pow(hand->index.proximal.prev_joint.x - hand->pinky.proximal.prev_joint.x, 2) +
                      std::pow(hand->index.proximal.prev_joint.z - hand->pinky.proximal.prev_joint.z, 2)) < rotationThreshold_;
}

