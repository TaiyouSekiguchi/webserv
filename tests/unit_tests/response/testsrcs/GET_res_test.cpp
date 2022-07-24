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

Config			GETResTest::config_("conf/get.conf");
ListenSocket*	GETResTest::lsocket_ = NULL;
ServerSocket*	GETResTest::ssocket_ = NULL;
ClientSocket*	GETResTest::csocket_ = NULL;

static const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

TEST_F(GETResTest, BasicTest)
{
	RunCommunication("GET /ind.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Basic");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/NotFound");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Root");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/DirRedirect");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Index");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/DirForbidden");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}
