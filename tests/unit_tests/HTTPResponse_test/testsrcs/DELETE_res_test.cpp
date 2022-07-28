#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class DELETEResTest : public ::testing::Test
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

Config					DELETEResTest::config_("conf/delete.conf");
const ServerDirective&	DELETEResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			DELETEResTest::lsocket_ = NULL;
ServerSocket*			DELETEResTest::ssocket_ = NULL;
ClientSocket*			DELETEResTest::csocket_ = NULL;

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

TEST_F(DELETEResTest, NotAllowedTest)
{
	const std::string NotAllowed = "HTTP/1.1 405 Method Not Allowed\r\n"
		"Connection: keep-alive\r\nContent-Length: 166\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(405);
	RunCommunication("DELETE /hoge/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotAllowed);
}

TEST_F(DELETEResTest, NotSlashEndDirTest)
{
	const std::string NotSlashEndDir = "HTTP/1.1 409 Conflict\r\n"
		"Connection: keep-alive\r\nContent-Length: 146\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(409);
	RunCommunication("DELETE /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotSlashEndDir);
}

TEST_F(DELETEResTest, NotEmptyDirTest)
{
	const std::string NotEmptyDir = "HTTP/1.1 403 Forbidden\r\n"
		"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(403);
	RunCommunication("DELETE /sub1/hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotEmptyDir);
}

TEST_F(DELETEResTest, FileTest)
{
	const std::string DeleteFile = "HTTP/1.1 204 No Content\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), DeleteFile);
}

TEST_F(DELETEResTest, EmptyDirTest)
{
	const std::string EmptyDir = "HTTP/1.1 204 No Content\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	mkdir("../../../html/sub1/empty", 0777);
	RunCommunication("DELETE /sub1/empty/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), EmptyDir);
}

/* error_pageのpathがベタがきのhtmlなので階層が合わない
TEST_F(DELETEResTest, NotFoundTest)
{
	const std::string NotFoud = "HTTP/1.1 404 Not Found\r\n"
		"Connection: keep-alive\r\nContent-Length: 14\r\nServer: Webserv\r\n\r\n"
		"html/40x.html\n";
	RunCommunication("DELETE /sub1/no.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), NotFoud);
}
 */
