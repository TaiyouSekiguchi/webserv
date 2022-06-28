#ifndef LISTENSOCKET_HPP
# define LISTENSOCKET_HPP

# include "ServerSocket.hpp"

class ListenSocket
{
	public:
		explicit ListenSocket(const int port = 8080);
		~ListenSocket();

		void	ListenConnection(const int backlog = 3) const;
		int		AcceptConnection() const;
		int		GetFd() const;

	private:
		int		fd_;
};

#endif  // LISTENSOCKET_HPP
