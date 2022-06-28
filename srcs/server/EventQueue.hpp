#ifndef EVENTQUEUE_HPP
# define EVENTQUEUE_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

class EventQueue
{
	public:
		EventQueue();
		~EventQueue();

		void	RegisterEvent(int sock) const;
		int		WaitEvent(void) const;

	private:
		int					kq_;
};

#endif  // LISTENSOCKET_HPP
