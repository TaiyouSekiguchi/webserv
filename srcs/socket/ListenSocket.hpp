#ifndef LISTENSOCKET_HPP
# define LISTENSOCKET_HPP

# include "ASocket.hpp"

class ListenSocket : public ASocket
{
	public:
		ListenSocket(const ServerDirective::Listen& listen, const ServerDirective& server_conf);
		~ListenSocket();

		void	ListenConnection(const int backlog = 3) const;
		int		AcceptConnection() const;
};

#endif  // LISTENSOCKET_HPP
