#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class DELETEResTest : public ::testing::Test
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

		void	RunCommunication(const std::string& msg)
		{
			try
			{
				csocket_->SendRequest(msg);
				req_.ParseRequest(*ssocket_, server_conf_);
				status_code_ = method_.ExecHTTPMethod(req_, server_conf_);
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
		}

		static Config					config_;
		static const ServerDirective&	server_conf_;
		static ListenSocket*			lsocket_;
		static ServerSocket*			ssocket_;
		static ClientSocket*			csocket_;

		int						status_code_;
		HTTPRequest				req_;
		HTTPMethod				method_;
};

Config					DELETEResTest::config_("conf/delete.conf");
const ServerDirective&	DELETEResTest::server_conf_ = *(config_.GetServers().begin());
ListenSocket*			DELETEResTest::lsocket_ = NULL;
ServerSocket*			DELETEResTest::ssocket_ = NULL;
ClientSocket*			DELETEResTest::csocket_ = NULL;

static const char NotAllowed[] = "HTTP/1.1 405 Method Not Allowed\r\n"
	"Connection: keep-alive\r\nContent-Length: 166\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>405 Method Not Allowed</title></head>\r\n"
	"<body>\r\n<center><h1>405 Method Not Allowed</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char NotFoud[] = "HTTP/1.1 404 Not Found\r\n"
	"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>404 Not Found</title></head>\r\n"
	"<body>\r\n<center><h1>404 Not Found</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char NotSlashEndDir[] = "HTTP/1.1 409 Conflict\r\n"
	"Connection: keep-alive\r\nContent-Length: 146\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>409 Conflict</title></head>\r\n"
	"<body>\r\n<center><h1>409 Conflict</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char NotEmptyDir[] = "HTTP/1.1 403 Forbidden\r\n"
	"Connection: keep-alive\r\nContent-Length: 148\r\nServer: Webserv\r\n\r\n"
	"<html>\r\n<head><title>403 Forbidden</title></head>\r\n"
	"<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n"
	"<hr><center>Webserv</center>\r\n</body>\r\n</html>\r\n";

static const char DeleteFile[] = "HTTP/1.1 204 No Content\r\n"
	"Connection: keep-alive\r\nContent-Length: 0\r\nServer: Webserv\r\n\r\n";

static const char EmptyDir[] = "HTTP/1.1 204 No Content\r\n"
	"Connection: keep-alive\r\nContent-Length: 0\r\nServer: Webserv\r\n\r\n";

const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

TEST_F(DELETEResTest, NotAllowedTest)
{
	RunCommunication("DELETE /hoge/index.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), NotAllowed);
}

TEST_F(DELETEResTest, NotFoundTest)
{
	RunCommunication("DELETE /sub1/no.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), NotFoud);
}

TEST_F(DELETEResTest, NotSlashEndDirTest)
{
	RunCommunication("DELETE /sub1/hoge HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), NotSlashEndDir);
}

TEST_F(DELETEResTest, NotEmptyDirTest)
{
	RunCommunication("DELETE /sub1/hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), NotEmptyDir);
}

TEST_F(DELETEResTest, FileTest)
{
	std::fstream	output_fstream;
	output_fstream.open("../../../html/sub1/delete.html", std::ios_base::out);
	RunCommunication("DELETE /sub1/delete.html HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), DeleteFile);
}

TEST_F(DELETEResTest, EmptyDirTest)
{
	mkdir("../../../html/sub1/empty", 0777);
	RunCommunication("DELETE /sub1/empty/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, server_conf_);
	EXPECT_EQ(RemoveDate(res.GetResMsg()), EmptyDir);
}
