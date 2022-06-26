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
	while (1)
	{
		Communication(lsocket);
	}
}

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
