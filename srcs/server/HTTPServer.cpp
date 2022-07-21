#include <string>
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "ListenSocket.hpp"
#include "debug.hpp"
#include "Config.hpp"
#include "ClientClosed.hpp"
#include "HTTPError.hpp"
#include "HTTPMethod.hpp"
#include "utils.hpp"

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
}

void	HTTPServer::RegisterListenSockets(const Config& config, EventQueue* equeue)
{
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	sitr = servers.begin();
	std::vector<ServerDirective>::const_iterator	send = servers.end();
	std::vector<ListenSocket*>::const_iterator		same_listen_lsocket;
	ListenSocket*									new_lsocket;

	while (sitr != send)
	{
		const std::vector<std::pair<unsigned int, int> >&			listens = sitr->GetListen();
		std::vector<std::pair<unsigned int, int> >::const_iterator	litr = listens.begin();
		std::vector<std::pair<unsigned int, int> >::const_iterator	lend = listens.end();
		while (litr != lend)
		{
			same_listen_lsocket = Utils::FindMatchMember(lsockets_, &ListenSocket::GetListen, *litr);
			if (same_listen_lsocket == lsockets_.end())
			{
				new_lsocket = new ListenSocket(*litr, *sitr);
				lsockets_.push_back(new_lsocket);
				new_lsocket->ListenConnection();
				equeue->RegisterEvent(new_lsocket->GetFd(), new_lsocket);
			}
			else
				(*same_listen_lsocket)->AddServerConf(*sitr);
			++litr;
		}
		++sitr;
	}
}

void	HTTPServer::Start(const Config& config)
{
	EventQueue	equeue;

	RegisterListenSockets(config, &equeue);
	MainLoop(equeue);
}

void	HTTPServer::MainLoop(const EventQueue& equeue) const
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
			new_ssocket = new ServerSocket(*lsocket);
			equeue.RegisterEvent(new_ssocket->GetFd(), new_ssocket);
		}
		else
			Communication(ssocket);
	}
}

void	HTTPServer::Communication(const ServerSocket *ssocket) const
{
	int						status_code;
	HTTPRequest				req;
	HTTPMethod				method;
	const ServerDirective&	server_conf = *(ssocket->GetServerConfs()[0]);

	try
	{
		// req.ParseRequest(*ssocket);
		req.ParseRequest(*ssocket, server_conf);
		// status_code = method.ExecHTTPMethod(req);
		status_code = method.ExecHTTPMethod(req, server_conf);
	}
	catch (const ClientClosed& e)
	{
		delete ssocket;
		return;
	}
	catch (const HTTPError& e)
	{
		status_code = e.GetStatusCode();
	}
	req.RequestDisplay();
	std::cout << "status_code: " << status_code << std::endl;
	method.MethodDisplay();
	// HTTPResponse	res(status_code, req, method);
	// res.SendResponse(ssocket);
}
