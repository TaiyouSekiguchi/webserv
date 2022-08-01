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
std::string	HTTPServer::GetRequestBody() const { return (request_->GetBody()); }
void		HTTPServer::SetStatusCode(const e_StatusCode sc) { method_->SetStatusCode(sc); }
void		HTTPServer::SetResponseBody(const std::string& body) { method_->SetBody(body); }

AIoEvent*	HTTPServer::Run()
{
	AIoEvent*		new_io_event;

	request_ = new HTTPRequest(ssocket_);
	method_ = new HTTPMethod(*request_);
	try
	{
		request_->ParseRequest();
		new_io_event = method_->ExecHTTPMethod();
		if (new_io_event)
			return (new_io_event);
	}
	catch (const ClientClosed& e)
	{
		connection_ = false;
		return (NULL);
	}
	catch (const HTTPError& e)
	{
		new_io_event = method_->SetErrorPage(e.GetStatusCode());
		if (new_io_event)
			return (new_io_event);
	}
	return (RunCreateResponse());
}

AIoEvent*	HTTPServer::RunCreateResponse()
{
	response_ = new HTTPResponse(*request_, *method_);
	return (ssocket_send_event_);
}

void	HTTPServer::RunSendResponse()
{
	try
	{
		response_->SendResponse(ssocket_);
		// connection_ = response_->GetConnection();
	}
	catch (const ClientClosed& e)
	{
		connection_ = false;
	}
}
