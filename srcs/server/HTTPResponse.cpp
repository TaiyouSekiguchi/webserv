#include <sstream>
#include <fstream>
#include <ctime>
#include "HTTPResponse.hpp"
#include "utils.hpp"

HTTPResponse::HTTPResponse(int status_code, const HTTPRequest &req, const HTTPMethod &method)
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
				status_code_ = 404;
				str = GenerateDefaultHTML();
				return (str);
			}
			std::string file_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			return (file_str);
		}
		status_code_ = 302;
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

std::map<int, std::string>HTTPResponse::kStatusMsg_(kPairs_, &kPairs_[61]);
const std::string &HTTPResponse::GetResMsg() const { return res_msg_; }
const bool &HTTPResponse::GetConnection() const { return connection_; }
