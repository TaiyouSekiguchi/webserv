#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "Model.hpp"

class GETRESTest : public ::testing::Test
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

Config			GETRESTest::config_("conf/get.conf");
ListenSocket*	GETRESTest::lsocket_ = NULL;
ServerSocket*	GETRESTest::ssocket_ = NULL;
ClientSocket*	GETRESTest::csocket_ = NULL;

const std::vector<std::string> rm_headers = {"ETag", "Last-Modified", "Accept-Ranges", "Server", "Content-Type"};
const std::vector<std::string> cmp_headers = {"Connection", "Date", "Location"};

static const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string s = res_msg.erase(pos_s, 37);
	return (s);
}

TEST_F(GETRESTest, BasicTest)
{
	RunCommunication("GET /ind.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Basic");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	RemoveDate(res.GetResMsg());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETRESTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/NotFound");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	RemoveDate(res.GetResMsg());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETRESTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Root");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	RemoveDate(res.GetResMsg());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETRESTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/DirRedirect");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	RemoveDate(res.GetResMsg());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETRESTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Index");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	RemoveDate(res.GetResMsg());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETRESTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/DirForbidden");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	RemoveDate(res.GetResMsg());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}
