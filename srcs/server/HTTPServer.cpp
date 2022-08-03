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

int		HTTPServer::GetMethodTargetFileFd() const { return (method_->GetTargetFileFd()); }
void	HTTPServer::DeleteMethodTargetFile() 	  { return (method_->DeleteTargetFile()); }

e_HTTPServerEventType	HTTPServer::Run()
{
	e_HTTPServerEventType	new_event;

	request_ = new HTTPRequest(ssocket_);
	method_ = new HTTPMethod(*request_);
	try
	{
		request_->ParseRequest();
		new_event = method_->ValidateHTTPMethod();
	}
	catch (const ClientClosed& e)
	{
		return (SEVENT_END);
	}
	catch (const HTTPError& e)
	{
		new_event = method_->ValidateErrorPage(e.GetStatusCode());
	}
	if (new_event != SEVENT_NO)
		return (new_event);
	return (RunCreateResponse());
}

e_HTTPServerEventType	HTTPServer::RunHTTPMethod(const e_HTTPServerEventType event_type)
{
	if (event_type == SEVENT_FILE_READ)
		method_->ExecGETMethod();
	else if (event_type == SEVENT_FILE_WRITE)
		method_->ExecPOSTMethod();
	else if (event_type == SEVENT_FILE_DELETE)
		method_->ExecDELETEMethod();
	return (RunCreateResponse());
}

e_HTTPServerEventType	HTTPServer::RunCreateResponse()
{
	response_ = new HTTPResponse(*request_, *method_);
	return (SEVENT_SOCKET_SEND);
}

e_HTTPServerEventType	HTTPServer::RunSendResponse()
{
	try
	{
		response_->SendResponse(ssocket_);
		if (response_->GetConnection() == false)
			return (SEVENT_END);
	}
	catch (const ClientClosed& e)
	{
		return (SEVENT_END);
	}
	return (SEVENT_SOCKET_RECV);
}
