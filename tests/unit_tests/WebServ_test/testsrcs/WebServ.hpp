#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <vector>
# include "ListenSocket.hpp"
# include "Config.hpp"

class WebServ
{
	public:
		WebServ();
		~WebServ();

		void	CreateListenSockets(const Config& config);
		const std::vector<ListenSocket*>&	GetLsockets() const { return (lsockets_); };

	private:
		std::vector<ListenSocket*>	lsockets_;
};

#endif  // WEBSERV_HPP
