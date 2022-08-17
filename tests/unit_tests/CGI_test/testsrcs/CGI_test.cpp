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
					case SEVENT_CGI_WRITE:
						event_type = RunPostToCgi();
						break;
					case SEVENT_CGI_READ:
						usleep(500);
						event_type = RunReceiveCgiResult();
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
				{
					sleep(1);
					return (new_event);
				}
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

		/*
		e_HTTPServerEventType	RunPostToCgi()
		{
			e_HTTPServerEventType	new_event;

			try
			{
				method_->PostToCgi();
				return (SEVENT_CGI_READ);
			}
			catch (const HTTPError& e)
			{
				e.PutMsg();
				new_event = method_->ValidateErrorPage(e.GetStatusCode());
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			return (SEVENT_NO);
		}

		e_HTTPServerEventType	RunReceiveCgiResult()
		{
			e_HTTPServerEventType	new_event;

			try
			{
				new_event = method_->ReceiveCgiResult();
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			catch (const HTTPError& e)
			{
				e.PutMsg();
				new_event = method_->ValidateErrorPage(e.GetStatusCode());
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			return (SEVENT_NO);
		}
		*/

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

Config					CGITest::config_("conf/cgi.conf");
const ServerDirective&	CGITest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			CGITest::lsocket_ = NULL;
ServerSocket*			CGITest::ssocket_ = NULL;
ClientSocket*			CGITest::csocket_ = NULL;

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
						event_type = RunPostToCgi();
						break;
					case SEVENT_CGI_READ:
						usleep(500);
						event_type = RunReceiveCgiResult();
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

		e_HTTPServerEventType	RunPostToCgi()
		{
			e_HTTPServerEventType	new_event;

			try
			{
				method_->PostToCgi();
				return (SEVENT_CGI_READ);
			}
			catch (const HTTPError& e)
			{
				e.PutMsg();
				new_event = method_->ValidateErrorPage(e.GetStatusCode());
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			return (SEVENT_NO);
		}

		e_HTTPServerEventType	RunReceiveCgiResult()
		{
			e_HTTPServerEventType	new_event;

			try
			{
				new_event = method_->ReceiveCgiResult();
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			catch (const HTTPError& e)
			{
				e.PutMsg();
				new_event = method_->ValidateErrorPage(e.GetStatusCode());
				if (new_event != SEVENT_NO)
					return (new_event);
			}
			return (SEVENT_NO);
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
	RunCommunication("GET /cgi-bin/test.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetHeaders()["content-type"]);
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

/*
TEST_F(CGITest, CommandArgTest)
{
	RunCommunication("GET /cgi-bin/command_arg_test.cgi?aaa+bbb+ccc HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ(first + "=================================\nCommand Arguments\n=================================\naaa\nbbb\nccc\n" + last, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, EnvironmentVariableTest)
{
	RunCommunication("GET /cgi-bin/env_test.cgi?first=aaa&last=bbb HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Debian\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ(first + "=================================\nEnvironment Variable\n=================================\nAUTH_TYPE = [ TEST ]\nCONTENT_LENGTH = [  ]\nCONTENT_TYPE = [  ]\nGATEWAY_INTERFACE = [ CGI/1.1 ]\nHTTP_ACCEPT = [ TEST ]\nHTTP_FORWARDED = [  ]\nHTTP_REFERER = [ TEST ]\nHTTP_USER_AGENT = [ Debian ]\nHTTP_X_FORWARDED_FOR = [  ]\nPATH_INFO = [ /cgi-bin/env_test.cgi ]\nPATH_TRANSLATED = [ ../../../html/cgi-bin/env_test.cgi ]\nQUERY_STRING = [ first=aaa&amp;last=bbb ]\nREMOTE_ADDR = [  ]\nREMOTE_HOST = [  ]\nREMOTE_IDENT = [  ]\nREMOTE_USER = [  ]\nREQUEST_METHOD = [ GET ]\nSCRIPT_NAME = [ /cgi-bin/env_test.cgi ]\nSERVER_NAME = [  ]\nSERVER_PORT = [ 8080 ]\nSERVER_PROTOCOL = [ HTTP/1.1 ]\nSERVER_SOFTWARE = [ 42Webserv ]\n" + last, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, PostTest)
{
	RunCommunication("POST /cgi-bin/post_test.cgi HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 10\r\n\r\nVALUE=abcd");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ(first + "=================================\nForm Variable\n=================================\nVALUE = [ abcd ]" + last, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, NoExistFileTest)
{
	RunCommunication("GET /cgi-bin/noexist.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ("403 Forbidden", method_->GetBody());
	EXPECT_EQ(SC_FORBIDDEN, method_->GetStatusCode());
}

TEST_F(CGITest, EmptyFileTest)
{
	RunCommunication("GET /cgi-bin/empty_file.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ("502 Bad Gateway", method_->GetBody());
	EXPECT_EQ(SC_BAD_GATEWAY, method_->GetStatusCode());
}

TEST_F(CGITest, OnlyShebangFileTest)
{
	RunCommunication("GET /cgi-bin/only_shebang.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ("An error occurred while reading CGI reply (no response received)", method_->GetBody());
	EXPECT_EQ(SC_BAD_GATEWAY, method_->GetStatusCode());
}

TEST_F(CGITest, WrongShebangFileTest)
{
	RunCommunication("GET /cgi-bin/wrong_shebang.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ("502 Bad Gateway", method_->GetBody());
	EXPECT_EQ(SC_BAD_GATEWAY, method_->GetStatusCode());
}

TEST_F(CGITest, BodyStartFileTest)
{
	EXPECT_ANY_THROW(RunCommunication("GET /cgi-bin/body_start.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"));
}

TEST_F(CGITest, NoNewLineFileTest)
{
	EXPECT_ANY_THROW(RunCommunication("GET /cgi-bin/no_nl.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"));
}

TEST_F(CGITest, NoHeaderFileTest)
{
	RunCommunication("GET /cgi-bin/no_header.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ("An error occurred while parsing CGI reply", method_->GetBody());
	EXPECT_EQ(SC_BAD_GATEWAY, method_->GetStatusCode());
}

TEST_F(CGITest, OnlyNewLineFileTest)
{
	RunCommunication("GET /cgi-bin/only_nl.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ("An error occurred while parsing CGI reply", method_->GetBody());
	EXPECT_EQ(SC_BAD_GATEWAY, method_->GetStatusCode());
}

TEST_F(CGITest, MultipleContentTypeTest)
{
	RunCommunication("GET /cgi-bin/multiple_content_type.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, SimpleLocationTest)
{
	RunCommunication("GET /cgi-bin/simple_location.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("test", method_->GetLocation());
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_FOUND, method_->GetStatusCode());
}

TEST_F(CGITest, SpaceSeparatedLocationTest)
{
	RunCommunication("GET /cgi-bin/space_separated_location.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("test test test", method_->GetLocation());
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_FOUND, method_->GetStatusCode());
}

TEST_F(CGITest, MultipleLocationTest)
{
	RunCommunication("GET /cgi-bin/multiple_location.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("An error occurred while parsing CGI reply", method_->GetBody());
	EXPECT_EQ(SC_BAD_GATEWAY, method_->GetStatusCode());
}
*/

/*
TEST_F(CGITest, StatusLocationTest)
{
	RunCommunication("GET /cgi-bin/location_and_status.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("test", method_->GetLocation());
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, SimpleStatusTest)
{
	RunCommunication("GET /cgi-bin/simple_status.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, MultipleStatusTest)
{
	RunCommunication("GET /cgi-bin/multiple_status.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("", method_->GetContentType());
	EXPECT_EQ("", method_->GetLocation());
	EXPECT_EQ(simple_body, method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(CGITest, EmptyStatusTest)
{
	EXPECT_ANY_THROW(RunCommunication("GET /cgi-bin/empty_status.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"));
}

TEST_F(CGITest, SpaceStatusTest)
{
	EXPECT_ANY_THROW(RunCommunication("GET /cgi-bin/space_status.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"));
}

TEST_F(CGITest, StringStatusTest)
{
	EXPECT_ANY_THROW(RunCommunication("GET /cgi-bin/string_status.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"));
}
*/
