#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

class ServerSocket
{
	public:
		explicit ServerSocket(const int fd);
		~ServerSocket();

		std::string RecvRequest() const;
		void		SendResponse(const std::string& response_msg) const;

	private:
		int		fd_;
};

#endif  // SERVERSOCKET_HPP
