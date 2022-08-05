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

			req_ = new HTTPRequest(*ssocket_);
			method_ = new HTTPMethod(*req_);
			try
			{
				req_->ParseRequest();
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
