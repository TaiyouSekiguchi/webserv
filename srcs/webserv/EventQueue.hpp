#ifndef EVENTQUEUE_HPP
# define EVENTQUEUE_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <unistd.h>
# include <iostream>
# include "EventFlag.hpp"
# include "AEvent.hpp"

class EventQueue
{
	public:
		EventQueue();
		~EventQueue();

		void	SetIoEvent(const int fd, const e_EventType type, const e_EventAction act, AEvent *event);
		AEvent*	WaitIoEvent() const;

	private:
		int		kq_;
};

#endif  // LISTENSOCKET_HPP
