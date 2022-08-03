#include "AcceptClientEvent.hpp"
#include "HTTPServerEvent.hpp"

AcceptClientEvent::AcceptClientEvent(const ListenSocket* lsocket)
	: AEvent(), lsocket_(lsocket)
{
}

AcceptClientEvent::~AcceptClientEvent()
{
	delete lsocket_;
}

void	AcceptClientEvent::RunEvent(EventQueue* equeue)
{
	ServerSocket*		new_ssocket = new ServerSocket(*lsocket_);
	HTTPServerEvent*	new_event = new HTTPServerEvent(new_ssocket);
	equeue->SetIoEvent(new_ssocket->GetFd(), ET_READ, EA_ADD, new_event);
}
