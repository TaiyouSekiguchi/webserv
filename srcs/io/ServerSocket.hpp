#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include <string>
# include "ListenSocket.hpp"

class ServerSocket : public ASocket
{
	public:
		explicit ServerSocket(const ListenSocket& lsocket);
		~ServerSocket();

		ssize_t	RecvData(std::string* data, const ssize_t size = 1048576) const;
		ssize_t	SendData(const std::string& data) const;
};

#endif  // SERVERSOCKET_HPP
