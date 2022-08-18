#ifndef AEVENT_HPP
# define AEVENT_HPP

# include <sys/event.h>

class AEvent
{
	public:
		AEvent();
		virtual ~AEvent() = 0;
};

#endif
