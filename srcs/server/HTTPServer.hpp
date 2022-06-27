#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

# include "ListenSocket.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		void	Start();

	private:
		int					kq_;
		struct timespec		waitspec_;

		void	CreateKqueue(void);
		void	RegisterKevent(int sock);
		void	KeventWaitLoop(ListenSocket const & lsocket);
		void	Communication(int event_sock) const;
		// void	Communication(const ListenSocket& lsocket) const;
};


#endif  // HTTPSERVER_HPP
