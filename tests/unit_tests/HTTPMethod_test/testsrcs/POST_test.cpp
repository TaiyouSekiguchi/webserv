#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"

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

Config					POSTTest::config_("conf/post.conf");
const ServerDirective&	POSTTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			POSTTest::lsocket_ = NULL;
ServerSocket*			POSTTest::ssocket_ = NULL;
ClientSocket*			POSTTest::csocket_ = NULL;

TEST_F(POSTTest, NotAllowedTest)
{
	RunCommunication("POST / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_METHOD_NOT_ALLOWED);
}

TEST_F(POSTTest, NotFoundTest)
{
	RunCommunication("POST /upload/no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_NOT_FOUND);
}

TEST_F(POSTTest, NotDirTest)
{
	RunCommunication("POST /upload/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_CONFLICT);
}

TEST_F(POSTTest, Upload1Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_CREATED);
	EXPECT_NE(method_->GetLocation().find("/upload/16"), std::string::npos);
}

TEST_F(POSTTest, Upload2Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_CREATED);
	EXPECT_NE(method_->GetLocation().find("/upload/16"), std::string::npos);
}
