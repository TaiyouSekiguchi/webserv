#include <gtest/gtest.h>
#include <iostream>
#include "utils.hpp"

TEST(MySplit1, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("GET / HTTP/1.0");
	list = Utils::MySplit(str, " ");

	EXPECT_EQ("GET", list.at(0));
	EXPECT_EQ("/", list.at(1));
	EXPECT_EQ("HTTP/1.0", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit2, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("GET      /        HTTP/1.0");
	list = Utils::MySplit(str, " ");

	EXPECT_EQ("GET", list.at(0));
	EXPECT_EQ("/", list.at(1));
	EXPECT_EQ("HTTP/1.0", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit3, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("GET      /        HTTP/1.0          ");
	list = Utils::MySplit(str, " ");

	EXPECT_EQ("GET", list.at(0));
	EXPECT_EQ("/", list.at(1));
	EXPECT_EQ("HTTP/1.0", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit4, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("           GET      /        HTTP/1.0");
	list = Utils::MySplit(str, " ");

	EXPECT_EQ("GET", list.at(0));
	EXPECT_EQ("/", list.at(1));
	EXPECT_EQ("HTTP/1.0", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit5, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("           GET      /        HTTP/1.0           ");
	list = Utils::MySplit(str, " ");

	EXPECT_EQ("GET", list.at(0));
	EXPECT_EQ("/", list.at(1));
	EXPECT_EQ("HTTP/1.0", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit6, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("AAAxyzBBBxyzCCC");
	list = Utils::MySplit(str, "xyz");

	EXPECT_EQ("AAA", list.at(0));
	EXPECT_EQ("BBB", list.at(1));
	EXPECT_EQ("CCC", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit7, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("AAAxyzxyzxyzBBBxyzxyzxyzCCCxyzxyzxyz");
	list = Utils::MySplit(str, "xyz");

	EXPECT_EQ("AAA", list.at(0));
	EXPECT_EQ("BBB", list.at(1));
	EXPECT_EQ("CCC", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit8, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("xyzxyzxyzAAAxyzxyzxyzBBBxyzxyzxyzCCC");
	list = Utils::MySplit(str, "xyz");

	EXPECT_EQ("AAA", list.at(0));
	EXPECT_EQ("BBB", list.at(1));
	EXPECT_EQ("CCC", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit9, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("xyzxyzxyzAAAxyzxyzxyzBBBxyzxyzxyzCCCxyzxyzxyz");
	list = Utils::MySplit(str, "xyz");

	EXPECT_EQ("AAA", list.at(0));
	EXPECT_EQ("BBB", list.at(1));
	EXPECT_EQ("CCC", list.at(2));
	EXPECT_EQ((const unsigned long)3, list.size());
}

TEST(MySplit10, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("AAABBBCCC");
	list = Utils::MySplit(str, "xyz");

	EXPECT_EQ("AAABBBCCC", list.at(0));
	EXPECT_ANY_THROW(list.at(1));
	EXPECT_ANY_THROW(list.at(2));
	EXPECT_EQ((const unsigned long)1, list.size());
}

TEST(MySplit11, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("AAABBBCCC");
	list = Utils::MySplit(str, "");

	EXPECT_EQ("AAABBBCCC", list.at(0));
	EXPECT_ANY_THROW(list.at(1));
	EXPECT_ANY_THROW(list.at(2));
	EXPECT_EQ((const unsigned long)1, list.size());
}

TEST(MySplit12, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("");
	list = Utils::MySplit(str, "xyz");

	EXPECT_ANY_THROW(list.at(0));
	EXPECT_EQ((const unsigned long)0, list.size());
}

TEST(MySplit13, ExpectEquql)
{
	std::vector<std::string>	list;
	std::string					str;

	str = std::string("");
	list = Utils::MySplit(str, "");

	EXPECT_ANY_THROW(list.at(0));
	EXPECT_EQ((const unsigned long)0, list.size());
}
