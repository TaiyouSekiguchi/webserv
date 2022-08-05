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

Config					DELETETest::config_("conf/delete.conf");
const ServerDirective&	DELETETest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			DELETETest::lsocket_ = NULL;
ServerSocket*			DELETETest::ssocket_ = NULL;
ClientSocket*			DELETETest::csocket_ = NULL;

TEST_F(DELETETest, NotAllowedTest)
{
	RunCommunication("DELETE /hoge/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_METHOD_NOT_ALLOWED);
}

TEST_F(DELETETest, NotFoundTest)
{
	RunCommunication("DELETE /sub1/no.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_NOT_FOUND);
}

TEST_F(DELETETest, NotSlashEndDirTest)
{
	RunCommunication("DELETE /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_CONFLICT);
}

TEST_F(DELETETest, NotEmptyDirTest)
{
	RunCommunication("DELETE /sub1/hoge/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_FORBIDDEN);
}

TEST_F(DELETETest, FileTest)
{
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_NO_CONTENT);
}

TEST_F(DELETETest, EmptyDirTest)
{
	mkdir("../../../html/sub1/empty", 0777);
	RunCommunication("DELETE /sub1/empty/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_NO_CONTENT);
}
