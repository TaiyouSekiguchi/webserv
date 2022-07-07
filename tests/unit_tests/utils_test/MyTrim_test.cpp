#include <gtest/gtest.h>
#include <iostream>
#include "utils.hpp"

TEST(MyTrim1, ExpectEquql)
{
	std::string		str;

	str = std::string("****TEST");
	str = Utils::MyTrim(str, "*");

	EXPECT_EQ("TEST", str);
}

TEST(MyTrim2, ExpectEquql)
{
	std::string		str;

	str = std::string("TEST********");
	str = Utils::MyTrim(str, "*");

	EXPECT_EQ("TEST", str);
}

TEST(MyTrim3, ExpectEquql)
{
	std::string		str;

	str = std::string("*********TEST********");
	str = Utils::MyTrim(str, "*");

	EXPECT_EQ("TEST", str);
}

TEST(MyTrim4, ExpectEquql)
{
	std::string		str;

	str = std::string("TEST");
	str = Utils::MyTrim(str, "*");

	EXPECT_EQ("TEST", str);
}

TEST(MyTrim5, ExpectEquql)
{
	std::string		str;

	str = std::string("*TEST*");
	str = Utils::MyTrim(str, "*");

	EXPECT_EQ("TEST", str);
}

TEST(MyTrim6, ExpectEquql)
{
	std::string		str;

	str = std::string("");
	str = Utils::MyTrim(str, "*");

	EXPECT_EQ("", str);
}

TEST(MyTrim7, ExpectEquql)
{
	std::string		str;

	str = std::string("****TEST****");
	str = Utils::MyTrim(str, "");

	EXPECT_EQ("****TEST****", str);
}

TEST(MyTrim8, ExpectEquql)
{
	std::string		str;

	str = std::string("  ****TEST****    ");
	str = Utils::MyTrim(str, " *");

	EXPECT_EQ("TEST", str);
}
