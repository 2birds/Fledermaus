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
					//printf("x=%f, y=%f, z=%f\n", hand.palm.position.x, hand.palm.position.y, hand.palm.position.z);
					if (limitTrackingToWithinBounds
						&& hand.palm.position.x * 0.001f < -boundsLeftM || hand.palm.position.x * 0.001f > boundsRightM
						|| hand.palm.position.y * 0.001f < -boundsLowerM || hand.palm.position.y * 0.001f > boundsUpperM
						|| hand.palm.position.z * 0.001f > boundsNearM || hand.palm.position.z * 0.001f < -boundsFarM)
					{
						continue;
					}

					// Do hand stuff.
					if (positionCallback_)
					{
						positionCallback_(hand.palm.position);
					}
					
					int64_t timestamp = tracking_event->info.timestamp;

					// The following need to be added manually, the macro can't do it
					AlmostPinchChecks(timestamp, &hand);
					FistChecks(timestamp, &hand);
					if (!doingFist_) // A fist is also detected as a pinch, but not the other way round
					{
					    PinchChecks(timestamp, &hand);
						IndexPinchChecks(timestamp, &hand);
						MiddlePinchChecks(timestamp, &hand);
						RingPinchChecks(timestamp, &hand);
						PinkyPinchChecks(timestamp, &hand);
						VChecks(timestamp, &hand);
						AlmostRotateChecks(timestamp, &hand);
						RotateChecks(timestamp, &hand);
					}
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
			    printf("Received unsupported message\n");
				break;
		}
	}
}

void UltraleapPoller::SetPositionCallback(position_callback_t callback)
{
	positionCallback_ = callback;
}

