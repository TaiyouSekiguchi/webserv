#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class GETResTest : public ::testing::Test
{
	protected:
		static void SetUpTestCase()
		{
			lsocket_ = new ListenSocket(server_conf_.GetListen()[0], server_conf_);
			lsocket_->ListenConnection();
			csocket_ = new ClientSocket();
			csocket_->ConnectServer("127.0.0.1", 8080);
			ssocket_ = new ServerSocket(*lsocket_);
		}
		static void TearDownTestCase()
		{
			delete lsocket_;
			delete ssocket_;
			delete csocket_;
		}
		virtual void TearDown()
		{
			delete req_;
			delete res_;
		}

		void	RunCommunication(const std::string& msg)
		{
			req_ = new HTTPRequest(*ssocket_);
			try
			{
				csocket_->SendRequest(msg);
				req_->ParseRequest();
				status_code_ = method_.ExecHTTPMethod(*req_);
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
			res_ = new HTTPResponse(status_code_, *req_, method_);
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		int						status_code_;
		HTTPRequest*			req_;
		HTTPMethod				method_;
		HTTPResponse*			res_;
};

Config					GETResTest::config_("conf/get.conf");
const ServerDirective&	GETResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			GETResTest::lsocket_ = NULL;
ServerSocket*			GETResTest::ssocket_ = NULL;
ClientSocket*			GETResTest::csocket_ = NULL;

static const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

static std::map<int, std::string> SetStatusMsg()
{
	std::map<int, std::string> StatusMsg;

	StatusMsg[100] = "Continue";
    StatusMsg[101] = "Switching Protocols";
    StatusMsg[102] = "Processing";
    StatusMsg[103] = "Early Hints";
    StatusMsg[200] = "OK";
    StatusMsg[201] = "Created";
    StatusMsg[202] = "Accepted";
    StatusMsg[203] = "Non-Authoritative Information";
    StatusMsg[204] = "No Content";
    StatusMsg[205] = "Reset Content";
    StatusMsg[206] = "Partial Content";
    StatusMsg[207] = "Multi-Status";
    StatusMsg[208] = "Already Reported";
    StatusMsg[226] = "IM Used";
    StatusMsg[300] = "Multiple Choice";
    StatusMsg[301] = "Moved Permanently";
    StatusMsg[302] = "Found";
    StatusMsg[303] = "See Other";
    StatusMsg[304] = "Not Modified";
    StatusMsg[307] = "Temporary Redirect";
    StatusMsg[308] = "Permanent Redirect";
    StatusMsg[400] = "Bad Request";
    StatusMsg[401] = "Unauthorized";
    StatusMsg[402] = "Payment Required";
    StatusMsg[403] = "Forbidden";
    StatusMsg[404] = "Not Found";
    StatusMsg[405] = "Method Not Allowed";
    StatusMsg[406] = "Not Acceptable";
    StatusMsg[407] = "Proxy Authentication Required";
    StatusMsg[408] = "Request Timeout";
    StatusMsg[409] = "Conflict";
    StatusMsg[410] = "Gone";
    StatusMsg[411] = "Length Required";
    StatusMsg[412] = "Precondition Failed";
    StatusMsg[413] = "Payload Too Large";
    StatusMsg[414] = "URI Too Long";
    StatusMsg[415] = "Unsupported Media Type";
    StatusMsg[416] = "Range Not Satisfiable";
    StatusMsg[417] = "Expectation Failed";
    StatusMsg[418] = "I'm a teapot";
    StatusMsg[421] = "Misdirected Request";
    StatusMsg[422] = "Unprocessable Entity";
    StatusMsg[423] = "Locked";
    StatusMsg[424] = "Failed Dependency";
    StatusMsg[425] = "Too Early";
    StatusMsg[426] = "Upgrade Required";
    StatusMsg[428] = "Precondition Required";
    StatusMsg[429] = "Too Many Requests";
    StatusMsg[431] = "Request Header Fields Too Large";
    StatusMsg[451] = "Unavailable For Legal Reasons";
    StatusMsg[500] = "Internal Server Error";
    StatusMsg[501] = "Not Implemented";
    StatusMsg[502] = "Bad Gateway";
    StatusMsg[503] = "Service Unavailable";
    StatusMsg[504] = "Gateway Timeout";
    StatusMsg[505] = "HTTP Version Not Supported";
    StatusMsg[506] = "Variant Also Negotiates";
    StatusMsg[507] = "Insufficient Storage";
    StatusMsg[508] = "Loop Detected";
    StatusMsg[510] = "Not Extended";
    StatusMsg[511] = "Network Authentication Required";
    return StatusMsg;
}
static std::map<int, std::string> StatusMsg_ = SetStatusMsg();

static std::string GenerateDefaultHTML(int status_code)
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << status_code << " " << StatusMsg_[status_code] <<"</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << status_code << " " << StatusMsg_[status_code] << "</h1></center>\r\n";
	ss << "<hr><center>" << "Webserv" << "</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";
	return (ss.str());
}

TEST_F(GETResTest, BasicTest)
{
	const std::string Basic = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 9\r\nServer: Webserv\r\n\r\nind.html\n";
	RunCommunication("GET /ind.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Basic);
}

TEST_F(GETResTest, NotFoundTest)
{
	const std::string NotFound = "HTTP/1.1 404 Not Found\r\n"
		"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(404);
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotFound);
}

TEST_F(GETResTest, RootTest)
{
	const std::string Root = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 26\r\nServer: Webserv\r\n\r\n"
		"html/sub1/hoge/index.html\n";
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Root);
}

TEST_F(GETResTest, DirRedirectTest)
{
	const std::string DirRiderect = "HTTP/1.1 301 Moved Permanently\r\n"
		"Connection: keep-alive\r\nContent-Length: 164\r\n"
		"Location: http://localhost:8080/sub1/\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(301);
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), DirRiderect);
}

TEST_F(GETResTest, IndexTest)
{
	const std::string Index = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 20\r\nServer: Webserv\r\n\r\n"
		"html/sub1/sub1.html\n";
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Index);
}

TEST_F(GETResTest, DirForbiddenTest)
{
	const std::string DirForbidden = "HTTP/1.1 403 Forbidden\r\n"
		"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(403);
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), DirForbidden);
}
