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

Config					GETTest::config_("conf/get.conf");
const ServerDirective&	GETTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			GETTest::lsocket_ = NULL;
ServerSocket*			GETTest::ssocket_ = NULL;
ClientSocket*			GETTest::csocket_ = NULL;
int						GETTest::kq_ = kqueue();

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
	EXPECT_EQ(method_->GetHeaders()["Location"], "http://localhost:8080/sub1/");
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
