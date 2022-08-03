#include <string>
#include "WebServ.hpp"
#include "utils.hpp"
#include "AcceptClientEvent.hpp"
#include "HTTPServerEvent.hpp"

WebServ::WebServ()
{
}

WebServ::~WebServ()
{
}

void	WebServ::CreateListenSockets(const Config& config)
{
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	sitr = servers.begin();
	std::vector<ServerDirective>::const_iterator	send = servers.end();
	std::vector<ListenSocket*>::const_iterator		same_listen_lsocket;

	while (sitr != send)
	{
		const std::vector<ServerDirective::Listen>&				listens = sitr->GetListen();
		std::vector<ServerDirective::Listen>::const_iterator	litr = listens.begin();
		std::vector<ServerDirective::Listen>::const_iterator	lend = listens.end();
		while (litr != lend)
		{
			same_listen_lsocket = Utils::FindMatchMember(lsockets_, &ListenSocket::GetListen, *litr);
			if (same_listen_lsocket == lsockets_.end())
				lsockets_.push_back(new ListenSocket(*litr, *sitr));
			else
				(*same_listen_lsocket)->AddServerConf(*sitr);
			++litr;
		}
		++sitr;
	}
}

void	WebServ::RegisterAcceptClientEvent(EventQueue* equeue) const
{
	std::vector<ListenSocket*>::const_iterator	lsocket_itr = lsockets_.begin();
	std::vector<ListenSocket*>::const_iterator	end = lsockets_.end();

	while (lsocket_itr != end)
	{
		(*lsocket_itr)->ListenConnection();
		AcceptClientEvent*	new_event = new AcceptClientEvent(*lsocket_itr);
		equeue->SetIoEvent((*lsocket_itr)->GetFd(), ET_READ, EA_ADD, new_event);
		++lsocket_itr;
	}
}

void	WebServ::Start(const std::string& conf_path)
{
	try
	{
		Config		config(conf_path);
		EventQueue	equeue;

		CreateListenSockets(config);
		RegisterAcceptClientEvent(&equeue);
		EventLoop(&equeue);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void	WebServ::EventLoop(EventQueue* equeue) const
{
	AEvent			*event;
	AcceptClientEvent*	ac_event;
	HTTPServerEvent*	server_event;

	while (1)
	{
		event = equeue->WaitIoEvent();
		if (ac_event = dynamic_cast<AcceptClientEvent*>(event))
			ac_event->RunEvent(equeue);
		else if (server_event = dynamic_cast<HTTPServerEvent*>(event))
		{
			server_event->RunAnyEvent(equeue);
			if (server_event->IsEnd())
				delete server_event;
		}
	}
}
