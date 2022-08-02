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
#include "ServerSocketEvent.hpp"
#include "RegularFile.hpp"

HTTPServer::HTTPServer(ServerSocketEvent* ssocket_event, const ServerSocket& ssocket)
	:	ssocket_send_event_(ssocket_event), ssocket_(ssocket),
		request_(NULL), method_(NULL), response_(NULL)
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

bool		HTTPServer::GetConnection() const { return (connection_); }

AServerIoEvent*	HTTPServer::Run()
{
	AServerIoEvent*	new_server_event;

	request_ = new HTTPRequest(ssocket_);
	method_ = new HTTPMethod(*request_);
	try
	{
		request_->ParseRequest();
		new_server_event = method_->ValidateHTTPMethod();
	}
	catch (const ClientClosed& e)
	{
		connection_ = false;
		return (NULL);
	}
	catch (const HTTPError& e)
	{
		new_server_event = method_->ValidateErrorPage(e.GetStatusCode());
	}

	if (new_server_event)
		return (new_server_event);
	return (RunCreateResponse());
}

AServerIoEvent*	HTTPServer::RunCreateResponse()
{
	response_ = new HTTPResponse(*request_, *method_);
	return (ssocket_send_event_);
}

void	HTTPServer::RunSendResponse()
{
	try
	{
		response_->SendResponse(ssocket_);
		connection_ = response_->GetConnection();
	}
	catch (const ClientClosed& e)
	{
		connection_ = false;
	}
}

void	HTTPServer::ExecGETMethod(const RegularFile& rfile)
{
	return (method_->ExecGETMethod(rfile));
}

void	HTTPServer::ExecPOSTMethod(const RegularFile& rfile)
{
	return (method_->ExecPOSTMethod(rfile));
}

void	HTTPServer::ExecDELETEMethod(const RegularFile& rfile)
{
	return (method_->ExecDELETEMethod(rfile));
}
