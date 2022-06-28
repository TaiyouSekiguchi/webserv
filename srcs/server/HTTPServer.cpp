#include <string>
#include "HTTPServer.hpp"
#include "debug.hpp"

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

void	HTTPServer::Start()
{
	ListenSocket	lsocket;
	EventQueue		equeue;

	lsocket.ListenConnection();

	equeue.RegisterEvent(lsocket.GetFd());

	MainLoop(lsocket, equeue);
}

void	HTTPServer::MainLoop(ListenSocket const & lsocket, EventQueue const & equeue)
{
	while (1)
	{
		int		fd;

		fd = equeue.WaitEvent();
		if (fd == lsocket.GetFd())
		{
			std::cout << "Accept!!" << std::endl;
			equeue.RegisterEvent(lsocket.AcceptConnection());
		}
		else
			Communication(fd);
	}
}

void	HTTPServer::Communication(int event_fd) const
{
	const ssize_t	kSize = 1048576;     // 1MiB バイト
	char 			request_msg[kSize];
	int				ret;

	int recv_size = recv(event_fd, request_msg, kSize, 0);
	if (recv_size == -1)
		throw std::runtime_error("recv error");
	else if (recv_size == 0)
	{
		std::cout << "event_fd (" << event_fd << ") is disconnected." << std::endl;
		ret = close(event_fd);
		if (ret == -1)
			throw std::runtime_error("close error");
	}
	else
	{
		int send_size = send(event_fd, request_msg, recv_size, 0);
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
