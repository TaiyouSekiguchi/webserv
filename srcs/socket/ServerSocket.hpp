#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include <string>
# include "ASocket.hpp"

class ServerSocket : public ASocket
{
	public:
		explicit ServerSocket(const int fd);
		~ServerSocket();

		std::string RecvRequest() const;
		void		SendResponse(const std::string& response_msg) const;
};

#endif  // SERVERSOCKET_HPP
