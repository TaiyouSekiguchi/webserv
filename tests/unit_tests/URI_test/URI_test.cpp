#include <gtest/gtest.h>
#include <iostream>
#include "URI.hpp"

TEST(URI_TEST, ExpectEquql1)
{
	std::string		root = "/www/var/html";
	std::string		target = "/index.html";

	URI				uri(root, target);

	EXPECT_EQ("/www/var/html/index.html", uri.GetFullPath());
	EXPECT_EQ("/index.html", uri.GetPath());
	EXPECT_EQ("", uri.GetQuery());
}

TEST(URI_TEST, ExpectEquql2)
{
	std::string		root = "/www/var/html";
	std::string		target = "/index.html?aaa+bbb";

	URI				uri(root, target);

	EXPECT_EQ("/www/var/html/index.html", uri.GetFullPath());
	EXPECT_EQ("/index.html", uri.GetPath());
	EXPECT_EQ("aaa+bbb", uri.GetQuery());
}

TEST(URI_TEST, ExpectEquql3)
{
	std::string		root = "/www/var/html";
	std::string		target = "/index.html?first=aaa&second=bbb";

	URI				uri(root, target);

	EXPECT_EQ("/www/var/html/index.html", uri.GetFullPath());
	EXPECT_EQ("/index.html", uri.GetPath());
	EXPECT_EQ("first=aaa&second=bbb", uri.GetQuery());
}
