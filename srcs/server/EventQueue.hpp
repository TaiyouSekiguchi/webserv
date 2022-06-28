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

		void	CreateQueue(void);
		void	RegisterEvent(int sock);
		int		WaitEvent(struct kevent * kev);

	private:
		int					kq_;
		struct timespec		waitspec_;
};

#endif  // LISTENSOCKET_HPP
