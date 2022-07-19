#include <gtest/gtest.h>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"

class OthersTest : public ::testing::Test
{
	protected:
		static void SetUpTestCase()
		{
			lsocket_ = new ListenSocket(*(config_.GetServers().begin()));
			lsocket_->ListenConnection();
			csocket_ = new ClientSocket();
			csocket_->ConnectServer("127.0.0.1", 8080);
			ssocket_ = new ServerSocket(lsocket_->AcceptConnection(), lsocket_->GetServerConf());
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
				req_.ParseRequest(*ssocket_, ssocket_->GetServerConf());
				status_code_ = method_.ExecHTTPMethod(req_, ssocket_->GetServerConf());
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
		}

		static Config			config_;
		static ListenSocket		*lsocket_;
		static ServerSocket 	*ssocket_;
		static ClientSocket		*csocket_;

		int						status_code_;
		HTTPRequest				req_;
		HTTPMethod				method_;
};

Config			OthersTest::config_("conf/others.conf");
ListenSocket*	OthersTest::lsocket_ = NULL;
ServerSocket*	OthersTest::ssocket_ = NULL;
ClientSocket*	OthersTest::csocket_ = NULL;

TEST_F(OthersTest, ReturnTest)
{
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 301);
	EXPECT_EQ(method_.GetLocation(), "http://localhost:8080");
}

TEST_F(OthersTest, UnknownMethodTest)
{
	RunCommunication("AAA / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::METHOD_NOT_ALLOWED);
}

TEST_F(OthersTest, ValidCGITest)
{
	RunCommunication("GET /cgi-bin/tohoho.pl HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 200);
	EXPECT_EQ(method_.GetBody(), "");
}

TEST_F(OthersTest, AutoIndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 200);
	EXPECT_EQ(method_.GetBody(),
		"<html>\r\n"
		"<head><title>Index of /</title></head>\r\n"
		"<body>\r\n"
		"<h1>Index of /</h1><hr><pre><a href=\"../\">../</a>\r\n"
		"<a href=\"hoge/\">hoge/</a>\t\tSun Jul 17 15:57:27 2022\t-\r\n"
		"<a href=\"index.html\">index.html</a>\t\tSun Jul 17 15:57:00 2022\t21\r\n"
		"<a href=\"noindex/\">noindex/</a>\t\tSun Jul 17 15:57:27 2022\t-\r\n"
		"<a href=\"sub1.html\">sub1.html</a>\t\tSun Jul 17 15:57:00 2022\t20\r\n"
		"</pre><hr></body>\r\n"
		"</html>\r\n"
	);
}
