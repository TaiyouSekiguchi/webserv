#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "Config.hpp"

TEST(DefaultTest, Basic)
{
	Config											config("conf/default.conf");
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
	EXPECT_EQ(sitr->GetClientMaxBodySize(), 1024);
	EXPECT_EQ(sitr->GetErrorPages().size(), (size_t)3);
	EXPECT_EQ(sitr->GetErrorPages().find(404)->second, "/404.html");
	EXPECT_EQ(sitr->GetErrorPages().find(500)->second, "/50x.html");
	EXPECT_EQ(sitr->GetErrorPages().find(502)->second, "/50x.html");
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
	EXPECT_EQ(sitr->GetClientMaxBodySize(), 10 * 1024 * 1024);
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
