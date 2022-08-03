#ifndef HTTPSERVEREVENT_HPP
# define HTTPSERVEREVENT_HPP

# include "AEvent.hpp"
# include "ServerSocket.hpp"
# include "HTTPServer.hpp"

class HTTPServerEvent : public AEvent
{
	public:
		enum e_Type
		{
			NOEVENT			= -1,
			SOCKET_RECV		= 0,
			SOCKET_SEND		= 1,
			FILE_READ		= 2,
			FILE_WRITE		= 3,
			FILE_DELETE 	= 4,
			END				= 5
		};

		explicit HTTPServerEvent(const ServerSocket* ssocket);
		~HTTPServerEvent();

		bool	IsEnd() const;
		void	RunAnyEvent(EventQueue* equeue);

	private:
		void	DeleteEvent(EventQueue* equeue);
		void	RegisterEvent(EventQueue* equeue);

		e_Type				type_;
		const ServerSocket*	ssocket_;
		HTTPServer*			hserver_;
};

#endif
