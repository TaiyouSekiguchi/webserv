#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include "ServerSocket.hpp"
# include "EventQueue.hpp"
# include "HTTPRequest.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		void	Start() const;

	private:
		void	MainLoop(EventQueue const & equeue) const;
		void	Communication(ServerSocket *ssocket) const;
};

#endif  // HTTPSERVER_HPP
