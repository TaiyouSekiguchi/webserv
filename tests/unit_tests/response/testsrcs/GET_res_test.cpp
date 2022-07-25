#include <gtest/gtest.h>
#include <fstream>
#include "ListenSocket.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"
#include "HTTPMethod.hpp"
#include "HTTPResponse.hpp"

class GETResTest : public ::testing::Test
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
			HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
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

Config			GETResTest::config_("conf/get.conf");
ListenSocket*	GETResTest::lsocket_ = NULL;
ServerSocket*	GETResTest::ssocket_ = NULL;
ClientSocket*	GETResTest::csocket_ = NULL;

static const std::string RemoveDate(std::string res_msg)
{
	std::string::size_type pos_s = res_msg.find("Date");
	std::string str = res_msg.erase(pos_s, 37);
	return (str);
}

TEST_F(GETResTest, BasicTest)
{
	RunCommunication("GET /ind.html HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	EXPECT_EQ(RemoveDate(res.GetResMsg()), Basic);
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Basic");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

/* 
TEST_F(GETResTest, NotFoundTest)
{
	RunCommunication("GET /no HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/NotFound");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, RootTest)
{
	RunCommunication("GET /hoge/ HTTP/1.1\r\nHost: localhost:8085\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Root");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, DirRedirectTest)
{
	RunCommunication("GET /sub1 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/DirRedirect");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, IndexTest)
{
	RunCommunication("GET /sub1/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/Index");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}

TEST_F(GETResTest, DirForbiddenTest)
{
	RunCommunication("GET /sub2/ HTTP/1.1\r\nHost: localhost:8080\r\n\r\n");
	HTTPResponse res(status_code_, req_, method_, ssocket_->GetServerConf());
	std::ifstream ifs("samp/GET/DirForbidden");
	std::string samp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	EXPECT_EQ(RemoveDate(res.GetResMsg()), samp);
}
 */

static std::string *Basic = "HTTP/1.1 200 OK\r\n"
+ "Connection: keep-alive\r\n"+ "Content-Length: 9\r\n" + "Server: Webserv\r\n\r\n"
+ "ind.html\n";
/* 

static std::string *NotFound = "HTTP/1.1 404 Not Found\r\n"
+ "Connection: keep-alive\r\n" + "Content-Length: 148\r\n" + "Server: Webserv\r\n\r\n"
+ "<html>\r\n" + "<head><title>404 Not Found</title></head>\r\n"
+ "<body>\r\n" + "<center><h1>404 Not Found</h1></center>\r\n"
+ "<hr><center>Webserv</center>\r\n" + "</body>\r\n" + "</html>\r\n";

static std::string *Root = "HTTP/1.1 200 OK\r\n"
+ "Connection: keep-alive\r\n" + "Content-Length: 26\r\n" + "Server: Webserv\r\n\r\n"
+ "html/sub1/hoge/index.html\n";

static std::string *DirRiderect = "HTTP/1.1 301 Moved Permanently\r\n"
+ "Connection: keep-alive\r\n" + "Content-Length: 164\r\n"
+ "Location: http://localhost:8080/sub1/\r\n" + "Server: Webserv\r\n\r\n"
+ "<html>\r\n" + "<head><title>301 Moved Permanently</title></head>\r\n" + "<body>\r\n"
+ "<center><h1>301 Moved Permanently</h1></center>\r\n"
+ "<hr><center>Webserv</center>\r\n" + "</body>\r\n" + "</html>";

static std::string *Index = "HTTP/1.1 200 OK\r\n"
+ "Connection: keep-alive\r\n" + "Content-Length: 20\r\n" + "Server: Webserv\r\n\r\n"
+ "html/sub1/sub1.html\n";

static std::string *DirForbidden = "HTTP/1.1 403 Forbidden\r\n"
"Connection: keep-alive\r\n" + "Content-Length: 148\r\n" + "Server: Webserv\r\n\r\n"
+ "<html>\r\n" + "<head><title>403 Forbidden</title></head>\r\n" + "<body>\r\n"
+ "<center><h1>403 Forbidden</h1></center>\r\n"
+ "<hr><center>Webserv</center>\r\n" + "</body>\r\n" + "</html>\r\n";
 */
