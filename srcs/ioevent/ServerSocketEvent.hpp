#ifndef SERVERSOCKETEVENT_HPP
# define SERVERSOCKETEVENT_HPP

# include "ServerSocket.hpp"
# include "AIoEvent.hpp"
# include "AServerIoEvent.hpp"
# include "HTTPServer.hpp"

class ServerSocketEvent : public AServerIoEvent
{
	public:
		explicit ServerSocketEvent(const ServerSocket* ssocket);
		~ServerSocketEvent();

		int				GetFd() const;
		e_EventStatus	RunEvent(EventQueue* equeue);

	private:
		e_EventStatus	RunReadEvent(EventQueue* equeue);
		e_EventStatus	RunWriteEvent(EventQueue* equeue);

		const ServerSocket*	ssocket_;
};

#endif
