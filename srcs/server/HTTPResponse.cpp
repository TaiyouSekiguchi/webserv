#include <sstream>
#include <fstream>
#include <ctime>
#include "HTTPResponse.hpp"
#include "utils.hpp"

HTTPResponse::HTTPResponse(const HTTPRequest &req, const HTTPMethod &method)
	: req_(req), method_(method),
	  server_conf_(req.GetServerConf()), status_code_(method.GetStatusCode())
{
	CheckConnection();
	AppendHeaders();
	res_msg_ = CreateResponse();
}

HTTPResponse::~HTTPResponse()
{
}

const std::string&	HTTPResponse::GetResMsg() const { return (res_msg_); }
const bool&			HTTPResponse::GetConnection() const { return (connection_); }

void HTTPResponse::SendResponse(const ServerSocket& ssocket)
{
	ssocket.SendData(res_msg_);
}

void HTTPResponse::CheckConnection()
{
	if (status_code_ == BAD_REQUEST || status_code_ == HTTP_VERSION_NOT_SUPPORTED)
	{
		connection_ = false;
		return;
	}
	connection_ = req_.GetConnection();
}

void HTTPResponse::AppendHeaders()
{
	AppendHeader("Server", "Webserv");
	AppendHeader("Date", GetDate());
	AppendHeader("Connection", connection_ ? "keep-alive" : "close");
	AppendHeader("Content-type", method_.GetContentType());
	AppendHeader("Location", method_.GetLocation());
	AppendHeader("Content-Length",
		(req_.GetMethod() == "GET" || !IsNormalStatus()) ? Utils::ToString(method_.GetBody().size()) : "");
}

void HTTPResponse::AppendHeader(const std::string &key, const std::string &value)
{
	if (headers_.find(key) != headers_.end())
	{
		return;
	}
	if (value.empty())
	{
		return;
	}
	headers_[key] = value + "\r\n";
}

std::string HTTPResponse::GetDate() const
{
	time_t now = time(NULL);
	struct tm current_time;
	char str[50];

	asctime_r(gmtime_r(&now, &current_time), str);
	strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", &current_time);
	return (str);
}

bool HTTPResponse::IsNormalStatus() const
{
	return (status_code_ < 300);
}

std::string HTTPResponse::CreateResponse()
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << status_code_ << " " << kStatusMsg_[status_code_] << "\r\n";
	ss << HeaderField();
	ss << method_.GetBody();
	return (ss.str());
}

std::string	HTTPResponse::HeaderField() const
{
	std::stringstream ss;
	std::map<std::string, std::string>::const_iterator ite = headers_.begin();

	for (; ite != headers_.end(); ite++)
	{
		ss << ite->first << ": " << ite->second;
	}
	ss << "\r\n";
	return (ss.str());
}

