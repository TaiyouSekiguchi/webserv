#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class POSTResTest : public ::testing::Test
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
		}

		void	RunCommunication(const std::string& msg)
		{
			try
			{
				csocket_->SendRequest(msg);
				req_ = new HTTPRequest(*ssocket_);
				req_->ParseRequest();
				status_code_ = method_.ExecHTTPMethod(*req_, server_conf_);
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
			res_ = new HTTPResponse(status_code_, *req_, method_, server_conf_);
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		int						status_code_;
		HTTPRequest*			req_;
		HTTPMethod				method_;
		HTTPResponse*			res_;
};

Config					POSTResTest::config_("conf/post.conf");
const ServerDirective&	POSTResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			POSTResTest::lsocket_ = NULL;
ServerSocket*			POSTResTest::ssocket_ = NULL;
ClientSocket*			POSTResTest::csocket_ = NULL;

static const char NotAllowed[] = "HTTP/1.1 405 Method Not Allowed\r\n"
	"Connection: keep-alive\r\nContent-Length: 166\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>405 Method Not Allowed</title></head>\r\n"
	"<body>\r\n<center><h1>405 Method Not Allowed</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char NotFound[] = "HTTP/1.1 404 Not Found\r\n"
	"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>404 Not Found</title></head>\r\n"
	"<body>\r\n<center><h1>404 Not Found</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char NotDir[] = "HTTP/1.1 409 Conflict\r\n"
	"Connection: keep-alive\r\nContent-Length: 146\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>409 Conflict</title></head>\r\n"
	"<body>\r\n<center><h1>409 Conflict</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char Upload[] = "HTTP/1.1 201 Created\r\n"
	"Connection: keep-alive\r\nContent-Length: 0\r\nServer: Webserv\r\n\r\n";

static const std::string RemoveHeader(std::string res_msg)
{
	std::string::size_type pos_date = res_msg.find("Date");
	std::string str = res_msg.erase(pos_date, 37);
	std::string::size_type pos_location = res_msg.find("Location");
	if (pos_location != std::string::npos)
		str = res_msg.erase(pos_location, 36);
	return (str);
}

TEST_F(POSTResTest, NotAllowedTest)
{
	RunCommunication("POST / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), NotAllowed);
}

TEST_F(POSTResTest, NotFoundTest)
{
	RunCommunication("POST /upload/no HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), NotFound);
}

TEST_F(POSTResTest, NotDirTest)
{
	RunCommunication("POST /upload/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), NotDir);
}

TEST_F(POSTResTest, UploadTest)
{
	RunCommunication("POST /upload HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	EXPECT_EQ(RemoveHeader(res_->GetResMsg()), Upload);
	EXPECT_NE(res_->GetResMsg().find("/upload/16"), std::string::npos);
}
