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

void UltraleapPoller::pinchChecks(const LEAP_HAND* hand)
{
  if (isPinching(hand))
  {
		if (doingPinch_)
		{
			if (pinchContinueCallback_)
			{
				pinchContinueCallback_(hand->type);
			}
		}
		else
		{
			if (pinchStartCallback_)
			{
				pinchStartCallback_(hand->type);
			}
			doingPinch_ = true;
		}
  }
  else
  {
		if (doingPinch_)
		{
			if (pinchStopCallback_)
			{
				pinchStopCallback_(hand->type);
			}
			doingPinch_ = false;
		}
  }
}

void UltraleapPoller::indexPinchChecks(const LEAP_HAND* hand)
{
  if (isIndexPinching(hand))
  {
		if (doingIndexPinch_)
		{
			if (indexPinchContinueCallback_)
			{
				indexPinchContinueCallback_(hand->type);
			}
		}
		else
		{
			if (indexPinchStartCallback_)
			{
				indexPinchStartCallback_(hand->type);
			}
			doingIndexPinch_ = true;
		}
  }
  else
  {
		if (doingIndexPinch_)
		{
			if (indexPinchStopCallback_)
			{
				indexPinchStopCallback_(hand->type);
			}
			doingIndexPinch_ = false;
		}
  }
}

void UltraleapPoller::middlePinchChecks(const LEAP_HAND* hand)
{
  if (isMiddlePinching(hand))
  {
		if (doingMiddlePinch_)
		{
			if (middlePinchContinueCallback_)
			{
				middlePinchContinueCallback_(hand->type);
			}
		}
		else
		{
			if (middlePinchStartCallback_)
			{
				middlePinchStartCallback_(hand->type);
			}
			doingMiddlePinch_ = true;
		}
  }
  else
  {
		if (doingMiddlePinch_)
		{
			if (middlePinchStopCallback_)
			{
				middlePinchStopCallback_(hand->type);
			}
			doingMiddlePinch_ = false;
		}
  }
}

void UltraleapPoller::ringPinchChecks(const LEAP_HAND* hand)
{
  if (isRingPinching(hand))
  {
		if (doingRingPinch_)
		{
			if (ringPinchContinueCallback_)
			{
				ringPinchContinueCallback_(hand->type);
			}
		}
		else
		{
			if (ringPinchStartCallback_)
			{
				ringPinchStartCallback_(hand->type);
			}
			doingRingPinch_ = true;
		}
  }
  else
  {
		if (doingRingPinch_)
		{
			if (ringPinchStopCallback_)
			{
				ringPinchStopCallback_(hand->type);
			}
			doingRingPinch_ = false;
		}
  }
}

void UltraleapPoller::pinkyPinchChecks(const LEAP_HAND* hand)
{
  if (isPinkyPinching(hand))
  {
		if (doingPinkyPinch_)
		{
			if (pinkyPinchContinueCallback_)
			{
				pinkyPinchContinueCallback_(hand->type);
			}
		}
		else
		{
			if (pinkyPinchStartCallback_)
			{
				pinkyPinchStartCallback_(hand->type);
			}
			doingPinkyPinch_ = true;
		}
  }
  else
  {
		if (doingPinkyPinch_)
		{
			if (pinkyPinchStopCallback_)
			{
				pinkyPinchStopCallback_(hand->type);
			}
			doingPinkyPinch_ = false;
		}
  }
}

void UltraleapPoller::fistChecks(const LEAP_HAND* hand)
{
  if (isFist(hand))
  {
		if (doingFist_)
		{
			if (fistContinueCallback_)
			{
				fistContinueCallback_(hand->type);
			}
		}
		else
		{
			if (fistStartCallback_)
			{
				fistStartCallback_(hand->type);
			}
			doingFist_ = true;
		}
  }
  else
  {
		if (doingFist_)
		{
			if (fistStopCallback_)
			{
				fistStopCallback_(hand->type);
			}
			doingFist_ = false;
		}
  }
}

void UltraleapPoller::VChecks(const LEAP_HAND* hand)
{
  if (isV(hand))
  {
		if (doingV_)
		{
			if (VContinueCallback_)
			{
				VContinueCallback_(hand->type);
			}
		}
		else
		{
			if (VStartCallback_)
			{
				VStartCallback_(hand->type);
			}
			doingV_ = true;
		}
  }
  else
  {
		if (doingV_)
		{
			if (VStopCallback_)
			{
				VStopCallback_(hand->type);
			}
			doingV_ = false;
		}
  }
}

