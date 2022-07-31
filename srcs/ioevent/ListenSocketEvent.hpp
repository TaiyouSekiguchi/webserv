#ifndef LISTENSOCKETEVENT_HPP
# define LISTENSOCKETEVENT_HPP

# include "ListenSocket.hpp"
# include "AIoEvent.hpp"

class ListenSocketEvent : public AIoEvent
{
	public:
		explicit ListenSocketEvent(const ListenSocket* lsocket);
		~ListenSocketEvent();

		int				GetFd() const;
		e_EventStatus	RunEvent(EventQueue* equeue);

	private:
		const ListenSocket*	lsocket_;
};

#endif
