#include <gtest/gtest.h>
#include "CGI.hpp"

TEST(CGITest, standard)
{
	CGI		cgi("./test.cgi");

	EXPECT_EQ("Content-type: text/html\n\n<html>\n<body>\n<div>Welcome CGI test page!! ;)</div>\n</body>\n</html>", cgi.GetData());
	EXPECT_EQ("text/html", cgi.GetContentType());
	EXPECT_EQ("<html>\n<body>\n<div>Welcome CGI test page!! ;)</div>\n</body>\n</html>", cgi.GetBody());

	EXPECT_ANY_THROW(CGI cgi("./noexist.cgi"));
}
