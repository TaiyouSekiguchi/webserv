#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include <vector>
# include "ServerSocket.hpp"
# include "EventQueue.hpp"
# include "Config.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		void	Start(const Config& config);

	private:
		void	RegisterListenSockets(const Config& config, EventQueue* equeue);
		void	MainLoop(const EventQueue& equeue) const;
		void	Communication(const ServerSocket *ssocket) const;

		std::vector<ListenSocket*>	lsockets_;
};

#endif  // HTTPSERVER_HPP
