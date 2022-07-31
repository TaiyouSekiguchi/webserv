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

	hserver_ = new HTTPServer();
	new_io_event = hserver_->RunRequestStep(*ssocket_);
	if (new_io_event)
		equeue->SetIoEvent(new_io_event, new_io_event->GetEventType(), EA_ADD);
	else
	{
		if (hserver_->GetConnection() == false)
			return (ES_END);
		equeue->SetIoEvent(this, event_type_ = ET_WRITE, EA_ADD);
	}
	equeue->SetIoEvent(this, ET_READ, EA_DISABLE);
	return (ES_CONTINUE);
}

e_EventStatus	ServerSocketEvent::RunWriteEvent(EventQueue* equeue)
{
	ssocket_->SendData(hserver_->GetResponseMsg());
	if (hserver_->GetConnection() == false)
		return (ES_END);
	delete hserver_;
	hserver_ = NULL;
	equeue->SetIoEvent(this, ET_WRITE, EA_DELETE);
	equeue->SetIoEvent(this, event_type_ = ET_READ, EA_ENABLE);
	return (ES_CONTINUE);
}
