#include "UltraleapPoller.h"

#include <cmath>


void UltraleapPoller::SetPositionCallback(LEAP_HAND* hands, int nhands){}

void UltraleapPoller::SetOnPinchStartCallback(eLeapHandType chirality){}
void UltraleapPoller::SetOnPinchContinueCallback(eLeapHandType chirality){}
void UltraleapPoller::SetOnPinchEnd(eLeapHandType chirality){}

void UltraleapPoller::SetOnFistStartCallback(eLeapHandType chirality){}
void UltraleapPoller::SetOnFistContinueCallback(eLeapHandType chirality){}
void UltraleapPoller::SetOnFistEnd(eLeapHandType chirality){}

void UltraleapPoller::SetOnVStartCallback(eLeapHandType chirality){}
void UltraleapPoller::SetOnVContinueCallback(eLeapHandType chirality){}
void UltraleapPoller::SetOnVEnd(eLeapHandType chirality){}

float distance(const LEAP_VECTOR first, const LEAP_VECTOR second) const
{
	return std::sqrt(std::exp(second.x - first.x, 2),
	                 std::exp(second.y - first.y, 2),
	                 std::exp(second.z - first.z, 2));
}

bool UltraleapPoller::isPinching(const LEAP_HAND* hand) const
{
	return hand->pinch_strength > pinchThreshold_;
}

bool UltraleapPoller::isFist(const LEAP_HAND* hand) const
{
	return hand->grab_strength > fistThreshold_;
}

bool UltraleapPoller::isV(const LEAP_HAND* hand) const
{
     return distance(hand->fingers.index.distal.next_joint, hand->palm.position)  > 80f &&
            distance(hand->fingers.middle.distal.next_joint, hand->palm.position) > 80f &&
            distance(hand->fingers.ring.distal.next_joint, hand->palm.position)   < 40f &&
            distance(hand->fingers.pinky.distal.next_joint, hand->palm.position)  < 40f;
}

