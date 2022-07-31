#include <unistd.h>
#include "AIoEvent.hpp"

AIoEvent::AIoEvent(const e_EventType event_type)
	: event_type_(event_type)
{
}

AIoEvent::~AIoEvent()
{
}

e_EventType		AIoEvent::GetEventType() const { return (event_type_); }
