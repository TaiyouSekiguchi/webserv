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

		std::string		RecvRequest() const;
		std::string		RecvData() const;
		void			SendResponse(const std::string& response_msg) const;
		void			DisconnectSocket(void);
		e_status		GetSocketStatus(void) const;

	private:
		e_status		socket_status_;
};

#endif  // SERVERSOCKET_HPP
