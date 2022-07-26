#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"

class POSTTest : public ::testing::Test
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
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		int						status_code_;
		HTTPRequest*			req_;
		HTTPMethod				method_;
};

Config					POSTTest::config_("conf/post.conf");
const ServerDirective&	POSTTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			POSTTest::lsocket_ = NULL;
ServerSocket*			POSTTest::ssocket_ = NULL;
ClientSocket*			POSTTest::csocket_ = NULL;

TEST_F(POSTTest, NotAllowedTest)
{
	RunCommunication("POST / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::METHOD_NOT_ALLOWED);
}

TEST_F(POSTTest, NotFoundTest)
{
	RunCommunication("POST /upload/no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::NOT_FOUND);
}

TEST_F(POSTTest, NotDirTest)
{
	RunCommunication("POST /upload/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::CONFLICT);
}

TEST_F(POSTTest, Upload1Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 201);
	EXPECT_NE(method_.GetLocation().find("/upload/16"), std::string::npos);
}

TEST_F(POSTTest, Upload2Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 201);
	EXPECT_NE(method_.GetLocation().find("/upload/16"), std::string::npos);
}
