#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include <string>

class ClientSocket
{
	public:
		ClientSocket();
		~ClientSocket();

		void		ConnectServer(const char *ip = "127.0.0.1", const int port = 8080) const;
		void		SendRequest(const std::string& request_msg) const;
		std::string	RecvResponse() const;

	private:
		int		fd_;
};

#endif
