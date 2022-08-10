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

		void	RunCommunication(const std::string& msg)
		{
			e_HTTPServerEventType	event_type = SEVENT_SOCKET_RECV;
			csocket_->SendRequest(msg);

			while (event_type != SEVENT_NO)
			{
				switch (event_type)
				{
					case SEVENT_SOCKET_RECV:
						event_type = Run();
						break;
					case SEVENT_FILE_READ:
					case SEVENT_FILE_WRITE:
					case SEVENT_FILE_DELETE:
						event_type = RunExecHTTPMethod(event_type);
						break;
					case SEVENT_ERRORPAGE_READ:
						event_type = RunReadErrorPage();
						break;
					default: {}
				}
			}
		}

		e_HTTPServerEventType	Run()
		{
			e_HTTPServerEventType	new_event;

			try
			{
				new_event = req_->ParseRequest();
				if (new_event != SEVENT_NO)
					return (SEVENT_SOCKET_RECV);
				new_event = method_->ValidateHTTPMethod();
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			catch (const HTTPError& e)
			{
				new_event = method_->ValidateErrorPage(e.GetStatusCode());
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			return (SEVENT_NO);
		}

		e_HTTPServerEventType	RunExecHTTPMethod(e_HTTPServerEventType event_type)
		{
			e_HTTPServerEventType	new_event;

			try
			{
				if (event_type == SEVENT_FILE_READ)
					method_->ExecGETMethod();
				else if (event_type == SEVENT_FILE_WRITE)
					method_->ExecPOSTMethod();
				else if (event_type == SEVENT_FILE_DELETE)
					method_->ExecDELETEMethod();
			}
			catch (const HTTPError& e)
			{
				new_event = method_->ValidateErrorPage(e.GetStatusCode());
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			return (SEVENT_NO);
		}

		e_HTTPServerEventType	RunReadErrorPage()
		{
			method_->ReadErrorPage();
			return (SEVENT_NO);
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

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

TEST_F(GETTest, EmptyFileTest)
{
	RunCommunication("GET /empty.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
	EXPECT_EQ(method_->GetBody(), "");
}
