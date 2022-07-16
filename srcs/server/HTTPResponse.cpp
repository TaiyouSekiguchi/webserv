#include <sstream>
#include <fstream>
#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(int status_code, const HTTPRequest &req,
							const HTTPMethod &method, const ServerDirective &server_conf)
{
	AppendHeader(&status_code, req, method, server_conf);
	res_msg_ = CreateResponse(status_code);
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::SendResponse(ServerSocket *ssocket)
{
	ssocket->SendData(res_msg_);
}

void HTTPResponse::AppendHeader(int *status_code, const HTTPRequest &req,
									const HTTPMethod &method, const ServerDirective &server_conf)
{
	headers_["Server"] = "Webserv";
	headers_["Date"] = GetDate();
	headers_["Connection"] = req.GetConnection() ? "keep-alive" : "close";
	ParseHeader(status_code, method, server_conf);
}

std::string HTTPResponse::GetDate() const
{
	time_t now = time(NULL);
	struct tm current_time;
	char str[50];

	asctime_r(localtime_r(&now, &current_time), str);
	strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", &current_time);
    return (str);
}

void HTTPResponse::ParseHeader(int *status_code, const HTTPMethod &method, const ServerDirective &server_conf)
{
	if (!method.GetContentType().empty())
	{
		headers_["Content-type"] = method.GetContentType();
	}
	if (!method.GetLocation().empty())
	{
		headers_["Location"] = method.GetLocation();
	}
	SelectBody(status_code, method, server_conf);
	if (!body_.empty())
	{
		std::stringstream ss;

		ss << body_.size();
		headers_["Content-Length"] = ss.str();
	}
}

void HTTPResponse::SelectBody(int *status_code, const HTTPMethod &method, const ServerDirective &server_conf)
{
	if (!IsNormalStatus(*status_code))
	{
		body_ = GenerateHTML(status_code, server_conf);
		return;
	}
	body_ = method.GetBody();
}

bool HTTPResponse::IsNormalStatus(const int &status_code) const
{
	return (status_code < 400);
}

std::string HTTPResponse::GenerateHTML(int *status_code, const ServerDirective &server_conf)
{
	std::map<int, std::string>::const_iterator ite = server_conf.GetErrorPages().find(*status_code);

	if (ite != server_conf.GetErrorPages().end())
	{
		*status_code = 302;
		headers_["Location"] = ite->second;
	}

	std::string str = GenerateDefaultHTML(*status_code);
		return (str);
}

std::string HTTPResponse::GenerateDefaultHTML(const int &status_code) const
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << status_code << " " << kStatusMsg_.at(status_code) <<"</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << status_code << " " << kStatusMsg_.at(status_code) << "</h1></center>\r\n";
	ss << "<hr><center>" << headers_.at("Server") << "</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";
	return (ss.str());
}

std::string HTTPResponse::CreateResponse(const int &status_code)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << status_code << " " << kStatusMsg_.at(status_code) << "\r\n";
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
		ss << ite->first << ": " << ite->second << "\r\n";
	}
	ss << "\r\n";
	return (ss.str());
}

const std::pair<int, std::string> HTTPResponse::kPairs_[] = {
	std::make_pair(100, "Continue"),
	std::make_pair(101, "Switching Protocols"),
	std::make_pair(102, "Processing"),
	std::make_pair(103, "Early Hints"),
	std::make_pair(200, "OK"),
	std::make_pair(201, "Created"),
	std::make_pair(202, "Accepted"),
	std::make_pair(203, "Non-Authoritative Information"),
	std::make_pair(204, "No Content"),
	std::make_pair(205, "Reset Content"),
	std::make_pair(206, "Partial Content"),
	std::make_pair(207, "Multi-Status"),
	std::make_pair(208, "Already Reported"),
	std::make_pair(226, "IM Used"),
	std::make_pair(300, "Multiple Choice"),
	std::make_pair(301, "Moved Permanently"),
	std::make_pair(302, "Found"),
	std::make_pair(303, "See Other"),
	std::make_pair(304, "Not Modified"),
	std::make_pair(307, "Temporary Redirect"),
	std::make_pair(308, "Permanent Redirect"),
	std::make_pair(400, "Bad Request"),
	std::make_pair(401, "Unauthorized"),
	std::make_pair(402, "Payment Required"),
	std::make_pair(403, "Forbidden"),
	std::make_pair(404, "Not Found"),
	std::make_pair(405, "Method Not Allowed"),
	std::make_pair(406, "Not Acceptable"),
	std::make_pair(407, "Proxy Authentication Required"),
	std::make_pair(408, "Request Timeout"),
	std::make_pair(409, "Conflict"),
	std::make_pair(410, "Gone"),
	std::make_pair(411, "Length Required"),
	std::make_pair(412, "Precondition Failed"),
	std::make_pair(413, "Payload Too Large"),
	std::make_pair(414, "URI Too Long"),
	std::make_pair(415, "Unsupported Media Type"),
	std::make_pair(416, "Range Not Satisfiable"),
	std::make_pair(417, "Expectation Failed"),
	std::make_pair(418, "I'm a teapot"),
	std::make_pair(421, "Misdirected Request"),
	std::make_pair(422, "Unprocessable Entity"),
	std::make_pair(423, "Locked"),
	std::make_pair(424, "Failed Dependency"),
	std::make_pair(425, "Too Early"),
	std::make_pair(426, "Upgrade Required"),
	std::make_pair(428, "Precondition Required"),
	std::make_pair(429, "Too Many Requests"),
	std::make_pair(431, "Request Header Fields Too Large"),
	std::make_pair(451, "Unavailable For Legal Reasons"),
	std::make_pair(500, "Internal Server Error"),
	std::make_pair(501, "Not Implemented"),
	std::make_pair(502, "Bad Gateway"),
	std::make_pair(503, "Service Unavailable"),
	std::make_pair(504, "Gateway Timeout"),
	std::make_pair(505, "HTTP Version Not Supported"),
	std::make_pair(506, "Variant Also Negotiates"),
	std::make_pair(507, "Insufficient Storage"),
	std::make_pair(508, "Loop Detected"),
	std::make_pair(510, "Not Extended"),
	std::make_pair(511, "Network Authentication Required")
};

const std::map<int, std::string>HTTPResponse::kStatusMsg_(kPairs_, &kPairs_[61]);
