#include "HTTPServerEvent.hpp"

HTTPServerEvent::HTTPServerEvent(const ServerSocket* ssocket)
	: AEvent(), event_type_(SEVENT_SOCKET_RECV), ssocket_(ssocket), hserver_(NULL)
{
}

HTTPServerEvent::~HTTPServerEvent()
{
	if (hserver_)
		delete hserver_;
	delete ssocket_;
}

bool	HTTPServerEvent::IsEnd() const
{
	return (event_type_ == SEVENT_END);
}

void	HTTPServerEvent::RunAnyEvent(EventQueue* equeue)
{
	e_HTTPServerEventType	next_event_type;

	switch (event_type_)
	{
		case SEVENT_SOCKET_RECV:
			hserver_ = new HTTPServer(*ssocket_);
			next_event_type = hserver_->Run();
			break;
		case SEVENT_FILE_READ:
		case SEVENT_FILE_WRITE:
		case SEVENT_FILE_DELETE:
			next_event_type = hserver_->RunExecHTTPMethod(event_type_);
			break;
		case SEVENT_ERRORPAGE_READ:
			next_event_type = hserver_->RunReadErrorPage();
			break;
		case SEVENT_SOCKET_SEND:
			next_event_type = hserver_->RunSendResponse();
			delete hserver_;
			hserver_ = NULL;
			break;
		default:
			throw std::runtime_error("RunAnyEvent error");
	}
	DeleteEvent(equeue);
	event_type_ = next_event_type;
	RegisterEvent(equeue);
}

void	HTTPServerEvent::DeleteEvent(EventQueue* equeue)
{
	switch (event_type_)
	{
		case SEVENT_SOCKET_RECV:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_READ, EA_DISABLE, this);
			break;
		case SEVENT_SOCKET_SEND:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_WRITE, EA_DELETE, this);
			break;
		case SEVENT_FILE_READ:
		case SEVENT_FILE_WRITE:
		case SEVENT_FILE_DELETE:
		case SEVENT_ERRORPAGE_READ:
			hserver_->DeleteMethodTargetFile();
			break;
		default: {}
	}
}

void	HTTPServerEvent::RegisterEvent(EventQueue* equeue)
{
	switch (event_type_)
	{
		case SEVENT_SOCKET_RECV:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_READ, EA_ENABLE, this);
			break;
		case SEVENT_SOCKET_SEND:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_WRITE, EA_ADD, this);
			break;
		case SEVENT_FILE_READ:
		case SEVENT_ERRORPAGE_READ:
			equeue->SetIoEvent(hserver_->GetMethodTargetFileFd(), ET_READ, EA_ADD, this);
			break;
		case SEVENT_FILE_WRITE:
		case SEVENT_FILE_DELETE:
			equeue->SetIoEvent(hserver_->GetMethodTargetFileFd(), ET_WRITE, EA_ADD, this);
			break;
		default: {}
	}
}
