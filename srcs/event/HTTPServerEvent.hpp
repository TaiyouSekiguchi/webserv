#ifndef HTTPSERVEREVENT_HPP
# define HTTPSERVEREVENT_HPP

# include "AEvent.hpp"
# include "EventQueue.hpp"
# include "ServerSocket.hpp"
# include "HTTPServerEventType.hpp"
# include "HTTPServer.hpp"

class HTTPServerEvent : public AEvent
{
	public:
		explicit HTTPServerEvent(const ServerSocket* ssocket);
		~HTTPServerEvent();

		bool	IsEnd() const;
		void	RunAnyEvent(EventQueue* equeue);

	private:
		void	DeleteEvent(EventQueue* equeue);
		void	RegisterEvent(EventQueue* equeue);

		e_HTTPServerEventType	event_type_;
		const ServerSocket*		ssocket_;
		HTTPServer*				hserver_;
};

#endif
