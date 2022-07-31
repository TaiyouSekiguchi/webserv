#include "ListenSocketEvent.hpp"
#include "ServerSocketEvent.hpp"
#include "EventQueue.hpp"

ListenSocketEvent::ListenSocketEvent(const ListenSocket* lsocket)
	: AIoEvent(ET_READ), lsocket_(lsocket)
{
}

ListenSocketEvent::~ListenSocketEvent()
{
	delete lsocket_;
}

int		ListenSocketEvent::GetFd() const { return (lsocket_->GetFd()); }

e_EventStatus	ListenSocketEvent::RunEvent(EventQueue* equeue)
{
	ServerSocket*	new_ssocket = new ServerSocket(*lsocket_);
	equeue->SetIoEvent(new ServerSocketEvent(new_ssocket), ET_READ, EA_ADD);
	return (ES_CONTINUE);
}
