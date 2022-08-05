#include <gtest/gtest.h>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"

class OthersTest : public ::testing::Test
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
		virtual void SetUp()
		{
			req_ = new HTTPRequest(*ssocket_);
			method_ = new HTTPMethod(*req_);
		}
		virtual void TearDown()
		{
			delete req_;
			delete method_;
		}

		void	RunMethodEvent()
		{
			switch (event_type_)
			{
				case SEVENT_FILE_READ:
				case SEVENT_FILE_WRITE:
				case SEVENT_FILE_DELETE:
					RunExecHTTPMethod();
					break;
				case SEVENT_ERRORPAGE_READ:
					RunReadErrorPage();
					break;
				default: {}
			}
		}

		void	RunCommunication(const std::string& msg)
		{
			try
			{
				csocket_->SendRequest(msg);
				req_->ParseRequest();
				event_type_ = method_->ValidateHTTPMethod();
				RunMethodEvent();
			}
			catch (const HTTPError& e)
			{
				event_type_ = method_->ValidateErrorPage(e.GetStatusCode());
				RunMethodEvent();
			}
		}

		void	RunExecHTTPMethod()
		{
			try
			{
				if (event_type_ == SEVENT_FILE_READ)
					method_->ExecGETMethod();
				else if (event_type_ == SEVENT_FILE_WRITE)
					method_->ExecPOSTMethod();
				else if (event_type_ == SEVENT_FILE_DELETE)
					method_->ExecDELETEMethod();
			}
			catch (const HTTPError& e)
			{
				event_type_ = method_->ValidateErrorPage(e.GetStatusCode());
				RunMethodEvent();
			}
		}

		void	RunReadErrorPage()
		{
			method_->ReadErrorPage();
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		e_HTTPServerEventType	event_type_;
		HTTPRequest*			req_;
		HTTPMethod*				method_;
};

Config					OthersTest::config_("conf/others.conf");
const ServerDirective&	OthersTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			OthersTest::lsocket_ = NULL;
ServerSocket*			OthersTest::ssocket_ = NULL;
ClientSocket*			OthersTest::csocket_ = NULL;

TEST_F(OthersTest, ReturnTest)
{
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_MOVED_PERMANENTLY);
	EXPECT_EQ(method_->GetLocation(), "http://localhost:8080");
}

TEST_F(OthersTest, UnknownMethodTest)
{
	RunCommunication("AAA / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_METHOD_NOT_ALLOWED);
}

// TEST_F(OthersTest, ValidCGITest)
// {
// 	RunCommunication("GET /cgi-bin/tohoho.pl HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
// 	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
// 	EXPECT_EQ(method_->GetBody(), "");
// }

TEST_F(OthersTest, AutoIndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
	const std::string&	body = method_->GetBody();
	EXPECT_NE(body.find("<head><title>Index of /sub1/</title></head>"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"hoge/\">hoge/</a>\t\t"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"index.html\">index.html</a>\t\t"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"noindex/\">noindex/</a>\t\t"), std::string::npos);
	EXPECT_NE(body.find("<a href=\"sub1.html\">sub1.html</a>\t\t"), std::string::npos);
}