const std::pair<e_StatusCode, std::string> HTTPResponse::kPairs_[] = {
	std::make_pair(static_cast<e_StatusCode>(100), "Continue"),
	std::make_pair(static_cast<e_StatusCode>(101), "Switching Protocols"),
	std::make_pair(static_cast<e_StatusCode>(102), "Processing"),
	std::make_pair(static_cast<e_StatusCode>(103), "Early Hints"),
	std::make_pair(static_cast<e_StatusCode>(200), "OK"),
	std::make_pair(static_cast<e_StatusCode>(201), "Created"),
	std::make_pair(static_cast<e_StatusCode>(202), "Accepted"),
	std::make_pair(static_cast<e_StatusCode>(203), "Non-Authoritative Information"),
	std::make_pair(static_cast<e_StatusCode>(204), "No Content"),
	std::make_pair(static_cast<e_StatusCode>(205), "Reset Content"),
	std::make_pair(static_cast<e_StatusCode>(206), "Partial Content"),
	std::make_pair(static_cast<e_StatusCode>(207), "Multi-Status"),
	std::make_pair(static_cast<e_StatusCode>(208), "Already Reported"),
	std::make_pair(static_cast<e_StatusCode>(226), "IM Used"),
	std::make_pair(static_cast<e_StatusCode>(300), "Multiple Choice"),
	std::make_pair(static_cast<e_StatusCode>(301), "Moved Permanently"),
	std::make_pair(static_cast<e_StatusCode>(302), "Found"),
	std::make_pair(static_cast<e_StatusCode>(303), "See Other"),
	std::make_pair(static_cast<e_StatusCode>(304), "Not Modified"),
	std::make_pair(static_cast<e_StatusCode>(307), "Temporary Redirect"),
	std::make_pair(static_cast<e_StatusCode>(308), "Permanent Redirect"),
	std::make_pair(static_cast<e_StatusCode>(400), "Bad Request"),
	std::make_pair(static_cast<e_StatusCode>(401), "Unauthorized"),
	std::make_pair(static_cast<e_StatusCode>(402), "Payment Required"),
	std::make_pair(static_cast<e_StatusCode>(403), "Forbidden"),
	std::make_pair(static_cast<e_StatusCode>(404), "Not Found"),
	std::make_pair(static_cast<e_StatusCode>(405), "Method Not Allowed"),
	std::make_pair(static_cast<e_StatusCode>(406), "Not Acceptable"),
	std::make_pair(static_cast<e_StatusCode>(407), "Proxy Authentication Required"),
	std::make_pair(static_cast<e_StatusCode>(408), "Request Timeout"),
	std::make_pair(static_cast<e_StatusCode>(409), "Conflict"),
	std::make_pair(static_cast<e_StatusCode>(410), "Gone"),
	std::make_pair(static_cast<e_StatusCode>(411), "Length Required"),
	std::make_pair(static_cast<e_StatusCode>(412), "Precondition Failed"),
	std::make_pair(static_cast<e_StatusCode>(413), "Payload Too Large"),
	std::make_pair(static_cast<e_StatusCode>(414), "URI Too Long"),
	std::make_pair(static_cast<e_StatusCode>(415), "Unsupported Media Type"),
	std::make_pair(static_cast<e_StatusCode>(416), "Range Not Satisfiable"),
	std::make_pair(static_cast<e_StatusCode>(417), "Expectation Failed"),
	std::make_pair(static_cast<e_StatusCode>(418), "I'm a teapot"),
	std::make_pair(static_cast<e_StatusCode>(421), "Misdirected Request"),
	std::make_pair(static_cast<e_StatusCode>(422), "Unprocessable Entity"),
	std::make_pair(static_cast<e_StatusCode>(423), "Locked"),
	std::make_pair(static_cast<e_StatusCode>(424), "Failed Dependency"),
	std::make_pair(static_cast<e_StatusCode>(425), "Too Early"),
	std::make_pair(static_cast<e_StatusCode>(426), "Upgrade Required"),
	std::make_pair(static_cast<e_StatusCode>(428), "Precondition Required"),
	std::make_pair(static_cast<e_StatusCode>(429), "Too Many Requests"),
	std::make_pair(static_cast<e_StatusCode>(431), "Request Header Fields Too Large"),
	std::make_pair(static_cast<e_StatusCode>(451), "Unavailable For Legal Reasons"),
	std::make_pair(static_cast<e_StatusCode>(500), "Internal Server Error"),
	std::make_pair(static_cast<e_StatusCode>(501), "Not Implemented"),
	std::make_pair(static_cast<e_StatusCode>(502), "Bad Gateway"),
	std::make_pair(static_cast<e_StatusCode>(503), "Service Unavailable"),
	std::make_pair(static_cast<e_StatusCode>(504), "Gateway Timeout"),
	std::make_pair(static_cast<e_StatusCode>(505), "HTTP Version Not Supported"),
	std::make_pair(static_cast<e_StatusCode>(506), "Variant Also Negotiates"),
	std::make_pair(static_cast<e_StatusCode>(507), "Insufficient Storage"),
	std::make_pair(static_cast<e_StatusCode>(508), "Loop Detected"),
	std::make_pair(static_cast<e_StatusCode>(510), "Not Extended"),
	std::make_pair(static_cast<e_StatusCode>(511), "Network Authentication Required")
};

std::map<e_StatusCode, std::string>HTTPResponse::kStatusMsg_(kPairs_, &kPairs_[61]);
