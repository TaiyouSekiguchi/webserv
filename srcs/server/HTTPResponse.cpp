#include <sstream>
#include <fstream>
#include "HTTPResponse.hpp"

const size_t HTTPResponse::BUF_SIZE = 10000;

// HTTPResponse::HTTPResponse(int status_code, HTTPRequest req, HTTPMethod method, const ServerDirective server_conf)
HTTPResponse::HTTPResponse(const int &status_code, const HTTPRequest &req, const ServerDirective &server_conf)
{
	// AppendHeader(req, method);
	AppendHeader(req);
	res_msg_ = CreateResponse(status_code, server_conf);
	std::cout << res_msg_ << std::endl;
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::SendResponse(ServerSocket *ssocket)
{
	ssize_t send_size = send(ssocket->GetFd(), res_msg_.c_str(), res_msg_.size(), 0);
	if (send_size == -1)
		throw std::runtime_error("send error");
}

// void HTTPResponse::AppendHeader(const HTTPRequest &req, const HTTPMethod &method)
void HTTPResponse::AppendHeader(const HTTPRequest &req)
{
	if (req.GetConnection())
	{
		headers_.insert(std::make_pair("Connection", "keep-alive"));
	}
	else
	{
		headers_.insert(std::make_pair("Connection", "close"));
	}
	headers_.insert(std::make_pair("Server", "Webserv"));
	headers_.insert(std::make_pair("Date", GetDate()));
	// headers_.insert(std::make_pair("Content-Length: ", content_length);
	// headers_.insert(std::make_pair("Content-type", method.GetContentType()));
	// headers_.insert(std::make_pair("Location", method.Getlocation()));
}

std::string HTTPResponse::CreateResponse(const int &status_code, const ServerDirective &server_conf)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << status_code << " " << STATUS_MSG_.at(status_code) << "\r\n";
	ss << HeaderFeild();
	if (IsErrorStatus(status_code))
	{
		ss << GenerateHTML(status_code, server_conf);
	}
	else
	{
		// ss << method.Getbody()
	}
	return ss.str();
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
	return ss.str();
}

std::string HTTPResponse::GetDate() const
{
	time_t now = time(NULL);
	struct tm current_time;
	char str[50];

	asctime_r(localtime_r(&now, &current_time), str);
	strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", &current_time);
    return str;
}

std::string HTTPResponse::GenerateHTML(const int &status_code, const ServerDirective &server_conf) const
{
	std::stringstream ss;
	std::ifstream ifs;
	std::string error_path = server_conf.GetErrorPages().at(status_code);

	if (error_path.at(0) == '/')
	{
		error_path = error_path.substr(1, error_path.size());
	}
	ifs.open(error_path.c_str());
	if (!ifs)
	{
		return "";
	}
	char buffer[BUF_SIZE];
	ssize_t read_byte;
	while (!ifs.eof())
	{
		ifs.read(buffer, BUF_SIZE - 1);
		read_byte = ifs.gcount();
		buffer[read_byte] = '\0';
		ss << buffer;
	}
	ifs.close();
	return ss.str();
}

bool HTTPResponse::IsErrorStatus(const int &status_code) const
{
	return status_code >= 400;
}

const std::map<int, std::string> HTTPResponse::STATUS_MSG_ = {
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
