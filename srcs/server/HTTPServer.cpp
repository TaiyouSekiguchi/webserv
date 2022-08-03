#include <string>
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "ClientClosed.hpp"
#include "HTTPError.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "utils.hpp"
#include "RegularFile.hpp"

HTTPServer::HTTPServer(const ServerSocket& ssocket)
	:	ssocket_(ssocket), request_(NULL), method_(NULL), response_(NULL)
{
}

HTTPServer::~HTTPServer()
{
	if (request_)
		delete request_;
	if (method_)
		delete method_;
	if (response_)
		delete response_;
}

const int	HTTPServer::GetMethodTargetFileFd() const { return (method_->GetTargetFileFd()); }
void		HTTPServer::DeleteMethodTargetFile() 	  { return (method_->DeleteTargetFile()); }

HTTPServerEvent::e_Type	HTTPServer::Run()
{
	HTTPServerEvent::e_Type	new_event;

	request_ = new HTTPRequest(ssocket_);
	method_ = new HTTPMethod(*request_);
	try
	{
		request_->ParseRequest();
		new_event = method_->ValidateHTTPMethod();
	}
	catch (const ClientClosed& e)
	{
		return (HTTPServerEvent::END);
	}
	catch (const HTTPError& e)
	{
		new_event = method_->ValidateErrorPage(e.GetStatusCode());
	}
	if (new_event != HTTPServerEvent::NOEVENT)
		return (new_event);
	return (RunCreateResponse());
}

HTTPServerEvent::e_Type	HTTPServer::RunHTTPMethod(const HTTPServerEvent::e_Type event_type)
{
	if (event_type == HTTPServerEvent::FILE_READ)
		method_->ExecGETMethod();
	else if (event_type == HTTPServerEvent::FILE_WRITE)
		method_->ExecPOSTMethod();
	else if (event_type == HTTPServerEvent::FILE_DELETE)
		method_->ExecDELETEMethod();
	return (RunCreateResponse());
}

HTTPServerEvent::e_Type	HTTPServer::RunCreateResponse()
{
	response_ = new HTTPResponse(*request_, *method_);
	return (HTTPServerEvent::SOCKET_SEND);
}

HTTPServerEvent::e_Type	HTTPServer::RunSendResponse()
{
	try
	{
		response_->SendResponse(ssocket_);
		if (response_->GetConnection() == false)
			return (HTTPServerEvent::END);
	}
	catch (const ClientClosed& e)
	{
		return (HTTPServerEvent::END);
	}
	return (HTTPServerEvent::SOCKET_RECV);
}
