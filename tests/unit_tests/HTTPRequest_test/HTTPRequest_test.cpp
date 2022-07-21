#include <gtest/gtest.h>
#include <iostream>
#include "./HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"

class RequestTest : public ::testing::Test
{
  protected:
	virtual void SetUp()
	{
		Config	config("./default.conf");

		const std::vector<ServerDirective>&				servers = config.GetServers();
		std::vector<ServerDirective>::const_iterator	itr = servers.begin();

		lsocket_ = new ListenSocket(*itr);
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

TEST_F(RequestTest, ParseMethodTest)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseMethod("GET");
	EXPECT_EQ("GET", req1.GetMethod());

	HTTPRequest		req2(*ssocket_, ssocket_->GetServerConf());
	req2.ParseMethod("POST");
	EXPECT_EQ("POST", req2.GetMethod());

	HTTPRequest		req3(*ssocket_, ssocket_->GetServerConf());
	req3.ParseMethod("DELETE");
	EXPECT_EQ("DELETE", req3.GetMethod());

	HTTPRequest		req4(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req4.ParseMethod("test"));

	HTTPRequest		req5(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req5.ParseMethod("TEST["));
}

TEST_F(RequestTest, ParseTargetTest)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseTarget("/");
	EXPECT_EQ("/", req1.GetTarget());

	HTTPRequest		req2(*ssocket_, ssocket_->GetServerConf());
	req2.ParseTarget("/index.html");
	EXPECT_EQ("/index.html", req2.GetTarget());
}

TEST_F(RequestTest, ParseVersionTest)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseVersion("HTTP/1.1");
	EXPECT_EQ("HTTP/1.1", req1.GetVersion());

	HTTPRequest		req2(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req2.ParseVersion("HTTP/1.0"));

	HTTPRequest		req3(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req3.ParseVersion("HTTP/0.0"));

	HTTPRequest		req4(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req4.ParseVersion("HTTP/1.11"));
}

TEST_F(RequestTest, ParseHostTest)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseHost(" localhost:8080");
	EXPECT_EQ("localhost:8080", req1.GetHost());

	HTTPRequest		req2(*ssocket_, ssocket_->GetServerConf());
	req2.ParseHost(" developer.mozilla.org");
	EXPECT_EQ("developer.mozilla.org", req2.GetHost());
}

TEST_F(RequestTest, ParseContentLength)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseContentLength(" 123");
	EXPECT_EQ((size_t)123, req1.GetContentLength());

	HTTPRequest		req2(*ssocket_, ssocket_->GetServerConf());
	req2.ParseContentLength(" 0");
	EXPECT_EQ((size_t)0, req2.GetContentLength());

	HTTPRequest		req3(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req3.ParseContentLength(" 12345abc"));

	HTTPRequest		req4(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req4.ParseContentLength(""));

	HTTPRequest		req5(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req5.ParseContentLength("         "));

	HTTPRequest		req6(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req6.ParseContentLength("-1234"));

	HTTPRequest		req7(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req7.ParseContentLength("123 456 789"));

	HTTPRequest		req8(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req8.ParseContentLength("123,456,789"));

	HTTPRequest		req10(*ssocket_, ssocket_->GetServerConf());
	EXPECT_ANY_THROW(req10.ParseContentLength("111111111111"));
}

TEST_F(RequestTest, ParseUserAgent)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseUserAgent("   Debian    ");
	EXPECT_EQ("Debian", req1.GetUserAgent());
}

TEST_F(RequestTest, ParseAcceptEncodingTest)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	req1.ParseAcceptEncoding(" gzip, compress, br");
	EXPECT_EQ("gzip", req1.GetAcceptEncoding().at(0));
	EXPECT_EQ("compress", req1.GetAcceptEncoding().at(1));
	EXPECT_EQ("br", req1.GetAcceptEncoding().at(2));
}

TEST_F(RequestTest, ParseConnectionTest)
{
	HTTPRequest		req1(*ssocket_, ssocket_->GetServerConf());
	EXPECT_EQ(true, req1.GetConnection());
	
	HTTPRequest		req2(*ssocket_, ssocket_->GetServerConf());
	req2.ParseConnection("close");
	EXPECT_EQ(false, req2.GetConnection());

	HTTPRequest		req3(*ssocket_, ssocket_->GetServerConf());
	req3.ParseConnection("CLOSE");
	EXPECT_EQ(false, req3.GetConnection());

	HTTPRequest		req4(*ssocket_, ssocket_->GetServerConf());
	req4.ParseConnection("Close");
	EXPECT_EQ(false, req4.GetConnection());

	HTTPRequest		req5(*ssocket_, ssocket_->GetServerConf());
	req5.ParseConnection("cloSE");
	EXPECT_EQ(false, req5.GetConnection());
}

TEST_F(RequestTest, GetLine)
{
	HTTPRequest		req(*ssocket_, ssocket_->GetServerConf());

	const char	*msg1 = "GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
	csocket_->SendRequest(msg1);

	EXPECT_EQ("GET / HTTP/1.1", req.GetLine());
	EXPECT_EQ("Host: localhost:8080", req.GetLine());
	EXPECT_EQ("", req.GetLine());

	const char	*msg2 = "aaaaa\r\nbbbbb\r\nccccc\r\n";
	csocket_->SendRequest(msg2);

	EXPECT_EQ("aaaaa", req.GetLine());
	EXPECT_EQ("bbbbb", req.GetLine());
	EXPECT_EQ("ccccc", req.GetLine());
}

TEST_F(RequestTest, RequestTest)
{
	HTTPRequest		req(*ssocket_, ssocket_->GetServerConf());

	const char	*msg1 = "POST / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: debian\r\nContent-Type: text/html\r\nContent-Length: 10\r\n\r\naaaaaaaaaa";
	csocket_->SendRequest(msg1);

	req.ParseRequest();
	EXPECT_EQ("POST", req.GetMethod());
	EXPECT_EQ("/", req.GetTarget());
	EXPECT_EQ("HTTP/1.1", req.GetVersion());
	EXPECT_EQ("localhost:8080", req.GetHost());
	EXPECT_EQ((size_t)10, req.GetContentLength());
	EXPECT_EQ(true, req.GetConnection());
	EXPECT_EQ("debian", req.GetUserAgent());
	EXPECT_EQ("text/html", req.GetContentType());
	EXPECT_EQ("aaaaaaaaaa", req.GetBody());
}
