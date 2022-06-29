#ifndef LISTENSOCKET_HPP
# define LISTENSOCKET_HPP

# include "ASocket.hpp"

class ListenSocket : public ASocket
{
	public:
		explicit ListenSocket(const int port = 8080);
		~ListenSocket();

		void	ListenConnection(const int backlog = 3) const;
		int		AcceptConnection() const;
};

#endif  // LISTENSOCKET_HPP
