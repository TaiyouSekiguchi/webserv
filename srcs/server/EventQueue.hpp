#ifndef EVENTQUEUE_HPP
# define EVENTQUEUE_HPP

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <unistd.h>
# include <iostream>
# include "ASocket.hpp"
class EventQueue
{
	public:
		EventQueue();
		~EventQueue();

		void	RegisterEvent(const int fd, void *udata) const;
		void	*WaitEvent() const;

	private:
		int		kq_;
};

#endif  // LISTENSOCKET_HPP
