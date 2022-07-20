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

class RESTest : public ::testing::Test
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

Config			RESTest::config_("conf/default.conf");
ListenSocket*	RESTest::lsocket_ = NULL;
ServerSocket*	RESTest::ssocket_ = NULL;
ClientSocket*	RESTest::csocket_ = NULL;

const std::string &HTTPResponse::GetResMsg() const { return res_msg_; }

/* 
TEST_F(RESTest, BasicTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, ReturnTest)
{
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, UnknownMethodTest)
{
	RunCommunication("AAA / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, NotAllowedTest)
{
	RunCommunication("DELETE /hoge/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << status_code_ << std::endl;
	std::cout << res.GetResMsg() << std::endl;
}

TEST_F(RESTest, FileTest)
{
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::cout << res.GetResMsg() << std::endl;
}
 */

TEST(CrulTest, curl)
{
	Model model("localhost:8080");
	std::cout << model.GetResponse();
}
