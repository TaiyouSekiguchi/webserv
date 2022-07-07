#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "Config.hpp"

// テストフィクスチャ
class ConfigTest : public ::testing::Test {
	protected:
		// 最初に一回だけ実行
		static void SetUpTestCase() {}
		// 最後に一回だけ実行
		static void TearDownTestCase() {}
		// 各テストの最初に一回だけ実行
		virtual void SetUp() {}
		// 各テストの最後に一回だけ実行
		virtual void TearDown() {}
};

// テストフィクスチャを利用する場合、TEST_F(テストフィクスチャ名, テスト名)
TEST_F(ConfigTest, LexBasic)
{
	const char*	s[] =
		{"server", "{", "listen", "8080", ";", "location", "/", "{", "root",
		 "html", ";", "index", "index.html", ";", "}", "}"};
	const std::vector<std::string>				expected(s, &s[16]);
	std::vector<std::string>::const_iterator	eitr = expected.begin();

	Tokens			tokens("conf/lex_basic.conf");
	Tokens::citr	itr = tokens.begin();
	Tokens::citr	end = tokens.end();

	while (itr != end)
	{
		EXPECT_EQ(*itr, *eitr);
		itr++;
		eitr++;
	}
}

TEST_F(ConfigTest, LexComplex)
{
	const char*	s[] =
		{"server", "{", "listen", "8080", ";", "location", "/", "{", "root",
		 "html", ";", "index", "\"index.html\n\"", ";", "}", "}"};
	const std::vector<std::string>				expected(s, &s[16]);
	std::vector<std::string>::const_iterator	eitr = expected.begin();

	Tokens			tokens("conf/lex_complex.conf");
	Tokens::citr	itr = tokens.begin();
	Tokens::citr	end = tokens.end();

	while (itr != end)
	{
		EXPECT_EQ(*itr, *eitr);
		itr++;
		eitr++;
	}
}

TEST_F(ConfigTest, basic)
{
	Config											config("conf/basic.conf");
	std::vector<ServerDirective>					servers;
	std::vector<ServerDirective>::const_iterator	sitr;
	std::vector<LocationDirective>					locations;
	std::vector<LocationDirective>::const_iterator	litr;

	servers = config.GetServers();
	sitr = servers.begin();
	EXPECT_EQ(sitr->GetListen().first, inet_addr("127.0.0.1"));
	EXPECT_EQ(sitr->GetListen().second, 8080);
	EXPECT_EQ(sitr->GetServerNames().size(), (size_t)2);
	EXPECT_EQ(sitr->GetServerNames()[0], "localhost");
	EXPECT_EQ(sitr->GetServerNames()[1], "webserv");
	{
		locations = sitr->GetLocations();
		litr = locations.begin();
		EXPECT_EQ(litr->GetPath(), "/");
		EXPECT_EQ(litr->GetIndex().size(), (size_t)1);
		EXPECT_EQ(litr->GetIndex()[0], "index1.html");
		EXPECT_EQ(litr->GetAutoIndex(), true);
		litr++;
		EXPECT_EQ(litr->GetPath(), "/sub1");
		EXPECT_EQ(litr->GetRoot(), "html1");
		EXPECT_EQ(litr->GetIndex().size(), (size_t)2);
		EXPECT_EQ(litr->GetIndex()[0], "index2.html");
		EXPECT_EQ(litr->GetIndex()[1], "index2.htm");
		EXPECT_EQ(litr->GetAllowedMethods().size(), (size_t)2);
		EXPECT_EQ(litr->GetAllowedMethods()[0], "GET");
		EXPECT_EQ(litr->GetAllowedMethods()[1], "POST");
		EXPECT_EQ(++litr, locations.end());
	}
	sitr++;
	EXPECT_EQ(sitr->GetListen().first, INADDR_ANY);
	EXPECT_EQ(sitr->GetListen().second, 8090);
	{
		locations = sitr->GetLocations();
		litr = locations.begin();
		EXPECT_EQ(litr->GetPath(), "/");
		EXPECT_EQ(litr->GetReturn().first, 301);
		EXPECT_EQ(litr->GetReturn().second, "http://localhost:8080");
		litr++;
		EXPECT_EQ(litr->GetPath(), "/sub1");
		EXPECT_EQ(litr->GetReturn().first, 302);
		EXPECT_EQ(litr->GetReturn().second, "http://localhost:8080");
		EXPECT_EQ(++litr, locations.end());
	}
	EXPECT_EQ(++sitr, servers.end());
}
