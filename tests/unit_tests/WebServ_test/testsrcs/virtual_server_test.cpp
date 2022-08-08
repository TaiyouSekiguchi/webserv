#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "WebServ.hpp"
#include "Config.hpp"
#include "ListenSocket.hpp"

class VirtualServerTest : public ::testing::Test
{
};

TEST_F(VirtualServerTest, BasicTest)
{
	WebServ	webserv;
	Config	conf("conf/virtual_server.conf");
	webserv.CreateListenSockets(conf);
	const std::vector<ListenSocket*>&	lsockets = webserv.GetLsockets();
	EXPECT_EQ(lsockets.size(), (size_t)3);
	EXPECT_EQ(lsockets[0]->GetListen(), std::make_pair(INADDR_ANY, 8080));
	EXPECT_EQ(lsockets[0]->GetServerConfs().size(), (size_t)2);
	EXPECT_EQ(lsockets[1]->GetListen(), std::make_pair(INADDR_ANY, 8090));
	EXPECT_EQ(lsockets[1]->GetServerConfs().size(), (size_t)2);
	EXPECT_EQ(lsockets[2]->GetListen(), std::make_pair(inet_addr("127.0.0.1"), 8080));
	EXPECT_EQ(lsockets[2]->GetServerConfs().size(), (size_t)1);
}
