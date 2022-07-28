#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class OthersResTest : public ::testing::Test
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

Config					OthersResTest::config_("conf/others.conf");
const ServerDirective&	OthersResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			OthersResTest::lsocket_ = NULL;
ServerSocket*			OthersResTest::ssocket_ = NULL;
ClientSocket*			OthersResTest::csocket_ = NULL;

static const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

static const std::string RemoveDateBody(std::string res_msg)
{
	std::string::size_type pos_date = res_msg.find("Date");
	std::string str = res_msg.erase(pos_date, 37);
	std::string::size_type pos_s = res_msg.find("<html>");
	str = res_msg.erase(pos_s);
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

TEST_F(OthersResTest, ReturnTest)
{
	const std::string Return = "HTTP/1.1 301 Moved Permanently\r\n"
		"Connection: keep-alive\r\nContent-Length: 164\r\n"
		"Location: http://localhost:8080\r\nServer: Webserv\r\n\r\n" + GenerateDefaultHTML(301);
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Return);
}

TEST_F(OthersResTest, UnknownMethodTest)
{
	const std::string UnknownMethod = "HTTP/1.1 405 Method Not Allowed\r\n"
	"Connection: keep-alive\r\nContent-Length: 166\r\nServer: Webserv\r\n\r\n" + GenerateDefaultHTML(405);
	RunCommunication("AAA / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), UnknownMethod);
}

TEST_F(OthersResTest, ValidCGITest)
{
	const std::string ValidCGI = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	RunCommunication("GET /cgi-bin/tohoho.pl HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), ValidCGI);
}

TEST_F(OthersResTest, AutoIndexTest)
{
	const std::string AutoIndex = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 386\r\nServer: Webserv\r\n\r\n";
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDateBody(res_->GetResMsg()), AutoIndex);

	const std::string&	body = res_->GetResMsg();
	EXPECT_NE(body.find("<head><title>Index of /sub1/</title></head>"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"hoge/\">hoge/</a>\t\t"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"index.html\">index.html</a>\t\t"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"noindex/\">noindex/</a>\t\t"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"sub1.html\">sub1.html</a>\t\t"), std::string::npos);
}

TEST_F(OthersResTest, ErrorPageRedirectTest)
{
	const std::string ErrorPage = "HTTP/1.1 302 Found\r\n"
		"Connection: keep-alive\r\nContent-Length: 140\r\nLocation: 40x.html\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(302);
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), ErrorPage);
}

TEST_F(OthersResTest, VersionNotSupportTest)
{
	const std::string VersionNotSupport = "HTTP/1.1 505 HTTP Version Not Supported\r\n"
		"Connection: keep-alive\r\nContent-Length: 182\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(505);
	RunCommunication("GET /no HTTP/1.0\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), VersionNotSupport);
}

/* 
TEST_F(OthersResTest, BadRequestTest)
{
	const std::string BadRequest = "HTTP/1.1 400 Bad Request\r\n"
		"Connection: close\r\nContent-Length: 152\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(400);
	RunCommunication(" GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), BadRequest);
}

TEST_F(OthersResTest, NotPermittedTest)
{
	const std::string NotPermitted = "HTTP/1.1 403 Forbidden\r\n"
		"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(403);
	RunCommunication("GET /sub2/sub2.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotPermitted);
}

TEST_F(OthersResTest, NotPermittedAutoIndexTest)
{
	const std::string NotPermittedAutoIndex = "HTTP/1.1 500 Internal Server Error\r\n"
		"Connection: keep-alive\r\nContent-Length: 172\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(500);
	RunCommunication("GET /sub3/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotPermittedAutoIndex);
}
 */
