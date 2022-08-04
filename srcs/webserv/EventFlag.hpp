#ifndef EVENTFLAG_HPP
# define EVENTFLAG_HPP

# include <sys/event.h>

enum	e_EventType
{
	ET_READ = EVFILT_READ,
	ET_WRITE = EVFILT_WRITE,
};

enum	e_EventAction
{
	EA_ADD		= EV_ADD,
	EA_ENABLE	= EV_ENABLE,
	EA_DISABLE	= EV_DISABLE,
	EA_DELETE	= EV_DELETE
};

#endif
