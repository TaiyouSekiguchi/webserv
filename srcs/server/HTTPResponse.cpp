#include <sstream>
#include <fstream>
#include "HTTPResponse.hpp"

// HTTPResponse::HTTPResponse(int status_code, HTTPRequest req, HTTPMethod method, const ServerDirective server_conf)
HTTPResponse::HTTPResponse(const int &status_code, const ServerDirective &server_conf)
{
	// AppendHeader(req, method);
	AppendHeader();
	res_msg_ = CreateResponse(status_code, server_conf);
	std::cout << res_msg_ << std::endl;
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::SendResponse(ServerSocket *ssocket)
{
	ssocket->SendData(res_msg_);
}

// void HTTPResponse::AppendHeader(const HTTPRequest &req, const HTTPMethod &method)
void HTTPResponse::AppendHeader()
{
	headers_["Server"] = "Webserv";
	headers_["Date"] = GetDate();
	// headers_["Connection"] = req.GetConnection();
	// headers_["Content-Length"] = content_length;
	// headers_["Content-type"] = method.GetContentType();
	// headers_["Location"] = method.Getlocation();
}

std::string HTTPResponse::CreateResponse(const int &status_code, const ServerDirective &server_conf)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << status_code << " " << kStatusMsg_.at(status_code) << "\r\n";
	ss << HeaderFeild();
	if (IsErrorStatus(status_code))
	{
		ss << GenerateHTML(status_code, server_conf);
	}
	else
	{
		// ss << method.Getbody()
	}
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

std::string HTTPResponse::GetDate() const
{
	time_t now = time(NULL);
	struct tm current_time;
	char str[50];

	asctime_r(localtime_r(&now, &current_time), str);
	strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", &current_time);
    return (str);
}

std::string HTTPResponse::GenerateHTML(const int &status_code, const ServerDirective &server_conf) const
{
	std::string error_path = "html" + server_conf.GetErrorPages().at(status_code);

	std::ifstream ifs(error_path);
	if (ifs.fail())
	{
		std::string str = DefaultErrorPage(status_code);
		return (str);
	}
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	return (str);
}

bool HTTPResponse::IsErrorStatus(const int &status_code) const
{
	return (status_code >= 400);
}

std::string HTTPResponse::DefaultErrorPage(const int &status_code) const
{
	std::stringstream ss;

	ss << "Default Error Page\r\n";
	ss << status_code << " " << kStatusMsg_.at(status_code) << "\r\n";
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
