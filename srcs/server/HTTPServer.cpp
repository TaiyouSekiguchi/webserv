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
		if (new_event != SEVENT_NO)
			return (new_event);
	}
	catch (const ClientClosed& e)
	{
		return (SEVENT_END);
	}
	catch (const HTTPError& e)
	{
		e.PutMsg();
		new_event = method_->ValidateErrorPage(e.GetStatusCode());
		if (new_event != SEVENT_NO)
			return (new_event);
	}
	return (RunCreateResponse());
}

e_HTTPServerEventType	HTTPServer::RunExecHTTPMethod(const e_HTTPServerEventType event_type)
{
	e_HTTPServerEventType	new_event;

	try
	{
		if (event_type == SEVENT_FILE_READ)
			method_->ExecGETMethod();
		else if (event_type == SEVENT_FILE_WRITE)
			method_->ExecPOSTMethod();
		else if (event_type == SEVENT_FILE_DELETE)
			method_->ExecDELETEMethod();
	}
	catch (const HTTPError& e)
	{
		e.PutMsg();
		new_event = method_->ValidateErrorPage(e.GetStatusCode());
		if (new_event != SEVENT_NO)
			return (new_event);
	}
	return (RunCreateResponse());
}

e_HTTPServerEventType	HTTPServer::RunReadErrorPage()
{
	method_->ReadErrorPage();
	return (RunCreateResponse());
}

e_HTTPServerEventType	HTTPServer::RunCreateResponse()
{
	request_->RequestDisplay();
	method_->MethodDisplay();
	response_ = new HTTPResponse(*request_, *method_);
	std::cout << response_->GetResMsg() << std::endl;
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
