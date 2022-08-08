#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include "ListenSocket.hpp"
# include "EventQueue.hpp"
# include "Config.hpp"

class WebServ
{
	public:
		WebServ();
		~WebServ();

		void	Start(const std::string& conf_path);

	private:
		void	CreateListenSockets(const Config& config);
		void	RegisterAcceptClientEvent(EventQueue* equeue) const;
		void	EventLoop(EventQueue* equeue) const;

		std::vector<ListenSocket*>	lsockets_;
};

#endif  // WEBSERV_HPP
