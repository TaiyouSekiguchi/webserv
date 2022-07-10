#include <gtest/gtest.h>
#include <iostream>
#include "./HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"

TEST(ParseMthodTest, ParseMethodTest)
{
	HTTPRequest		req1;
	req1.ParseMethod("GET");
	EXPECT_EQ(HTTPRequest::GET, req1.GetMethod());

	HTTPRequest		req2;
	req2.ParseMethod("POST");
	EXPECT_EQ(HTTPRequest::POST, req2.GetMethod());

	HTTPRequest		req3;
	req3.ParseMethod("DELETE");
	EXPECT_EQ(HTTPRequest::DELETE, req3.GetMethod());

	HTTPRequest		req4;
	EXPECT_ANY_THROW(req4.ParseMethod("test"));

	HTTPRequest		req5;
	EXPECT_ANY_THROW(req5.ParseMethod(""));
}

TEST(ParseTargetTest, ParseTargetTest)
{
	HTTPRequest		req1;
	req1.ParseTarget("/");
	EXPECT_EQ("/", req1.GetTarget());

	HTTPRequest		req2;
	req2.ParseTarget("/index.html");
	EXPECT_EQ("/index.html", req2.GetTarget());

	HTTPRequest		req3;
	EXPECT_ANY_THROW(req3.ParseMethod(""));
	
	HTTPRequest		req4;
	EXPECT_ANY_THROW(req4.ParseMethod("index.html"));
}

TEST(ParseVersionTest, ParseVersionTest)
{
	HTTPRequest		req1;
	req1.ParseVersion("HTTP/1.1");
	EXPECT_EQ("HTTP/1.1", req1.GetVersion());

	HTTPRequest		req2;
	EXPECT_ANY_THROW(req2.ParseVersion("HTTP/1.0"));

	HTTPRequest		req3;
	EXPECT_ANY_THROW(req3.ParseVersion(""));

	HTTPRequest		req4;
	EXPECT_ANY_THROW(req4.ParseVersion("HTTP/0.0"));

	HTTPRequest		req5;
	EXPECT_ANY_THROW(req5.ParseVersion("HTTP/1.11"));
}

TEST(ParseHostTest, ParseHostTest)
{
	HTTPRequest		req1;
	req1.ParseHost(" localhost:8080");
	EXPECT_EQ("localhost", req1.GetHost().first);
	EXPECT_EQ(8080, req1.GetHost().second);

	HTTPRequest		req2;
	req2.ParseHost(" developer.mozilla.org");
	EXPECT_EQ("developer.mozilla.org", req2.GetHost().first);
	EXPECT_EQ(80, req2.GetHost().second);

	HTTPRequest		req3;
	EXPECT_ANY_THROW(req3.ParseHost("localhost:8080 test"));

	HTTPRequest		req4;
	EXPECT_ANY_THROW(req4.ParseHost("localhost:80:80"));
}

TEST(ParseContentLength, ParseContentLength)
{
	HTTPRequest		req1;
	req1.ParseContentLength(" 1234567");
	EXPECT_EQ((size_t)1234567, req1.GetContentLength());

	HTTPRequest		req2;
	req2.ParseContentLength(" 0");
	EXPECT_EQ((size_t)0, req2.GetContentLength());

	HTTPRequest		req3;
	EXPECT_ANY_THROW(req3.ParseContentLength(" 12345abc"));
}

TEST(ParseUserAgent, ParseUserAgent)
{
	HTTPRequest		req1;
	req1.ParseUserAgent("   Debian    ");
	EXPECT_EQ("Debian", req1.GetUserAgent());
}

TEST(ParseAcceptEncodingTest, ParseAcceptEncodingTest)
{
	HTTPRequest		req1;
	req1.ParseAcceptEncoding(" gzip, compress, br");
	EXPECT_EQ("gzip", req1.GetAcceptEncoding().at(0));
	EXPECT_EQ("compress", req1.GetAcceptEncoding().at(1));
	EXPECT_EQ("br", req1.GetAcceptEncoding().at(2));
}

TEST(ParseConnectionTest, ParseConnectionTest)
{
	HTTPRequest		req1;
	EXPECT_EQ(true, req1.GetConnection());
	
	HTTPRequest		req2;
	req2.ParseConnection("close");
	EXPECT_EQ(false, req2.GetConnection());

	HTTPRequest		req3;
	req3.ParseConnection("CLOSE");
	EXPECT_EQ(false, req3.GetConnection());

	HTTPRequest		req4;
	req4.ParseConnection("Close");
	EXPECT_EQ(false, req4.GetConnection());

	HTTPRequest		req5;
	req5.ParseConnection("cloSE");
	EXPECT_EQ(false, req5.GetConnection());
}

class RequestTest : public ::testing::Test
{
  protected:
    virtual void SetUp()
    {
      lsocket_ = new ListenSocket();
      lsocket_->ListenConnection();
      csocket_ = new ClientSocket();
      csocket_->ConnectServer("127.0.0.1", 8080);
      ssocket_ = new ServerSocket(lsocket_->AcceptConnection());
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

TEST_F(RequestTest, GetLine)
{
	HTTPRequest        req;

	const char	*msg1 = "GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
	csocket_->SendRequest(msg1);

	EXPECT_EQ("GET / HTTP/1.1", req.GetLine(*ssocket_));
	EXPECT_EQ("Host: localhost:8080", req.GetLine(*ssocket_));
	EXPECT_EQ("", req.GetLine(*ssocket_));

	const char	*msg2 = "aaaaa\r\nbbbbb\r\nccccc\r\n";
	csocket_->SendRequest(msg2);

	EXPECT_EQ("aaaaa", req.GetLine(*ssocket_));
	EXPECT_EQ("bbbbb", req.GetLine(*ssocket_));
	EXPECT_EQ("ccccc", req.GetLine(*ssocket_));
}

TEST_F(RequestTest, RequestTest)
{
	HTTPRequest        req;

	const char	*msg1 = "POST / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: debian\r\nContent-Type: text/html\r\nContent-Length: 10\r\n\r\naaaaaaaaaa";
	csocket_->SendRequest(msg1);

	req.ParseRequest(*ssocket_);
	EXPECT_EQ(HTTPRequest::POST, req.GetMethod());
	EXPECT_EQ("/", req.GetTarget());
	EXPECT_EQ("HTTP/1.1", req.GetVersion());
	EXPECT_EQ("localhost", req.GetHost().first);
	EXPECT_EQ(8080, req.GetHost().second);
	EXPECT_EQ((size_t)10, req.GetContentLength());
	EXPECT_EQ(true, req.GetConnection());
	EXPECT_EQ("debian", req.GetUserAgent());
	EXPECT_EQ("text/html", req.GetContentType());
	EXPECT_EQ("aaaaaaaaaa", req.GetBody());
}
