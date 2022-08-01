#include "ServerSocketEvent.hpp"
#include "ServerSocket.hpp"
#include "EventQueue.hpp"
#include "HTTPServer.hpp"

ServerSocketEvent::ServerSocketEvent(const ServerSocket* ssocket)
	: AIoEvent(ET_READ), ssocket_(ssocket), hserver_(NULL)
{
}

ServerSocketEvent::~ServerSocketEvent()
{
	if (hserver_)
		delete hserver_;
	delete ssocket_;
}

int		ServerSocketEvent::GetFd() const { return (ssocket_->GetFd()); }

e_EventStatus	ServerSocketEvent::RunEvent(EventQueue* equeue)
{
	if (event_type_ == ET_READ)
		return (RunReadEvent(equeue));
	else
		return (RunWriteEvent(equeue));
}

e_EventStatus	ServerSocketEvent::RunReadEvent(EventQueue* equeue)
{
	AIoEvent*	new_io_event;

	equeue->SetIoEvent(this, ET_READ, EA_DISABLE);
	event_type_ = ET_WRITE;
	hserver_ = new HTTPServer(this, *ssocket_);
	new_io_event = hserver_->Run();
	if (hserver_->GetConnection() == false)
		return (ES_END);
	equeue->SetIoEvent(new_io_event, new_io_event->GetEventType(), EA_ADD);
	return (ES_CONTINUE);
}

e_EventStatus	ServerSocketEvent::RunWriteEvent(EventQueue* equeue)
{
	hserver_->RunSendResponse();
	if (hserver_->GetConnection() == false)
		return (ES_END);
	delete hserver_;
	hserver_ = NULL;
	equeue->SetIoEvent(this, ET_WRITE, EA_DELETE);
	equeue->SetIoEvent(this, event_type_ = ET_READ, EA_ENABLE);
	return (ES_CONTINUE);
}
