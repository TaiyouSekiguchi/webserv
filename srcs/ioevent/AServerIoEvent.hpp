#ifndef ASERVERIOEVENT_HPP
# define ASERVERIOEVENT_HPP

# include "HTTPServer.hpp"

class AServerIoEvent : public AIoEvent
{
	public:
		explicit AServerIoEvent(const e_EventType type);
		virtual ~AServerIoEvent() = 0;

		void	SetServerInfo(HTTPServer* hserver);

	protected:
		void	RegisterServerEvent(EventQueue* equeue, AServerIoEvent *new_server_event);

		HTTPServer*		hserver_;
};

#endif
