#include <gtest/gtest.h>
#include <sys/event.h>
#include <fstream>
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

Config					POSTTest::config_("conf/post.conf");
const ServerDirective&	POSTTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			POSTTest::lsocket_ = NULL;
ServerSocket*			POSTTest::ssocket_ = NULL;
ClientSocket*			POSTTest::csocket_ = NULL;
int						POSTTest::kq_ = kqueue();

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
	EXPECT_NE(method_->GetHeaders()["Location"].find("/upload/16"), std::string::npos);
}

TEST_F(POSTTest, Upload2Test)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_CREATED);
	EXPECT_NE(method_->GetHeaders()["Location"].find("/upload/16"), std::string::npos);
}
