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

HTTPServer::HTTPServer()
	: request_(NULL), method_(NULL), response_(NULL), connection_(true)
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

bool			HTTPServer::GetConnection() const { return (connection_); }
std::string		HTTPServer::GetResponseMsg() const { return (response_msg_); }

AIoEvent	*HTTPServer::RunRequestStep(const ServerSocket& ssocket)
{
	int		status_code = 0;
	request_ = new HTTPRequest(ssocket);
	method_ = new HTTPMethod();

	try
	{
		request_->ParseRequest();
		request_->RequestDisplay();
		status_code = method_->ExecHTTPMethod(*request_);
	}
	catch (const ClientClosed& e)
	{
		connection_ = false;
		return (NULL);
	}
	catch (const HTTPError& e)
	{
		status_code = e.GetStatusCode();
		e.PutMsg();
	}
	std::cout << "status_code: " << status_code << std::endl;
	method_->MethodDisplay();
	response_ = new HTTPResponse(status_code, *request_, *method_);
	response_msg_ = response_->GetResMsg();
	// connection_ = response_->GetConnection();
	return (NULL);
}
