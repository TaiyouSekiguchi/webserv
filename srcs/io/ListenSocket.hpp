#ifndef LISTENSOCKET_HPP
# define LISTENSOCKET_HPP

# include <sys/socket.h>
# include "ASocket.hpp"

class ListenSocket : public ASocket
{
	public:
		ListenSocket(const ServerDirective::Listen& listen, const ServerDirective& server_conf);
		~ListenSocket();

		void	ListenConnection(const int backlog = SOMAXCONN) const;
		int		AcceptConnection() const;
};

#endif  // LISTENSOCKET_HPP
