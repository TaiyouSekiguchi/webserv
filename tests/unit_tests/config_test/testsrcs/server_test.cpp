#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "Config.hpp"

// server
TEST(ServerTest, Valid)
{
	Config	config("conf/server/server/valid.conf");
	const std::vector<ServerDirective>&	servers = config.GetServers();
	EXPECT_EQ(servers.size(), (size_t)3);
	const std::vector<LocationDirective>& locations = servers.begin()->GetLocations();
	EXPECT_EQ(locations.size(), (size_t)3);
}
TEST(ServerTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/server/server/err_braces.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/server/err_unknown.conf");});
}

// listen
TEST(ListenTest, Valid)
{
	Config	config("conf/server/listen/valid.conf");
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	sitr = servers.begin();

	EXPECT_EQ(sitr->GetListen(), std::make_pair(inet_addr("127.0.0.1"), 8080));
	EXPECT_EQ((++sitr)->GetListen(), std::make_pair(inet_addr("127.0.0.1"), 80));
	EXPECT_EQ((++sitr)->GetListen(), std::make_pair(INADDR_ANY, 8080));
	EXPECT_EQ((++sitr)->GetListen(), std::make_pair(inet_addr("1"), 8080));
	EXPECT_EQ((++sitr)->GetListen(), std::make_pair(inet_addr("1.1"), 80));
	EXPECT_EQ((++sitr)->GetListen(), std::make_pair(INADDR_ANY, 1));
	EXPECT_EQ((++sitr)->GetListen(), std::make_pair(INADDR_ANY, 65535));
}
TEST(ListenTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/server/listen/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/listen/err_ip_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/listen/err_ip.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/listen/err_port_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/listen/err_port_limit.conf");});
}

// server_name
TEST(ServerNameTest, Valid)
{
	Config	config("conf/server/server_name/valid.conf");
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	sitr = servers.begin();

	const std::string	s[3] = {"a", "b", "c"};
	std::vector<std::string> expected;
	expected.assign(s, s + 1); EXPECT_EQ(sitr->GetServerNames(), expected);
	expected.assign(s, s + 2); EXPECT_EQ((++sitr)->GetServerNames(), expected);
	expected.assign(s, s + 3); EXPECT_EQ((++sitr)->GetServerNames(), expected);
}
TEST(ServerNameTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/server/server_name/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/server_name/err_name.conf");});
}

// error_page
TEST(ErrorPageTest, Valid)
{
	Config	config("conf/server/error_page/valid.conf");
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	sitr = servers.begin();

	const std::pair<int, std::string>	p[] = {
		std::make_pair(404, "/404.html"),
		std::make_pair(500, "/50x.html"), std::make_pair(502, "/50x.html"),
		std::make_pair(503, "/50x.html"), std::make_pair(504, "/50x.html"),
		std::make_pair(300, "a"), std::make_pair(599, "b")
	};
	const std::map<int, std::string>	expected(p, p + 7);
	EXPECT_EQ(sitr->GetErrorPages(), expected);
}
TEST(ErrorPageTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/server/error_page/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/error_page/err_arg_num.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/error_page/err_statuscode.conf");});
}

// client_max_body_size
TEST(ClientMaxBodySizeTest, Valid)
{
	Config	config("conf/server/client_max_body_size/valid.conf");
	const std::vector<ServerDirective>&				servers = config.GetServers();
	std::vector<ServerDirective>::const_iterator	sitr = servers.begin();

	EXPECT_EQ(sitr->GetClientMaxBodySize(), 1 * 1024);
	EXPECT_EQ((++sitr)->GetClientMaxBodySize(), 1 * 1024 * 1024);
	EXPECT_EQ((++sitr)->GetClientMaxBodySize(), 1 * 1024 * 1024 * 1024);
	EXPECT_EQ((++sitr)->GetClientMaxBodySize(), 0);
	EXPECT_EQ((++sitr)->GetClientMaxBodySize(), 9223372036854775807);
}
TEST(ClientMaxBodySizeTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/server/client_max_body_size/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/client_max_body_size/err_char.conf");});
	EXPECT_ANY_THROW({Config config("conf/server/client_max_body_size/err_over.conf");});
}
