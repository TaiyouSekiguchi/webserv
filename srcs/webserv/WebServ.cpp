#include <string>
#include "WebServ.hpp"
#include "ListenSocketEvent.hpp"
#include "utils.hpp"

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
		equeue->SetIoEvent(new ListenSocketEvent(*lsocket_itr), ET_READ, EA_ADD);
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
	AIoEvent			*io_event;
	e_EventStatus	event_status;

	while (1)
	{
		io_event = equeue->WaitIoEvent();
		event_status = io_event->RunEvent(equeue);
		if (event_status == ES_CONTINUE)
			continue;
		else if (event_status == ES_END)
			delete io_event;
	}
}
