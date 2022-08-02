#include "ServerFileEvent.hpp"

ServerFileEvent::ServerFileEvent(const RegularFile* rfile, const e_EventType type)
	: AServerIoEvent(type), rfile_(rfile)
{
}

ServerFileEvent::~ServerFileEvent()
{
	delete rfile_;
}

int		ServerFileEvent::GetFd() const { return (rfile_->GetFd()); }

e_EventStatus	ServerFileEvent::RunEvent(EventQueue* equeue)
{
	AServerIoEvent*	new_server_event;

	if (event_type_ == ET_READ)
		hserver_->ExecGETMethod(*rfile_);
	else if (event_type_ == ET_WRITE)
		hserver_->ExecPOSTMethod(*rfile_);
	else if (event_type_ == ET_DELETE)
		hserver_->ExecDELETEMethod(*rfile_);
	hserver_->RunCreateResponse();
	RegisterServerEvent(equeue, new_server_event);
	return (ES_END);
}
