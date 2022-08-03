#ifndef ACCPETCLIENTEVENT_HPP
# define ACCPETCLIENTEVENT_HPP

# include "ListenSocket.hpp"
# include "AEvent.hpp"
# include "EventQueue.hpp"

class AcceptClientEvent : public AEvent
{
	public:
		explicit AcceptClientEvent(const ListenSocket* lsocket);
		~AcceptClientEvent();

		void	RunEvent(EventQueue* equeue);

	private:
		const ListenSocket*	lsocket_;
};

#endif
