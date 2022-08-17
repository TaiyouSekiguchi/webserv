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

class CGITest : public ::testing::Test
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
				std::cout << "event_type : " << event_type << std::endl;
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
					case SEVENT_CGI_WRITE:
						event_type = hserver_->RunPostToCgi();
						break;
					case SEVENT_CGI_READ:
						event_type = hserver_->RunReceiveCgiResult();
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
					EV_SET(&kev, hserver_->GetMethodTargetFileFd(), ET_WRITE, EA_ADD, 0, 0, NULL);
					break;
				case SEVENT_CGI_WRITE:
					EV_SET(&kev, hserver_->GetToCgiPipeFd(), ET_WRITE, EA_ADD, 0, 0, NULL);
					break;
				case SEVENT_CGI_READ:
					EV_SET(&kev, hserver_->GetFromCgiPipeFd(), ET_READ, EA_ADD, 0, 0, NULL);
					break;

				default:
					return;
			}
			kevent(kq_, &kev, 1, NULL, 0, NULL);
		}

		void	WaitEvent()
		{
			std::cout << "WaitEvent called." << std::endl;
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

Config					CGITest::config_("conf/cgi.conf");
const ServerDirective&	CGITest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			CGITest::lsocket_ = NULL;
ServerSocket*			CGITest::ssocket_ = NULL;
ClientSocket*			CGITest::csocket_ = NULL;
int						CGITest::kq_ = kqueue();

const std::string first = "<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<title>CGI TEST</title>\n</head>\n<body>\n<h1>CGI TEST</h1>\n<pre>\n";
const std::string last = "\n</pre>\n</body>\n</html>\n";
const std::string simple_body = "<html>\n<body>\n<div>Welcome CGI test page!! ;)\n</div>\n</body>\n</html>";

TEST_F(CGITest, SimpleGetTest)
{
	std::cout << "RunCommunication called." << std::endl;
	RunCommunication("GET /cgi-bin/test.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	std::cout << "test part start" << std::endl;
	EXPECT_EQ("text/html", method_->GetHeaders()["content-type"]);
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

/*
TEST_F(GETTest, BasicTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(method_->GetStatusCode(), SC_OK);
	EXPECT_EQ(method_->GetBody(), "html/index.html\n");
}
*/
