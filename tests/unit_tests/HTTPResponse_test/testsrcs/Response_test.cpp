#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class ResponseTest : public ::testing::Test
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

		e_StatusCode			status_code_;
		HTTPRequest*			req_;
		HTTPMethod				method_;
		HTTPResponse*			res_;
};

Config					ResponseTest::config_("conf/response.conf");
const ServerDirective&	ResponseTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			ResponseTest::lsocket_ = NULL;
ServerSocket*			ResponseTest::ssocket_ = NULL;
ClientSocket*			ResponseTest::csocket_ = NULL;

const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

const std::string RemoveHeader(std::string res_msg)
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
std::map<int, std::string> StatusMsg_ = SetStatusMsg();

std::string GenerateDefaultHTML(e_StatusCode status_code)
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << static_cast<int>(status_code) << " " << StatusMsg_[status_code] <<"</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << static_cast<int>(status_code) << " " << StatusMsg_[status_code] << "</h1></center>\r\n";
	ss << "<hr><center>" << "Webserv" << "</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";
	return (ss.str());
}

TEST_F(ResponseTest, GETSuccessTest)
{
	const std::string Success = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 16\r\nServer: Webserv\r\n\r\nhtml/index.html\n";
	RunCommunication("GET /index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Success);
}

TEST_F(ResponseTest, DELETESuccessTest)
{
	const std::string DeleteFile = "HTTP/1.1 204 No Content\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), DeleteFile);
}

TEST_F(ResponseTest, POSTSuccessTest)
{
	const std::string Upload = "HTTP/1.1 201 Created\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), Upload);
	EXPECT_NE(res_->GetResMsg().find("/upload/16"), std::string::npos);
}

TEST_F(ResponseTest, DefaultErrorPageTest)
{
	const std::string DefaultErrorPage = "HTTP/1.1 405 Method Not Allowed\r\n"
		"Connection: keep-alive\r\nContent-Length: 166\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(METHOD_NOT_ALLOWED);
	RunCommunication("DELETE /sub2 HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), DefaultErrorPage);
}

// error_page 404	../../../html/40x.html;
TEST_F(ResponseTest, RedirectErrorPageTest)
{
	const std::string RedirectErrorPage = "HTTP/1.1 302 Found\r\n"
		"Connection: keep-alive\r\nContent-Length: 140\r\n"
        "Location: ../../../html/40x.html\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(FOUND);
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), RedirectErrorPage);
}

// error_page 409	/../../../html/40x.html;
TEST_F(ResponseTest, SlashErrorPageTest)
{
    const std::string NotDir = "HTTP/1.1 409 Conflict\r\n"
                               "Connection: keep-alive\r\nContent-Length: 14\r\nServer: Webserv\r\n\r\n"
                               "html/40x.html\n";
    RunCommunication("POST /upload/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
    EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotDir);
}

// error_page 505	/../../../html/50x.html;
TEST_F(ResponseTest, NotMatchErrorPageTest)
{
    const std::string VersionNotSupported = "HTTP/1.1 404 Not Found\r\n"
		"Connection: close\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(NOT_FOUND);
	RunCommunication("GET /no HTTP/1.0\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), VersionNotSupported);
}

TEST_F(ResponseTest, RedirectTest)
{
	const std::string Redirect = "HTTP/1.1 301 Moved Permanently\r\n"
		"Connection: keep-alive\r\nContent-Length: 164\r\n"
		"Location: http://localhost:8080\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(MOVED_PERMANENTLY);
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Redirect);
}

TEST_F(ResponseTest, CloseTest)
{
    const std::string BadRequest = "HTTP/1.1 400 Bad Request\r\n"
		"Connection: close\r\nContent-Length: 152\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(BAD_REQUEST);
	RunCommunication(" GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), BadRequest);
}

TEST_F(ResponseTest, EmptyTest)
{
	const std::string Empty = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 0\r\nServer: Webserv\r\n\r\n";
	RunCommunication("GET /empty.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Empty);
}
