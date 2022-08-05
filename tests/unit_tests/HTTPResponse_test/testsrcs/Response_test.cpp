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
			delete res_;
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
			return (RunCreateResponse());
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
			return (RunCreateResponse());
		}

		e_HTTPServerEventType	RunReadErrorPage()
		{
			method_->ReadErrorPage();
			return (RunCreateResponse());
		}

		e_HTTPServerEventType	RunCreateResponse()
		{
			res_ = new HTTPResponse(*req_, *method_);
			return (SEVENT_NO);
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		HTTPRequest*			req_;
		HTTPMethod*				method_;
		HTTPResponse*			res_;
};

Config					ResponseTest::config_("conf/response.conf");
const ServerDirective&	ResponseTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			ResponseTest::lsocket_ = NULL;
ServerSocket*			ResponseTest::ssocket_ = NULL;
ClientSocket*			ResponseTest::csocket_ = NULL;

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
		str = res_msg.erase(pos_location, 36);
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
	EXPECT_NE(res_->GetResMsg().find("/upload/16"), std::string::npos);
}

TEST_F(ResponseTest, RedirectTest)
{
	const std::string Redirect = "HTTP/1.1 301 Moved Permanently\r\n"
		"Connection: keep-alive\r\nContent-Length: 164\r\n"
		"Location: http://localhost:8080\r\nServer: Webserv\r\n\r\n"
		+ GenerateDefaultHTML(SC_MOVED_PERMANENTLY);
	RunCommunication("AAA /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveDate(res_->GetResMsg()), Redirect);
}

TEST_F(ResponseTest, CloseTest)
{
    const std::string BadRequest = "HTTP/1.1 400 Bad Request\r\n"
		"Connection: close\r\nContent-Length: 152\r\nServer: Webserv\r\n\r\n"
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
