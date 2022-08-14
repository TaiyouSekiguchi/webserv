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
					case SEVENT_CGI_WRITE:
						event_type = RunPostToCgi();
						break;
					case SEVENT_CGI_READ:
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

TEST_F(GETTest, SimpleGet)
{
	RunCommunication("GET /cgi-bin/test.pl HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ("<html>\n<body>\n<div>Welcome CGI test page!! ;)\n</div>\n</body>\n</html>", method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(GETTest, COMMAND_ARG)
{
	RunCommunication("GET /cgi-bin/command_arg_test.cgi?aaa+bbb+ccc HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ("<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<title>CGI TEST</title>\n</head>\n<body>\n<h1>CGI TEST</h1>\n<pre>\n=================================\n\xE3\x82\xB3\xE3\x83\x9E\xE3\x83\xB3\xE3\x83\x89\xE5\xBC\x95\xE6\x95\xB0\n=================================\naaa\nbbb\nccc\n\n</pre>\n</body>\n</html>\n", method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}

TEST_F(GETTest, ENV_TEST)
{
	RunCommunication("GET /cgi-bin/env_test.cgi?first=aaa&last=bbb HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Debian\r\n\r\n");

	EXPECT_EQ("text/html", method_->GetContentType());
	EXPECT_EQ("<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<title>CGI TEST</title>\n</head>\n<body>\n<h1>CGI TEST</h1>\n<pre>\n=================================\n\xE7\x92\xB0\xE5\xA2\x83\xE5\xA4\x89\xE6\x95\xB0\n=================================\nAUTH_TYPE = [ TEST ]\nCONTENT_LENGTH = [  ]\nCONTENT_TYPE = [  ]\nGATEWAY_INTERFACE = [ CGI/1.1 ]\nHTTP_ACCEPT = [ TEST ]\nHTTP_FORWARDED = [  ]\nHTTP_REFERER = [ TEST ]\nHTTP_USER_AGENT = [ Debian ]\nHTTP_X_FORWARDED_FOR = [  ]\nPATH_INFO = [ /cgi-bin/env_test.cgi ]\nPATH_TRANSLATED = [ ../../../html/cgi-bin/env_test.cgi ]\nQUERY_STRING = [ first=aaa&amp;last=bbb ]\nREMOTE_ADDR = [  ]\nREMOTE_HOST = [  ]\nREMOTE_IDENT = [  ]\nREMOTE_USER = [  ]\nREQUEST_METHOD = [ GET ]\nSCRIPT_NAME = [ /cgi-bin/env_test.cgi ]\nSERVER_NAME = [  ]\nSERVER_PORT = [ 8080 ]\nSERVER_PROTOCOL = [ HTTP/1.1 ]\nSERVER_SOFTWARE = [ 42Webserv ]\n\n</pre>\n</body>\n</html>\n", method_->GetBody());
	EXPECT_EQ(SC_OK, method_->GetStatusCode());
}
