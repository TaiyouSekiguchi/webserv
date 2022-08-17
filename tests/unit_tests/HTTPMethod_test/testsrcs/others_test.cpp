#include <gtest/gtest.h>
#include <sys/event.h>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"
#include "HTTPServer.hpp"
#include "EventFlag.hpp"

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

			struct kevent	kev;
			EV_SET(&kev, ssocket_->GetFd(), ET_READ, EA_ADD, 0, 0, NULL);
			kevent(kq_, &kev, 1, NULL, 0, NULL);
		}
		static void TearDownTestCase()
		{
			delete lsocket_;
			delete ssocket_;
			delete csocket_;
		}
		virtual void SetUp()
		{
			hserver_ = new HTTPServer(*ssocket_);
			req_ = hserver_->GetRequest();
			method_ = hserver_->GetMethod();
		}
		virtual void TearDown()
		{
			delete hserver_;
		}

		void	RunCommunication(const std::string& msg)
		{
			e_HTTPServerEventType	event_type = SEVENT_SOCKET_RECV;
			e_HTTPServerEventType	next_event_type;
			csocket_->SendRequest(msg);

			while (event_type != SEVENT_SOCKET_SEND && event_type != SEVENT_END)
			{
				WaitEvent();
				switch (event_type)
				{
					case SEVENT_SOCKET_RECV:
						next_event_type = hserver_->Run();
						break;
					case SEVENT_FILE_READ:
					case SEVENT_FILE_WRITE:
					case SEVENT_FILE_DELETE:
						next_event_type = hserver_->RunExecHTTPMethod(event_type);
						break;
					case SEVENT_ERRORPAGE_READ:
						next_event_type = hserver_->RunReadErrorPage();
						break;
					default:
						return;
				}
				if (event_type != next_event_type)
					RegisterEvent(next_event_type);
				event_type = next_event_type;
			}
		}

		void	RegisterEvent(const e_HTTPServerEventType event_type)
		{
			struct kevent	kev;

			switch (event_type)
			{
				case SEVENT_FILE_READ:
				case SEVENT_ERRORPAGE_READ:
					EV_SET(&kev, hserver_->GetMethodTargetFileFd(), ET_READ, EA_ADD, 0, 0, NULL);
					break;
				case SEVENT_FILE_WRITE:
				case SEVENT_FILE_DELETE:
					EV_SET(&kev, hserver_->GetMethodTargetFileFd(), ET_WRITE, EA_ADD, 0, 0, NULL);					break;
				default:
					return;
			}
			kevent(kq_, &kev, 1, NULL, 0, NULL);
		}

		void	WaitEvent()
		{
			struct kevent		kev;
			while (kevent(kq_, NULL, 0, &kev, 1,  NULL) == 0) {}
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;
		static int						kq_;

		HTTPServer*				hserver_;
		HTTPRequest*			req_;
		HTTPMethod*				method_;
};

Config					OthersTest::config_("conf/others.conf");
const ServerDirective&	OthersTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			OthersTest::lsocket_ = NULL;
ServerSocket*			OthersTest::ssocket_ = NULL;
ClientSocket*			OthersTest::csocket_ = NULL;
int						OthersTest::kq_ = kqueue();

TEST_F(OthersTest, ReturnTest)
{
	RunCommunication("AAA /sub2 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_MOVED_PERMANENTLY);
	EXPECT_EQ(method_->GetHeaders()["Location"], "http://localhost:8080");
	EXPECT_NE(method_->GetBody().find("301 Moved Permanently"), std::string::npos);
}

TEST_F(OthersTest, ReturnErrorPageTest)
{
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_TEMPORARY_REDIRECT);
	EXPECT_EQ(method_->GetHeaders()["Location"], "http://localhost:8080");
	EXPECT_EQ(method_->GetBody(), "html/hello.html\n");
}

TEST_F(OthersTest, ReturnLocationTest)
{
	RunCommunication("AAA /sub1/noindex HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_BAD_REQUEST);
	EXPECT_EQ(method_->GetHeaders()["Location"], "");
	EXPECT_EQ(method_->GetBody(), "http://localhost:8080");
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

// 405 Method Not Allowed
TEST_F(OthersTest, DefaultErrorPageTest)
{
	RunCommunication("DELETE /sub1 HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_METHOD_NOT_ALLOWED);
	EXPECT_NE(method_->GetBody().find("405 Method Not Allowed"), std::string::npos);
}

// error_page 404	../../../html/40x.html;  (404 Not Found)
TEST_F(OthersTest, RedirectErrorPageTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_FOUND);
	EXPECT_EQ(method_->GetHeaders()["Location"], "../../../html/40x.html");
	EXPECT_NE(method_->GetBody().find("302 Found"), std::string::npos);
}

// error_page 400	/../../../html/40x.html;  (400 Bad Request)
TEST_F(OthersTest, HitErrorPageTest)
{
    RunCommunication("   GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_BAD_REQUEST);
	EXPECT_EQ(method_->GetBody(), "html/40x.html\n");
}

// error_page 505	/../../../html/50x.html;  (505 HTTP Version Not Supported)
TEST_F(OthersTest, NotFoundErrorPageTest)
{
	RunCommunication("GET /no HTTP/1.0\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_NOT_FOUND);
	EXPECT_NE(method_->GetBody().find("404 Not Found"), std::string::npos);
}
