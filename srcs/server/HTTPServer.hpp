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
		void	MainLoop(ListenSocket const & lsocket, EventQueue const & equeue);
		void	Communication(int event_fd) const;
		// void	Communication(const ListenSocket& lsocket) const;
};


#endif  // HTTPSERVER_HPP
