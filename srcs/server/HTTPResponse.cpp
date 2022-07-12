#include <sstream>
#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(int status_code, HTTPRequest req, HTTPMethod method, ServerDirective server_conf)
{
	res_msg_ = CreateResponse(status_code, req);
}

HTTPResponse::~HTTPResponse()
{
}

std::string HTTPResponse::GetResMsg() const { return res_msg_; };

void HTTPResponse::SendResponse(ServerSocket *ssocket)
{
	ssize_t send_size = send(ssocket->GetFd(), res_msg_.c_str(), res_msg_.size(), 0);
	if (send_size == -1)
		throw std::runtime_error("send error");
}


std::string HTTPResponse::CreateResponse(int status_code, HTTPRequest req)
{
	std::stringstream ss;

	ss << req.GetVersion() << " " << status_code << FindMsg(status_code) << "\r\n";
	ss << AppendHeader(req);
	// ss << method.Getbody()
	return ss.str();
}

std::string HTTPResponse::AppendHeader(HTTPRequest req)
{
	std::stringstream ss;

	ss << "date: " << GetDate() << "\r\n";
	ss << "content-type: " << req.GetContentType() << "\r\n";
	ss << "server: webserv\r\n";
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

std::string HTTPResponse::FindMsg(int status_code) const
{
	std::map<int, std::string> status_msg;
	status_msg[100] = "Continue";
	status_msg[101] = "Switching Protocols";
	status_msg[102] = "Processing";
	status_msg[103] = "Early Hints";
	status_msg[200] = "OK";
	status_msg[201] = "Created";
	status_msg[202] = "Accepted";
	status_msg[203] = "Non-Authoritative Information";
	status_msg[204] = "No Content";
	status_msg[205] = "Reset Content";
	status_msg[206] = "Partial Content";
	status_msg[207] = "Multi-Status";
	status_msg[208] = "Already Reported";
	status_msg[226] = "IM Used";
	status_msg[300] = "Multiple Choice";
	status_msg[301] = "Moved Permanently";
	status_msg[302] = "Found";
	status_msg[303] = "See Other";
	status_msg[304] = "Not Modified";
	status_msg[307] = "Temporary Redirect";
	status_msg[308] = "Permanent Redirect";
	status_msg[400] = "Bad Request";
	status_msg[401] = "Unauthorized";
	status_msg[402] = "Payment Required";
	status_msg[403] = "Forbidden";
	status_msg[404] = "Not Found";
	status_msg[405] = "Method Not Allowed";
	status_msg[406] = "Not Acceptable";
	status_msg[407] = "Proxy Authentication Required";
	status_msg[408] = "Request Timeout";
	status_msg[409] = "Conflict";
	status_msg[410] = "Gone";
	status_msg[411] = "Length Required";
	status_msg[412] = "Precondition Failed";
	status_msg[413] = "Payload Too Large";
	status_msg[414] = "URI Too Long";
	status_msg[415] = "Unsupported Media Type";
	status_msg[416] = "Range Not Satisfiable";
	status_msg[417] = "Expectation Failed";
	status_msg[418] = "I'm a teapot";
	status_msg[421] = "Misdirected Request";
	status_msg[422] = "Unprocessable Entity";
	status_msg[423] = "Locked";
	status_msg[424] = "Failed Dependency";
	status_msg[425] = "Too Early";
	status_msg[426] = "Upgrade Required";
	status_msg[428] = "Precondition Required";
	status_msg[429] = "Too Many Requests";
	status_msg[431] = "Request Header Fields Too Large";
	status_msg[451] = "Unavailable For Legal Reasons";
	status_msg[500] = "Internal Server Error";
	status_msg[501] = "Not Implemented";
	status_msg[502] = "Bad Gateway";
	status_msg[503] = "Service Unavailable";
	status_msg[504] = "Gateway Timeout";
	status_msg[505] = "HTTP Version Not Supported";
	status_msg[506] = "Variant Also Negotiates";
	status_msg[507] = "Insufficient Storage";
	status_msg[508] = "Loop Detected";
	status_msg[510] = "Not Extended";
	status_msg[511] = "Network Authentication Required";
	return status_msg[status_code];
}
