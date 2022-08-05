#include <gtest/gtest.h>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"

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

Config					GETTest::config_("conf/get.conf");
const ServerDirective&	GETTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			GETTest::lsocket_ = NULL;
ServerSocket*			GETTest::ssocket_ = NULL;
ClientSocket*			GETTest::csocket_ = NULL;

TEST_F(GETTest, BasicTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
	EXPECT_EQ(method_->GetBody(), "html/index.html\n");
}

TEST_F(GETTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_NOT_FOUND);
}

TEST_F(GETTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
	EXPECT_EQ(method_->GetBody(), "html/sub1/hoge/index.html\n");
}

TEST_F(GETTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_MOVED_PERMANENTLY);
	EXPECT_EQ(method_->GetLocation(), "http://localhost:8080/sub1/");
}

TEST_F(GETTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
	EXPECT_EQ(method_->GetBody(), "html/sub1/sub1.html\n");
}

TEST_F(GETTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_FORBIDDEN);
}
