#include <gtest/gtest.h>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"

class GETTest : public ::testing::Test
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

Config					GETTest::config_("conf/get.conf");
const ServerDirective&	GETTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			GETTest::lsocket_ = NULL;
ServerSocket*			GETTest::ssocket_ = NULL;
ClientSocket*			GETTest::csocket_ = NULL;

TEST_F(GETTest, BasicTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 200);
	EXPECT_EQ(method_.GetBody(), "html/index.html\n");
}

TEST_F(GETTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::NOT_FOUND);
}

TEST_F(GETTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 200);
	EXPECT_EQ(method_.GetBody(), "html/sub1/hoge/index.html\n");
}

TEST_F(GETTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 301);
	EXPECT_EQ(method_.GetLocation(), "http://localhost:8080/sub1/");
}

TEST_F(GETTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 200);
	EXPECT_EQ(method_.GetBody(), "html/sub1/sub1.html\n");
}

TEST_F(GETTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::FORBIDDEN);
}
