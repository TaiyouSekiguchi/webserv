#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class POSTResTest : public ::testing::Test
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

Config					POSTResTest::config_("conf/post.conf");
const ServerDirective&	POSTResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			POSTResTest::lsocket_ = NULL;
ServerSocket*			POSTResTest::ssocket_ = NULL;
ClientSocket*			POSTResTest::csocket_ = NULL;

static const std::string RemoveHeader(std::string res_msg)
{
	std::string::size_type pos_date = res_msg.find("Date");
	std::string str = res_msg.erase(pos_date, 37);
	std::string::size_type pos_location = res_msg.find("Location");
	if (pos_location != std::string::npos)
		str = res_msg.erase(pos_location, 36);
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

TEST_F(POSTResTest, NotAllowedTest)
{
	const std::string NotAllowed = "HTTP/1.1 405 Method Not Allowed\r\n"
		"Connection: keep-alive\r\nContent-Length: 166\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(405);
	RunCommunication("POST / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), NotAllowed);
}

TEST_F(POSTResTest, NotFoundTest)
{
	const std::string NotFound = "HTTP/1.1 404 Not Found\r\n"
		"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(404);
	RunCommunication("POST /upload/no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), NotFound);
}

TEST_F(POSTResTest, NotDirTest)
{
	const std::string NotDir = "HTTP/1.1 409 Conflict\r\n"
		"Connection: keep-alive\r\nContent-Length: 146\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(409);
	RunCommunication("POST /upload/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), NotDir);
}

TEST_F(POSTResTest, UploadTest)
{
	const std::string Upload = "HTTP/1.1 201 Created\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), Upload);
	EXPECT_NE(res_->GetResMsg().find("/upload/16"), std::string::npos);
}
