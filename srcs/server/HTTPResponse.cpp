#include <sstream>
#include <fstream>
#include <ctime>
#include "HTTPResponse.hpp"
#include "utils.hpp"

HTTPResponse::HTTPResponse(e_StatusCode status_code, const HTTPRequest &req, const HTTPMethod &method)
	: req_(req), method_(method), server_conf_(req.GetServerConf()), status_code_(status_code)
{
	CheckConnection();
	SelectBody();
	AppendHeaders();
	res_msg_ = CreateResponse();
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::SendResponse(const ServerSocket *ssocket)
{
	ssocket->SendData(res_msg_);
}

void HTTPResponse::CheckConnection()
{
	if (status_code_ == SC_BAD_REQUEST || status_code_ == SC_HTTP_VERSION_NOT_SUPPORTED)
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
		(req_.GetMethod() == "GET" || !IsNormalStatus()) ? Utils::ToString(body_.size()) : "");
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

void HTTPResponse::SelectBody()
{
	if (!IsNormalStatus())
	{
		body_ = GenerateHTML();
		return;
	}
	body_ = method_.GetBody();
}

bool HTTPResponse::IsNormalStatus() const
{
	return (status_code_ < 300);
}

std::string HTTPResponse::GenerateHTML()
{
	std::string str;
	std::map<int, std::string>::const_iterator ite = server_conf_->GetErrorPages().find(status_code_);
	if (ite != server_conf_->GetErrorPages().end())
	{
		std::string error_page_path = ite->second;
		if (error_page_path.at(0) == '/')
		{
			error_page_path = error_page_path.substr(1);
			std::ifstream ifs(error_page_path);
			if (ifs.fail())
			{
				status_code_ = SC_NOT_FOUND;
				str = GenerateDefaultHTML();
				return (str);
			}
			std::string file_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			return (file_str);
		}
		status_code_ = SC_FOUND;
		AppendHeader("Location", ite->second);
	}
	str = GenerateDefaultHTML();
	return (str);
}

std::string HTTPResponse::GenerateDefaultHTML() const
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << status_code_ << " " << kStatusMsg_[status_code_] <<"</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << status_code_ << " " << kStatusMsg_[status_code_] << "</h1></center>\r\n";
	ss << "<hr><center>" << "Webserv" << "</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";
	return (ss.str());
}

std::string HTTPResponse::CreateResponse()
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << status_code_ << " " << kStatusMsg_[status_code_] << "\r\n";
	ss << HeaderFeild();
	ss << body_;
	return (ss.str());
}

std::string HTTPResponse::HeaderFeild() const
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

const std::pair<int, std::string> HTTPResponse::kPairs_[] = {
	std::make_pair(SC_CONTINUE, "Continue"),
	std::make_pair(SC_SWITCHING_PROTOCOLS, "Switching Protocols"),
	std::make_pair(SC_OK, "OK"),
	std::make_pair(SC_CREATED, "Created"),
	std::make_pair(SC_ACCEPTED, "Accepted"),
	std::make_pair(SC_NON_AUTHORITATIVE_INFORMATION, "Non-Authoritative Information"),
	std::make_pair(SC_NO_CONTENT, "No Content"),
	std::make_pair(SC_RESET_CONTENT, "Reset Content"),
	std::make_pair(SC_MULTIPLE_CHOICES, "Multiple Choice"),
	std::make_pair(SC_MOVED_PERMANENTLY, "Moved Permanently"),
	std::make_pair(SC_FOUND, "Found"),
	std::make_pair(SC_SEE_OTHER, "See Other"),
	std::make_pair(SC_USE_PROXY, "Use Proxy"),
	std::make_pair(SC_TEMPORARY_REDIRECT, "Temporary Redirect"),
	std::make_pair(SC_BAD_REQUEST, "Bad Request"),
	std::make_pair(SC_PAYMENT_REQUIRED, "Payment Required"),
	std::make_pair(SC_FORBIDDEN, "Forbidden"),
	std::make_pair(SC_NOT_FOUND, "Not Found"),
	std::make_pair(SC_METHOD_NOT_ALLOWED, "Method Not Allowed"),
	std::make_pair(SC_NOT_ACCEPTABLE, "Not Acceptable"),
	std::make_pair(SC_REQUEST_TIMEOUT, "Request Timeout"),
	std::make_pair(SC_CONFLICT, "Conflict"),
	std::make_pair(SC_GONE, "Gone"),
	std::make_pair(SC_LENGTH_REQUIRED, "Length Required"),
	std::make_pair(SC_PAYLOAD_TOO_LARGE, "Payload Too Large"),
	std::make_pair(SC_URI_TOO_LONG, "URI Too Long"),
	std::make_pair(SC_UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"),
	std::make_pair(SC_EXPECTATION_FAILED, "Expectation Failed"),
	std::make_pair(SC_UPGRADE_REQUIRED, "Upgrade Required"),
	std::make_pair(SC_INTERNAL_SERVER_ERROR, "Internal Server Error"),
	std::make_pair(SC_NOT_IMPLEMENTED, "Not Implemented"),
	std::make_pair(SC_BAD_GATEWAY, "Bad Gateway"),
	std::make_pair(SC_SERVISE_UNAVAILABLE, "Service Unavailable"),
	std::make_pair(SC_GATEWAY_TIMEOUT, "Gateway Timeout"),
	std::make_pair(SC_HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"),
};

std::map<int, std::string>HTTPResponse::kStatusMsg_(kPairs_, &kPairs_[35]);
const std::string &HTTPResponse::GetResMsg() const { return res_msg_; }
const bool &HTTPResponse::GetConnection() const { return connection_; }
