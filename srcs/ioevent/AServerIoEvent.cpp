#include "AServerIoEvent.hpp"

AServerIoEvent::AServerIoEvent(const e_EventType type)
	: AIoEvent(type), hserver_(NULL)
{
}

AServerIoEvent::~AServerIoEvent()
{
}

void	AServerIoEvent::SetServerInfo(HTTPServer* hserver) { hserver_ = hserver; }

void	AServerIoEvent::RegisterServerEvent(EventQueue* equeue, AServerIoEvent *new_server_event)
{
	new_server_event->SetServerInfo(hserver_);
	equeue->SetIoEvent(new_server_event, new_server_event->GetEventType(), EA_ADD);
}
