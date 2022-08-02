#ifndef EVENTQUEUE_HPP
# define EVENTQUEUE_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <unistd.h>
# include <iostream>
# include "EventFlag.hpp"
# include "AIoEvent.hpp"

class AIoEvent;

class EventQueue
{
	public:
		EventQueue();
		~EventQueue();

		void		SetIoEvent(AIoEvent *io_event, e_EventType type, const e_EventAction act) const;
		AIoEvent*	WaitIoEvent() const;

	private:
		int		kq_;
};

#endif  // LISTENSOCKET_HPP
