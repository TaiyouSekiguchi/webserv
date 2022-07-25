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

		void	RunCommunication(const std::string& msg)
		{
			try
			{
				csocket_->SendRequest(msg);
				req_.ParseRequest(*ssocket_, server_conf_);
				status_code_ = method_.ExecHTTPMethod(req_, server_conf_);
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		int						status_code_;
		HTTPRequest				req_;
		HTTPMethod				method_;
};

Config					GETResTest::config_("conf/get.conf");
const ServerDirective&	GETResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			GETResTest::lsocket_ = NULL;
ServerSocket*			GETResTest::ssocket_ = NULL;
ClientSocket*			GETResTest::csocket_ = NULL;

static const std::string Basic = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 9\r\n\
Server: Webserv\r\n\r\nind.html\n";

static const std::string NotFound = "HTTP/1.1 404 Not Found\r\n\
Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n\
<html>\r\n<head><title>404 Not Found</title></head>\r\n<body>\r\n\
<center><h1>404 Not Found</h1></center>\r\n\
<hr><center>Webserv</center>\r\n\
</body>\r\n</html>\r\n";

static const std::string Root = "HTTP/1.1 200 OK\r\n\
Connection: keep-alive\r\nContent-Length: 26\r\nServer: Webserv\r\n\r\n\
html/sub1/hoge/index.html\n";

static const std::string DirRiderect = "HTTP/1.1 301 Moved Permanently\r\n\
Connection: keep-alive\r\nContent-Length: 164\r\n\
Location: http://localhost:8080/sub1/\r\nServer: Webserv\r\n\r\n\
<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n\
<body>\r\n<center><h1>301 Moved Permanently</h1></center>\r\n\
<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const std::string Index = "HTTP/1.1 200 OK\r\n\
Connection: keep-alive\r\nContent-Length: 20\r\nServer: Webserv\r\n\r\n\
html/sub1/sub1.html\n";

static const std::string DirForbidden = "HTTP/1.1 403 Forbidden\r\n\
Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n\
<html>\r\n<head><title>403 Forbidden</title></head>\r\n\
<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n\
<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

TEST_F(GETResTest, BasicTest)
{
	RunCommunication("GET /ind.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), Basic);
}

TEST_F(GETResTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), NotFound);
}

TEST_F(GETResTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), Root);
}

TEST_F(GETResTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), DirRiderect);
}

TEST_F(GETResTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), Index);
}

TEST_F(GETResTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), DirForbidden);
}