void UltraleapPoller::rotateChecks(const LEAP_HAND* hand)
{
  if (isRotated(hand))
  {
		if (doingRotate_)
		{
			if (rotateContinueCallback_)
			{
				rotateContinueCallback_(hand->type);
			}
		}
		else
		{
			if (rotateStartCallback_)
			{
				rotateStartCallback_(hand->type);
			}
			doingRotate_ = true;
		}
  }
  else
  {
		if (doingRotate_)
		{
			if (rotateStopCallback_)
			{
				rotateStopCallback_(hand->type);
			}
			doingRotate_ = false;
		}
  }
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

void UltraleapPoller::SetOnPinchStartCallback(gesture_callback_t callback)
{
	pinchStartCallback_ = callback;
}
void UltraleapPoller::SetOnPinchContinueCallback(gesture_callback_t callback)
{
	pinchContinueCallback_ = callback;
}
void UltraleapPoller::SetOnPinchStopCallback(gesture_callback_t callback)
{
	pinchStopCallback_ = callback;
}

void UltraleapPoller::SetOnIndexPinchStartCallback(gesture_callback_t callback)
{
	indexPinchStartCallback_ = callback;
}
void UltraleapPoller::SetOnIndexPinchContinueCallback(gesture_callback_t callback)
{
	indexPinchContinueCallback_ = callback;
}
void UltraleapPoller::SetOnIndexPinchStopCallback(gesture_callback_t callback)
{
	indexPinchStopCallback_ = callback;
}

void UltraleapPoller::SetOnMiddlePinchStartCallback(gesture_callback_t callback)
{
	middlePinchStartCallback_ = callback;
}
void UltraleapPoller::SetOnMiddlePinchContinueCallback(gesture_callback_t callback)
{
	middlePinchContinueCallback_ = callback;
}
void UltraleapPoller::SetOnMiddlePinchStopCallback(gesture_callback_t callback)
{
	middlePinchStopCallback_ = callback;
}

void UltraleapPoller::SetOnRingPinchStartCallback(gesture_callback_t callback)
{
	ringPinchStartCallback_ = callback;
}
void UltraleapPoller::SetOnRingPinchContinueCallback(gesture_callback_t callback)
{
	ringPinchContinueCallback_ = callback;
}
void UltraleapPoller::SetOnRingPinchStopCallback(gesture_callback_t callback)
{
	ringPinchStopCallback_ = callback;
}

void UltraleapPoller::SetOnPinkyPinchStartCallback(gesture_callback_t callback)
{
	pinkyPinchStartCallback_ = callback;
}
void UltraleapPoller::SetOnPinkyPinchContinueCallback(gesture_callback_t callback)
{
	pinkyPinchContinueCallback_ = callback;
}
void UltraleapPoller::SetOnPinkyPinchStopCallback(gesture_callback_t callback)
{
	pinkyPinchStopCallback_ = callback;
}

void UltraleapPoller::SetOnFistStartCallback(gesture_callback_t callback)
{
	fistStartCallback_ = callback;
}
void UltraleapPoller::SetOnFistContinueCallback(gesture_callback_t callback)
{
	fistContinueCallback_ = callback;
}
void UltraleapPoller::SetOnFistStopCallback(gesture_callback_t callback)
{
	fistStopCallback_ = callback;
}

void UltraleapPoller::SetOnVStartCallback(gesture_callback_t callback)
{
	VStartCallback_ = callback;
}
void UltraleapPoller::SetOnVContinueCallback(gesture_callback_t callback)
{
	VContinueCallback_ = callback;
}
void UltraleapPoller::SetOnVStopCallback(gesture_callback_t callback)
{
	VStopCallback_ = callback;
}

void UltraleapPoller::SetOnRotateStartCallback(gesture_callback_t callback)
{
	rotateStartCallback_ = callback;
}
void UltraleapPoller::SetOnRotateContinueCallback(gesture_callback_t callback)
{
	rotateContinueCallback_ = callback;
}
void UltraleapPoller::SetOnRotateStopCallback(gesture_callback_t callback)
{
	rotateStopCallback_ = callback;
}

void UltraleapPoller::ClearPositionCallback()
{
	positionCallback_ = nullptr;
}

void UltraleapPoller::ClearOnPinchStartCallback()
{
	pinchStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnPinchContinueCallback()
{
	pinchContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnPinchStopCallback()
{
	pinchStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnIndexPinchStartCallback()
{
	indexPinchStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnIndexPinchContinueCallback()
{
	indexPinchContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnIndexPinchStopCallback()
{
	indexPinchStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnMiddlePinchStartCallback()
{
	middlePinchStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnMiddlePinchContinueCallback()
{
	middlePinchContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnMiddlePinchStopCallback()
{
	middlePinchStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnRingPinchStartCallback()
{
	ringPinchStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnRingPinchContinueCallback()
{
	ringPinchContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnRingPinchStopCallback()
{
	ringPinchStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnPinkyPinchStartCallback()
{
	pinkyPinchStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnPinkyPinchContinueCallback()
{
	pinkyPinchContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnPinkyPinchStopCallback()
{
	pinkyPinchStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnFistStartCallback()
{
	fistStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnFistContinueCallback()
{
	fistContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnFistStopCallback()
{
	fistStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnVStartCallback()
{
	VStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnVContinueCallback()
{
	VContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnVStopCallback()
{
	VStopCallback_ = nullptr;
}

void UltraleapPoller::ClearOnRotateStartCallback()
{
	rotateStartCallback_ = nullptr;
}
void UltraleapPoller::ClearOnRotateContinueCallback()
{
	rotateContinueCallback_ = nullptr;
}
void UltraleapPoller::ClearOnRotateStopCallback()
{
	rotateStopCallback_ = nullptr;
}

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