#define AddGestureCallbackSettersDefinition(name) \
void UltraleapPoller::SetOn##name##StartCallback(gesture_callback_t callback) \
{ \
	name##StartCallback_ = callback; \
} \
void UltraleapPoller::SetOn##name##ContinueCallback(gesture_callback_t callback) \
{ \
	name##ContinueCallback_ = callback; \
} \
void UltraleapPoller::SetOn##name##StopCallback(gesture_callback_t callback) \
{ \
	name##StopCallback_ = callback; \
} \
void UltraleapPoller::ClearOn##name##StartCallback() \
{ \
	name##StartCallback_ = nullptr; \
} \
void UltraleapPoller::ClearOn##name##ContinueCallback() \
{ \
	name##ContinueCallback_ = nullptr; \
} \
void UltraleapPoller::ClearOn##name##StopCallback() \
{ \
	name##StopCallback_ = nullptr; \
} \
void UltraleapPoller::name##Checks(const int64_t timestamp, const LEAP_HAND* hand) \
{ \
  if (is##name##(hand)) \
  { \
		if (doing##name##_) \
		{ \
			if (name##ContinueCallback_) \
			{ \
				name##ContinueCallback_(timestamp, *hand); \
			} \
		} \
		else \
		{ \
			if (name##StartCallback_) \
			{ \
				name##StartCallback_(timestamp, *hand); \
			} \
			doing##name##_ = true; \
		} \
  } \
  else \
  { \
		if (doing##name##_) \
		{ \
			if (name##StopCallback_) \
			{ \
				name##StopCallback_(timestamp, *hand); \
			} \
			doing##name##_ = false; \
		} \
  } \
}


AddGestureCallbackSettersDefinition(AlmostPinch);
AddGestureCallbackSettersDefinition(Pinch);
AddGestureCallbackSettersDefinition(IndexPinch);
AddGestureCallbackSettersDefinition(MiddlePinch);
AddGestureCallbackSettersDefinition(RingPinch);
AddGestureCallbackSettersDefinition(PinkyPinch);
AddGestureCallbackSettersDefinition(Fist);
AddGestureCallbackSettersDefinition(V);
AddGestureCallbackSettersDefinition(AlmostRotate);
AddGestureCallbackSettersDefinition(Rotate);

float UltraleapPoller::distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const
{
	return static_cast<float>(std::sqrt(std::pow(second.x - first.x, 2) +
										std::pow(second.y - first.y, 2) +
										std::pow(second.z - first.z, 2)));
}

LEAP_VECTOR UltraleapPoller::difference(const LEAP_VECTOR first, const LEAP_VECTOR second) const
{
    LEAP_VECTOR ret;
    ret.x = first.x - second.x;
    ret.y = first.y - second.y;
    ret.z = first.z - second.z;
	return ret;
}

float UltraleapPoller::dot(const LEAP_VECTOR first, const LEAP_VECTOR second) const
{
	return (first.x * second.x) +
	       (first.y * second.y) +
	       (first.z * second.z);
}

float UltraleapPoller::magnitude(const LEAP_VECTOR vec) const
{
	return std::sqrt(
		(vec.x * vec.x) +
		(vec.y * vec.y) +
		(vec.z * vec.z));
}

// The following gesture tests need to be added manually and match names given to the macro above
bool UltraleapPoller::isAlmostPinch(const LEAP_HAND* hand) const
{
	return hand->pinch_strength < pinchThreshold_ && hand->pinch_strength > (pinchThreshold_ - 0.15);
}

bool UltraleapPoller::isPinch(const LEAP_HAND* hand) const
{
	return hand->pinch_strength > pinchThreshold_;
}

bool UltraleapPoller::isIndexPinch(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  < indexPinchThreshold &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) > indexPinchThreshold &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   > indexPinchThreshold &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  > indexPinchThreshold;
}


bool UltraleapPoller::isMiddlePinch(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  > middlePinchThreshold_ &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) < middlePinchThreshold_ &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   > middlePinchThreshold_ &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  > middlePinchThreshold_;
}

bool UltraleapPoller::isRingPinch(const LEAP_HAND* hand) const
{
	return distance(hand->index.distal.next_joint, hand->thumb.distal.next_joint)  > ringPinchThreshold_ &&
	       distance(hand->middle.distal.next_joint, hand->thumb.distal.next_joint) > ringPinchThreshold_ &&
	       distance(hand->ring.distal.next_joint, hand->thumb.distal.next_joint)   < ringPinchThreshold_ &&
	       distance(hand->pinky.distal.next_joint, hand->thumb.distal.next_joint)  > ringPinchThreshold_;
}

bool UltraleapPoller::isPinkyPinch(const LEAP_HAND* hand) const
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

// bool UltraleapPoller::isV(const LEAP_HAND* hand) const
// {
//      return distance(hand->index.distal.next_joint, hand->palm.position)  > 65.f &&
//             distance(hand->middle.distal.next_joint, hand->palm.position) > 65.f &&
//             distance(hand->ring.distal.next_joint, hand->palm.position)   < 40.f &&
//             distance(hand->pinky.distal.next_joint, hand->palm.position)  < 40.f;
// }

bool UltraleapPoller::isV(const LEAP_HAND* hand) const
{
     LEAP_VECTOR index_vec  = difference(hand->index.distal.next_joint, hand->index.distal.prev_joint);
     LEAP_VECTOR middle_vec = difference(hand->middle.distal.next_joint, hand->middle.distal.prev_joint);
     // LEAP_VECTOR ring_vec   = difference(hand->ring.distal.next_joint, hand->ring.distal.prev_joint);
     LEAP_VECTOR pinky_vec  = difference(hand->pinky.distal.next_joint, hand->pinky.distal.prev_joint);

	 float index_middle_cos = dot(index_vec, middle_vec) / (magnitude(index_vec) * magnitude(middle_vec));
	 // float ring_pinky_cos = dot(ring_vec, pinky_vec) / (magnitude(ring_vec) * magnitude(pinky_vec));
	 float index_pinky_cos = dot(index_vec, pinky_vec) / (magnitude(index_vec) * magnitude(pinky_vec));

     float thresh = 0.6f;
	 return index_middle_cos >  thresh &&
	        // ring_pinky_cos   >  thresh &&
	        index_pinky_cos  < 0;
}

bool UltraleapPoller::isAlmostRotate(const LEAP_HAND* hand) const
{
     double angle = std::sqrt(std::pow(hand->index.proximal.prev_joint.x - hand->pinky.proximal.prev_joint.x, 2) +
                      std::pow(hand->index.proximal.prev_joint.z - hand->pinky.proximal.prev_joint.z, 2));
     return angle > rotationThreshold_ && angle < rotationThreshold_ + 20.0;
}

bool UltraleapPoller::isRotate(const LEAP_HAND* hand) const
{
     return std::sqrt(std::pow(hand->index.proximal.prev_joint.x - hand->pinky.proximal.prev_joint.x, 2) +
                      std::pow(hand->index.proximal.prev_joint.z - hand->pinky.proximal.prev_joint.z, 2)) < rotationThreshold_;
}

