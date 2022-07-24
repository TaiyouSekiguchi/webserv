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
	EXPECT_EQ(sitr->GetListen().size(), (size_t)2);
	EXPECT_EQ(sitr->GetListen()[0], std::make_pair(INADDR_ANY, 8080));
	EXPECT_EQ(sitr->GetListen()[1], std::make_pair(INADDR_ANY, 8081));
	EXPECT_EQ(sitr->GetServerNames().size(), (size_t)2);
	EXPECT_EQ(sitr->GetServerNames()[0], "webserv1");
	EXPECT_EQ(sitr->GetServerNames()[1], "default");
	EXPECT_EQ(sitr->GetClientMaxBodySize(), 1 * 1024 * 1024);
	EXPECT_EQ(sitr->GetErrorPages().size(), (size_t)2);
	EXPECT_EQ(sitr->GetErrorPages().find(403)->second, "/40x.html");
	EXPECT_EQ(sitr->GetErrorPages().find(404)->second, "/40x.html");
	{
		locations = sitr->GetLocations();
		litr = locations.begin();
		EXPECT_EQ(litr->GetPath(), "/");
		EXPECT_EQ(litr->GetRoot(), "html/");
		EXPECT_EQ(litr->GetIndex().size(), (size_t)1);
		EXPECT_EQ(litr->GetIndex()[0], "index.html");
		litr++;
		EXPECT_EQ(litr->GetPath(), "/hoge");
		EXPECT_EQ(litr->GetRoot(), "html/sub1");
		litr++;
		EXPECT_EQ(litr->GetPath(), "/sub1");
		EXPECT_EQ(litr->GetAutoIndex(), true);
		EXPECT_EQ(litr->GetIndex().size(), (size_t)1);
		EXPECT_EQ(litr->GetIndex()[0], "no.html");
		litr++;
		EXPECT_EQ(litr->GetPath(), "/sub2");
		EXPECT_EQ(litr->GetIndex().size(), (size_t)2);
		EXPECT_EQ(litr->GetIndex()[0], "no.html");
		EXPECT_EQ(litr->GetIndex()[1], "sub2.html");
		EXPECT_EQ(++litr, locations.end());
	}
	sitr++;
	EXPECT_EQ(sitr->GetListen().size(), (size_t)1);
	EXPECT_EQ(sitr->GetListen()[0], std::make_pair(INADDR_ANY, 8080));
	EXPECT_EQ(sitr->GetServerNames().size(), (size_t)1);
	EXPECT_EQ(sitr->GetServerNames()[0], "webserv2");
	{
		locations = sitr->GetLocations();
		litr = locations.begin();
		EXPECT_EQ(litr->GetPath(), "/");
		EXPECT_EQ(litr->GetAllowedMethods().size(), (size_t)2);
		EXPECT_EQ(litr->GetAllowedMethods()[0], "GET");
		EXPECT_EQ(litr->GetAllowedMethods()[1], "DELETE");
		litr++;
		EXPECT_EQ(litr->GetPath(), "/upload");
		EXPECT_EQ(litr->GetAllowedMethods().size(), (size_t)2);
		EXPECT_EQ(litr->GetAllowedMethods()[0], "GET");
		EXPECT_EQ(litr->GetAllowedMethods()[1], "POST");
		EXPECT_EQ(litr->GetUploadRoot(), "html");
		litr++;
		EXPECT_EQ(litr->GetPath(), "/cgi-bin");
		EXPECT_EQ(litr->GetCGIEnableExtension().size(), (size_t)1);
		EXPECT_EQ(litr->GetCGIEnableExtension()[0], "pl");
		EXPECT_EQ(++litr, locations.end());
	}
	sitr++;
	EXPECT_EQ(sitr->GetListen().size(), (size_t)1);
	EXPECT_EQ(sitr->GetListen()[0], std::make_pair(INADDR_ANY, 8090));
	{
		locations = sitr->GetLocations();
		litr = locations.begin();
		EXPECT_EQ(litr->GetPath(), "/");
		EXPECT_EQ(litr->GetReturn().first, 301);
		EXPECT_EQ(litr->GetReturn().second, "http://localhost:8080");
		EXPECT_EQ(++litr, locations.end());
	}
	EXPECT_EQ(++sitr, servers.end());
}
