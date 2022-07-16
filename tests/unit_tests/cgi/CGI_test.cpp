#include "gtest/gtest.h"
#include "CGI.hpp"

TEST(CGITest, standard)
{
	CGI		cgi("./test.cgi");

	EXPECT_EQ("text/html", cgi.GetContentType());
	EXPECT_EQ("<html>\n<body>\n<div>Welcome CGI test page!! ;)\nGATEWAY_INTERFACE [CGI/1.1]\nCONTENT_LENGTH    [42]\n</div>\n</body>\n</html>", cgi.GetBody());

	EXPECT_ANY_THROW(CGI cgi("./noexist.cgi"));
}
