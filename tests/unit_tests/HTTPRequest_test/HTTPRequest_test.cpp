#include <gtest/gtest.h>
#include <iostream>
#include "./HTTPRequest.hpp"

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
	req1.ParseVersion("HTTP/1.0");
	EXPECT_EQ("HTTP/1.0", req1.GetVersion());

	HTTPRequest		req2;
	req2.ParseVersion("HTTP/1.1");
	EXPECT_EQ("HTTP/1.1", req2.GetVersion());

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
