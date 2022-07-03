#include <string>
#include "HTTPServer.hpp"
#include "ListenSocket.hpp"
#include "debug.hpp"

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

void	HTTPServer::Start() const
{
	ListenSocket	*lsocket = new ListenSocket();
	EventQueue		equeue;

	lsocket->ListenConnection();
	equeue.RegisterEvent(lsocket->GetFd(), lsocket);
	MainLoop(equeue);
	delete lsocket;
}

void	HTTPServer::MainLoop(EventQueue const & equeue) const
{
	void			*udata;
	ASocket			*asocket;
	ListenSocket	*lsocket;
	ServerSocket	*ssocket;
	ServerSocket 	*new_ssocket;

	while (1)
	{
		udata = equeue.WaitEvent();
		asocket = static_cast<ASocket*>(udata);
		lsocket = dynamic_cast<ListenSocket*>(asocket);
		ssocket = dynamic_cast<ServerSocket*>(asocket);
		if (lsocket)
		{
			std::cout << "Accept!!" << std::endl;
			new_ssocket = new ServerSocket(lsocket->AcceptConnection());
			equeue.RegisterEvent(new_ssocket->GetFd(), new_ssocket);
		}
		else
			Communication(ssocket);
	}
}

void	HTTPServer::Communication(ServerSocket *ssocket) const
{
	HTTPRequest		req;
	// HTTPResponse		res;
	// std::string		send_msg;

	req.ParseRequest(*ssocket);
	req.RequestDisplay();
	// send_msg = res.CreateResponse(req);
	// ssocket->SendResponse(send_msg);
	//ssocket->SendResponse(recv_msg);
	
	return;

	/*
	recv_msg = ssocket->RecvRequest();
	if (recv_msg.size() == 0)
		delete ssocket;
	else
	{
		std::cout << "[recv_msg]\n" << recv_msg << std::endl;
		ssocket->GetHTTPRequest()->ParseRequest();
		if ( ssocket->GetHTTPRequest()->GetRequestStatus() != 0 )
		{
			ssocket->GetHTTPRequest()->RequestDisplay();
			// send_msg = res.CreateResponse(req);
			// ssocket->SendResponse(send_msg);
			//ssocket->SendResponse(recv_msg);
		}
	}
	*/
}
