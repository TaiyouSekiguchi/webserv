#include <string>
#include "HTTPServer.hpp"
#include "HTTPRequest.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "HTTPError.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "utils.hpp"
#include "RegularFile.hpp"

HTTPServer::HTTPServer(const ServerSocket& ssocket)
	:	ssocket_(ssocket), request_(NULL), method_(NULL), response_(NULL)
{
	request_ = new HTTPRequest(ssocket_);
	method_ = new HTTPMethod(*request_);
}

HTTPServer::~HTTPServer()
{
	delete request_;
	delete method_;
	if (response_)
		delete response_;
}

int		HTTPServer::GetMethodTargetFileFd() const { return (method_->GetTargetFileFd()); }
int		HTTPServer::GetToCgiPipeFd() const	  { return (method_->GetToCgiPipeFd()); }
int		HTTPServer::GetFromCgiPipeFd() const 	  { return (method_->GetFromCgiPipeFd()); }

e_HTTPServerEventType	HTTPServer::Run()
{
	e_HTTPServerEventType	new_event;

	try
	{
		new_event = request_->ParseRequest();
		if (new_event != SEVENT_NO)
			return (new_event);
		new_event = method_->ValidateHTTPMethod();
		if (new_event != SEVENT_NO)
			return (new_event);
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

e_HTTPServerEventType	HTTPServer::RunPostToCgi()
{
	e_HTTPServerEventType	new_event;

	try
	{
		method_->PostToCgi();
		return (SEVENT_CGI_READ);
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

e_HTTPServerEventType	HTTPServer::RunReceiveCgiResult(const bool eof_flag)
{
	e_HTTPServerEventType	new_event;

	try
	{
		new_event = method_->ReceiveCgiResult(eof_flag);
		if (new_event != SEVENT_NO)
			return (new_event);
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
	e_HTTPServerEventType	new_event;

	new_event = response_->SendResponse(ssocket_);
	if (new_event != SEVENT_NO)
		return (SEVENT_END);
	else if (response_->GetConnection() == false)
		return (SEVENT_END);
	return (SEVENT_SOCKET_RECV);
}
