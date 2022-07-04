#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include <string>
# include "ASocket.hpp"

class ServerSocket : public ASocket
{
	public:
		explicit ServerSocket(const int fd);
		~ServerSocket();

		std::string	RecvMsg(const ssize_t size = 1048576) const;
		void		SendMsg(const std::string& msg) const;
};

#endif  // SERVERSOCKET_HPP
