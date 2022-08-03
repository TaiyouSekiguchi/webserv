#include "HTTPServerEvent.hpp"
#include "ServerSocket.hpp"
#include "EventQueue.hpp"
#include "HTTPServer.hpp"

HTTPServerEvent::HTTPServerEvent(const ServerSocket* ssocket)
	: AEvent(), type_(SOCKET_RECV), ssocket_(ssocket), hserver_(NULL)
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
	return (type_ == END);
}

void	HTTPServerEvent::RunAnyEvent(EventQueue* equeue)
{
	e_Type	next_type;

	switch (type_)
	{
		case SOCKET_RECV:
			hserver_ = new HTTPServer(*ssocket_);
			next_type = hserver_->Run();
			break;
		case FILE_READ:
		case FILE_WRITE:
		case FILE_DELETE:
			next_type = hserver_->RunHTTPMethod(type_);
			break;
		case SOCKET_SEND:
			next_type = hserver_->RunSendResponse();
			delete hserver_;
			hserver_ = NULL;
			break;
	}
	DeleteEvent(equeue);
	type_ = next_type;
	RegisterEvent(equeue);
}

void	HTTPServerEvent::DeleteEvent(EventQueue* equeue)
{
	switch (type_)
	{
		case SOCKET_RECV:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_READ, EA_DISABLE, this);
			break;
		case SOCKET_SEND:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_WRITE, EA_DELETE, this);
			break;
		case FILE_READ:
		case FILE_WRITE:
		case FILE_DELETE:
			hserver_->DeleteMethodTargetFile();
			break;
	}
}

void	HTTPServerEvent::RegisterEvent(EventQueue* equeue)
{
	switch (type_)
	{
		case SOCKET_RECV:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_READ, EA_ENABLE, this);
			break;
		case SOCKET_SEND:
			equeue->SetIoEvent(ssocket_->GetFd(), ET_WRITE, EA_ADD, this);
			break;
		case FILE_READ:
			equeue->SetIoEvent(hserver_->GetMethodTargetFileFd(), ET_READ, EA_ADD, this);
			break;
		case FILE_WRITE:
		case FILE_DELETE:
			equeue->SetIoEvent(hserver_->GetMethodTargetFileFd(), ET_WRITE, EA_ADD, this);
			break;
	}
}
