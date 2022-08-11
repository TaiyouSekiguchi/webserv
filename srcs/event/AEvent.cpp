#include "AEvent.hpp"

AEvent::AEvent()
{
}

AEvent::~AEvent()
{
}

void	AEvent::SetFlags(const u_short flags) { flags_ = flags; }

bool	AEvent::IsEOF() const
{
	return (flags_ & EV_EOF);
}
