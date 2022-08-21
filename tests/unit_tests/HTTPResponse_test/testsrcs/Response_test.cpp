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

class ResponseTest : public ::testing::Test
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
			req_ = hserver_->GetRequest();
			method_ = hserver_->GetMethod();
			res_ = hserver_->GetResponse();
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
		HTTPResponse*			res_;
};

Config					ResponseTest::config_("conf/response.conf");
const ServerDirective&	ResponseTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			ResponseTest::lsocket_ = NULL;
ServerSocket*			ResponseTest::ssocket_ = NULL;
ClientSocket*			ResponseTest::csocket_ = NULL;
int						ResponseTest::kq_ = kqueue();

const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

const std::string RemoveHeader(std::string res_msg)
{
	std::string::size_type pos_date = res_msg.find("Date");
	std::string str = res_msg.erase(pos_date, 37);
	std::string::size_type pos_location = res_msg.find("Location");
	if (pos_location != std::string::npos)
		str = res_msg.erase(pos_location, 40);
	return (str);
}

std::string GenerateDefaultHTML(e_StatusCode status_code)
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << status_code << " " << HTTPResponse::kStatusMsg_[status_code] <<"</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << status_code << " " << HTTPResponse::kStatusMsg_[status_code] << "</h1></center>\r\n";
	ss << "<hr><center>" << "Webserv" << "</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";
	return (ss.str());
}

TEST_F(ResponseTest, GETSuccessTest)
{
	const std::string Success = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 16\r\nServer: Webserv\r\n\r\nhtml/index.html\n";
	RunCommunication("GET /index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Success);
}

TEST_F(ResponseTest, DELETESuccessTest)
{
	const std::string DeleteFile = "HTTP/1.1 204 No Content\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), DeleteFile);
}

TEST_F(ResponseTest, POSTSuccessTest)
{
	const std::string Upload = "HTTP/1.1 201 Created\r\n"
		"Connection: keep-alive\r\nServer: Webserv\r\n\r\n";
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), Upload);
	EXPECT_NE(res_->GetResMsg().find("/upload/2022"), std::string::npos);
}

TEST_F(ResponseTest, RedirectTest)
{
	const std::string Redirect = "HTTP/1.1 301 Moved Permanently\r\n"
		"Connection: keep-alive\r\nContent-Length: 164\r\nContent-Type: text/plain\r\n"
		"Location: http://localhost:8080\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(SC_MOVED_PERMANENTLY);
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Redirect);
}

TEST_F(ResponseTest, CloseTest)
{
    const std::string BadRequest = "HTTP/1.1 400 Bad Request\r\n"
		"Connection: close\r\nContent-Length: 152\r\nContent-Type: text/plain\r\n"
		"Server: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(SC_BAD_REQUEST);
	RunCommunication(" GET /no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), BadRequest);
}

TEST_F(ResponseTest, EmptyTest)
{
	const std::string Empty = "HTTP/1.1 200 OK\r\n"
		"Connection: keep-alive\r\nContent-Length: 0\r\nServer: Webserv\r\n\r\n";
	RunCommunication("GET /empty.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Empty);
}
