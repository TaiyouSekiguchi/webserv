#include "MethodFileEvent.hpp"

MethodFileEvent::MethodFileEvent
	(const RegularFile* rfile, const e_EventType type, HTTPServer* hserver)
	: AIoEvent(type), rfile_(rfile), hserver_(hserver)
{
}

MethodFileEvent::~MethodFileEvent()
{
	delete rfile_;
}

int		MethodFileEvent::GetFd() const { return (rfile_->GetFd()); }

e_EventStatus	MethodFileEvent::RunEvent(EventQueue* equeue)
{
	if (event_type_ == ET_READ)
		return (RunReadEvent(equeue));
	else
		return (RunWriteEvent(equeue));
}

e_EventStatus	MethodFileEvent::RunReadEvent(EventQueue* equeue)
{
	AIoEvent*	new_io_event;
	std::string	body;
	int			ret;

	ret = rfile_->ReadFile(&body);
	if (ret == -1)
		hserver_->SetStatusCode(FORBIDDEN);
	else
	{
		hserver_->SetResponseBody(body);
		hserver_->SetStatusCode(OK);
	}
	new_io_event = hserver_->RunCreateResponse();
	equeue->SetIoEvent(new_io_event, new_io_event->GetEventType(), EA_ADD);
	return (ES_END);
}

e_EventStatus	MethodFileEvent::RunWriteEvent(EventQueue* equeue)
{
	AIoEvent*	new_io_event;
	int			ret;

	ret = rfile_->WriteToFile(hserver_->GetRequestBody());
	if (ret == -1)
		hserver_->SetStatusCode(FORBIDDEN);
	else
		hserver_->SetStatusCode(CREATED);
	new_io_event = hserver_->RunCreateResponse();
	equeue->SetIoEvent(new_io_event, new_io_event->GetEventType(), EA_ADD);
	return (ES_END);
}
