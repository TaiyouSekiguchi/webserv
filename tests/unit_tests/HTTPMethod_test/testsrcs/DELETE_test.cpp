#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
class DELETETest : public ::testing::Test
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

Config					DELETETest::config_("conf/delete.conf");
const ServerDirective&	DELETETest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			DELETETest::lsocket_ = NULL;
ServerSocket*			DELETETest::ssocket_ = NULL;
ClientSocket*			DELETETest::csocket_ = NULL;

TEST_F(DELETETest, NotAllowedTest)
{
	RunCommunication("DELETE /hoge/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::METHOD_NOT_ALLOWED);
}

TEST_F(DELETETest, NotFoundTest)
{
	RunCommunication("DELETE /sub1/no.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::NOT_FOUND);
}

TEST_F(DELETETest, NotSlashEndDirTest)
{
	RunCommunication("DELETE /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::CONFLICT);
}

TEST_F(DELETETest, NotEmptyDirTest)
{
	RunCommunication("DELETE /sub1/hoge/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, HTTPError::FORBIDDEN);
}

TEST_F(DELETETest, FileTest)
{
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 204);
}

TEST_F(DELETETest, EmptyDirTest)
{
	mkdir("../../../html/sub1/empty", 0777);
	RunCommunication("DELETE /sub1/empty/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(status_code_, 204);
}
