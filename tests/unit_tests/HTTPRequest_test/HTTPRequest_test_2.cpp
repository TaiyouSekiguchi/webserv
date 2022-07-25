#include <gtest/gtest.h>
#include <iostream>
#include <arpa/inet.h>
#include "./HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "EventQueue.hpp"
#include "HTTPServer.hpp"

class RequestTest : public ::testing::Test
{
	protected:
		static void SetUpTestCase()
		{
			MyRegisterListenSockets(config_);
		}
    	static void TearDownTestCase()
		{
			std::vector<ListenSocket*>::const_iterator	itr = lsockets_.begin();
			std::vector<ListenSocket*>::const_iterator	end = lsockets_.end();
			while (itr != end)
			{
				delete *itr;
				++itr;
			}
		}

		static void	MyRegisterListenSockets(const Config& config)
		{
			const std::vector<ServerDirective>&				servers = config.GetServers();
			std::vector<ServerDirective>::const_iterator	sitr = servers.begin();
			std::vector<ServerDirective>::const_iterator	send = servers.end();
			std::vector<ListenSocket*>::const_iterator		same_listen_lsocket;
			ListenSocket*									new_lsocket;

			while (sitr != send)
			{
				const std::vector<ServerDirective::Listen>&				listens = sitr->GetListen();
				std::vector<ServerDirective::Listen>::const_iterator	litr = listens.begin();
				std::vector<ServerDirective::Listen>::const_iterator	lend = listens.end();
				while (litr != lend)
				{
					same_listen_lsocket = Utils::FindMatchMember(lsockets_, &ListenSocket::GetListen, *litr);
					if (same_listen_lsocket == lsockets_.end())
					{
						new_lsocket = new ListenSocket(*litr, *sitr);
						lsockets_.push_back(new_lsocket);
						new_lsocket->ListenConnection();
					}
					else
						(*same_listen_lsocket)->AddServerConf(*sitr);
					++litr;
				}
				++sitr;
			}
		}

		void	RunCommunication(const std::string& msg, const int port, const in_addr_t host = INADDR_ANY)
		{
			ClientSocket	csocket;
			csocket.ConnectServer("127.0.0.1", port);

			std::vector<ListenSocket*>::const_iterator	target_lsocket;
			const ServerDirective::Listen				listen = std::make_pair(host, port);
			target_lsocket = Utils::FindMatchMember(lsockets_, &ListenSocket::GetListen, listen);

			ServerSocket	ssocket(**target_lsocket);

			req_ = new HTTPRequest(ssocket);
			try
			{
				csocket.SendRequest(msg);
				req_->ParseRequest();
				status_code_ = 200;
			}
			catch (const HTTPError& e)
			{
				status_code_ = e.GetStatusCode();
			}
		}

		static Config						config_;
		static std::vector<ListenSocket*>	lsockets_;

		int									status_code_;
		HTTPRequest*						req_;
};

Config						RequestTest::config_("default.conf");
std::vector<ListenSocket*>	RequestTest::lsockets_;

TEST_F(RequestTest, test)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 8080);
	EXPECT_EQ("", req_->GetMethod());
	EXPECT_EQ("Host: localhost:8080", req_->GetTarget());
	EXPECT_EQ("", req_->GetVersion());
	EXPECT_EQ("", req_->GetHost().first);
	EXPECT_EQ("", req_->GetHost().second);
}

TEST_F(RequestTest, test2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 8080);
	EXPECT_EQ("", req_->GetMethod());
	EXPECT_EQ("Host: localhost:8080", req_->GetTarget());
	EXPECT_EQ("", req_->GetVersion());
	EXPECT_EQ("", req_->GetHost().first);
	EXPECT_EQ("", req_->GetHost().second);
}
