#include <sys/types.h>  // kqueue
#include <sys/event.h>  // kqueue
#include <sys/time.h>  // kqueue
#include <sys/socket.h>  // kqueue
#include <unistd.h>  // kqueue
#include <iostream>  // kqueue

#include <string>

#include "HTTPServer.hpp"
#include "debug.hpp"


HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

void	HTTPServer::Start() const
{
	ListenSocket lsocket;
	lsocket.ListenConnection();

	int				kq;
	struct kevent	kev;

	kq = kqueue();
	if (kq == -1)
		throw std::runtime_error("kqueue error");

	EV_SET(&kev, lsocket.GetFileDiscriptor(), EVFILT_READ, EV_ADD, 0, 0, NULL);

	int ret = kevent(kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1)
		throw std::runtime_error("kevent error");

	while (1)
	{
		int					n;
		struct timespec		waitspec;

		waitspec.tv_sec = 2;
		waitspec.tv_nsec = 500000;

		n = kevent(kq, NULL, 0, &kev, 1, &waitspec);
		if (n == -1)
			throw std::runtime_error("kevent error");
		else if (n > 0)
		{
			if ( kev.ident == (uintptr_t)lsocket.GetFileDiscriptor() )
			{
				int		connected_sock;

				connected_sock = accept(kev.ident, NULL, NULL);
				if (connected_sock == -1)
					throw std::runtime_error("accept error");
				else
				{
					std::cout << "fd : " << kev.ident << " Connected!!" << std::endl;

					EV_SET(&kev, connected_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);

					n = kevent(kq, &kev, 1, NULL, 0, NULL);
					if (n == -1)
					{
						throw std::runtime_error("kevent error");
					}
				}
			}
			else
			{
				int		event_sock = kev.ident;

				Communication(event_sock);
			}
		}
	}
}

void	HTTPServer::Communication(int event_sock) const
{
	const ssize_t	kSize = 1048576;     // 1MiB バイト
	char 			request_msg[kSize];
	int				ret;

	int recv_size = recv(event_sock, request_msg, kSize, 0);
	if (recv_size == -1)
		throw std::runtime_error("recv error");
	else if (recv_size == 0)
	{
		std::cout << "event_sock (" << event_sock << ") is disconnected." << std::endl;
		ret = close(event_sock);
		if (ret == -1)
			throw std::runtime_error("close error");
	}
	else
	{
		int send_size = send(event_sock, request_msg, recv_size, 0);
		if (send_size == -1)
			throw std::runtime_error("send error");
	}
}

/*
void	HTTPServer::Communication(const ListenSocket& lsocket) const
{
	std::string		recv_msg;
	ServerSocket	ssocket(lsocket.AcceptConnection());

	MyPrint("Connected");
	while (1)
	{
		recv_msg = ssocket.RecvRequest();
		MyPrint("[recv_msg]\n" + recv_msg);
		// ParseRequest();
		// CreateResponse();
		ssocket.SendResponse(recv_msg);
	}
}
*/
