#include <gtest/gtest.h>
#include <gtest/gtest.h>
#include <iostream>
#include "HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"

class CGITest : public ::testing::Test
{
  protected:
	virtual void SetUp()
	{
		Config	config("./default.conf");

		const std::vector<ServerDirective>&				servers = config.GetServers();
		std::vector<ServerDirective>::const_iterator	itr = servers.begin();

		lsocket_ = new ListenSocket(*itr, servers.at(0));
		lsocket_->ListenConnection();
		csocket_ = new ClientSocket();
		csocket_->ConnectServer("127.0.0.1", 8080);
		ssocket_ = new ServerSocket(lsocket_->AcceptConnection(), lsocket_->GetServerConf());
	}
	virtual void TearDown()
	{
		delete lsocket_;
		delete ssocket_;
		delete csocket_;
	}
		ListenSocket *lsocket_;
		ServerSocket *ssocket_;
		ClientSocket *csocket_;
};

TEST_F(CGITest, standard)
{
	HTTPRequest		req(*ssocket_, ssocket_->GetServerConf());
	CGI				cgi("./test.cgi", req);

	EXPECT_EQ("text/html", cgi.GetContentType());
	EXPECT_EQ("<html>\n<body>\n<div>Welcome CGI test page!! ;)\nGATEWAY_INTERFACE [CGI/1.1]\nCONTENT_LENGTH    []\n</div>\n</body>\n</html>", cgi.GetBody());

	EXPECT_ANY_THROW(CGI cgi("./noexist.cgi", req));
}
