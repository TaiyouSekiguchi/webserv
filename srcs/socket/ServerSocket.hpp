#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include <string>
# include "ASocket.hpp"


# define BUF_SIZE	1024

class ServerSocket : public ASocket
{
	public:
		explicit ServerSocket(const int fd);
		~ServerSocket();

		std::string		RecvRequest() const;
		std::string		RecvData() const;
		void			SendResponse(const std::string& response_msg) const;
};

#endif  // SERVERSOCKET_HPP
