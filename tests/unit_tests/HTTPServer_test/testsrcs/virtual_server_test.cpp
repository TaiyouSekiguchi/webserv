#include <gtest/gtest.h>
#include <arpa/inet.h>
#include "Config.hpp"
#include "ListenSocket.hpp"
#include "utils.hpp"

class VirtualServerTest : public ::testing::Test
{
	protected:
		void	RuncRegisterListenSockets(const Config& config)
		{
			const std::vector<ServerDirective>&				servers = config.GetServers();
			std::vector<ServerDirective>::const_iterator	sitr = servers.begin();
			std::vector<ServerDirective>::const_iterator	send = servers.end();
			std::vector<ListenSocket*>::const_iterator		same_listen_lsocket;
			ListenSocket*									new_lsocket;

			while (sitr != send)
			{
				const std::vector<std::pair<unsigned int, int> >&			listens = sitr->GetListen();
				std::vector<std::pair<unsigned int, int> >::const_iterator	litr = listens.begin();
				std::vector<std::pair<unsigned int, int> >::const_iterator	lend = listens.end();
				while (litr != lend)
				{
					same_listen_lsocket = Utils::FindMatchMember(lsockets_, &ListenSocket::GetListen, *litr);
					if (same_listen_lsocket == lsockets_.end())
					{
						new_lsocket = new ListenSocket(*litr, *sitr);
						lsockets_.push_back(new_lsocket);
					}
					else
						(*same_listen_lsocket)->AddServerConf(*sitr);
					++litr;
				}
				++sitr;
			}
		}
		std::vector<ListenSocket*>	lsockets_;
};

TEST_F(VirtualServerTest, BasicTest)
{
	RuncRegisterListenSockets(Config("conf/virtual_server.conf"));
	EXPECT_EQ(lsockets_.size(), (size_t)3);
	EXPECT_EQ(lsockets_[0]->GetListen(), std::make_pair(INADDR_ANY, 8080));
	EXPECT_EQ(lsockets_[0]->GetServerConfs().size(), (size_t)2);
	EXPECT_EQ(lsockets_[1]->GetListen(), std::make_pair(INADDR_ANY, 8090));
	EXPECT_EQ(lsockets_[1]->GetServerConfs().size(), (size_t)2);
	EXPECT_EQ(lsockets_[2]->GetListen(), std::make_pair(inet_addr("127.0.0.1"), 8080));
	EXPECT_EQ(lsockets_[2]->GetServerConfs().size(), (size_t)1);
}
