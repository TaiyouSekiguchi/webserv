#include <string>
#include "HTTPServer.hpp"
#include "ListenSocket.hpp"
#include "debug.hpp"
#include "Config.hpp"

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

void	HTTPServer::Start(const Config& config) const
{
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	itr = servers.begin();
	std::vector<ServerDirective>::const_iterator	end = servers.end();
	ListenSocket	*lsocket;
	EventQueue		equeue;

	while (itr != end)
	{
		lsocket = new ListenSocket(*itr);
		lsocket->ListenConnection();
		equeue.RegisterEvent(lsocket->GetFd(), lsocket);
		++itr;
	}
	MainLoop(equeue);
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
			new_ssocket = new ServerSocket(lsocket->AcceptConnection(), lsocket->GetServerConf());
			equeue.RegisterEvent(new_ssocket->GetFd(), new_ssocket);
		}
		else
			Communication(ssocket);
	}
}

void	HTTPServer::Communication(ServerSocket *ssocket) const
{
	(void)ssocket;
	// int				status_code;
	// HTTPRequest		req;
	// HTTPMethod		method;
	// const ServerDirective&	server_conf = ssocket->GetServerConf();

	// try
	// {
	// 	req.RecvRequest(ssocket, server_conf);
	// 	status_code = method.ExecHTTPMethod(req, server_conf);
	// }
	// catch (const ClientClosed& e)
	// {
	// 	delete ssocket;
	// 	return;
	// }
	// catch (const HTTPError& e)
	// {
	// 	status_code = e.GetStatusCode();
	// }
	// HTTPResponse	res(status_code, req, method, server_conf);
	// res.SendResponse(ssocket);
}
