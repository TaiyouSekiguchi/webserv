#include <gtest/gtest.h>
#include "Config.hpp"

TEST(LexerTest, Basic)
{
	const char*	s[] =
		{"server", "{", "listen", "8080", ";", "location", "/", "{", "root",
		 "html", ";", "index", "index.html", ";", "}", "}"};
	const std::vector<std::string>				expected(s, &s[16]);
	std::vector<std::string>::const_iterator	eitr = expected.begin();

	Tokens			tokens("conf/lexer/basic.conf");
	Tokens::citr	itr = tokens.begin();
	Tokens::citr	end = tokens.end();

	while (itr != end)
	{
		EXPECT_EQ(*itr, *eitr);
		itr++;
		eitr++;
	}
}

TEST(LexerTest, Complex)
{
	const char*	s[] =
		{"server", "{", "listen", "8080", ";", "location", "/", "{", "root",
		 "html", ";", "index", "\"index.html\n\"", ";", "}", "}"};
	const std::vector<std::string>				expected(s, &s[16]);
	std::vector<std::string>::const_iterator	eitr = expected.begin();

	Tokens			tokens("conf/lexer/complex.conf");
	Tokens::citr	itr = tokens.begin();
	Tokens::citr	end = tokens.end();

	while (itr != end)
	{
		EXPECT_EQ(*itr, *eitr);
		itr++;
		eitr++;
	}
}
