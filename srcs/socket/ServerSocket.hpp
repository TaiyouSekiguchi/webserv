#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include <string>
# include "ASocket.hpp"

# define BUF_SIZE	1024

class ServerSocket : public ASocket
{
	public:
		enum	e_status
		{
			CONNECT,
			DISCONNECT,
		};

		explicit ServerSocket(const int fd);
		~ServerSocket();

		std::string	RecvData(const ssize_t size = 1048576) const;
		void		SendData(const std::string& msg) const;
};

#endif  // SERVERSOCKET_HPP
