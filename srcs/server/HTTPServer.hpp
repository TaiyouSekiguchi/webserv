#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

# include "ListenSocket.hpp"
# include "EventQueue.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		void	Start();

	private:
		void	MainLoop(ListenSocket const & lsocket, EventQueue * equeue);
		void	Communication(int event_sock) const;
		// void	Communication(const ListenSocket& lsocket) const;
};


#endif  // HTTPSERVER_HPP
