#include <gtest/gtest.h>
#include <iostream>
#include "./HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "EventQueue.hpp"
#include "HTTPServer.hpp"

class RequestTest : public ::testing::Test
{
  protected:
	virtual void SetUp()
	{
		config_ = new Config("./default.conf");
		equeue_ = new EventQueue();

		MyRegisterListenSockets(*config_, equeue_);
		MyMainLoop(*equeue_);
	}

	virtual void TearDown()
	{
		delete config_;
		delete equeue_;
	}

	void	MyRegisterListenSockets(const Config& config, EventQueue* equeue)
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
					equeue->RegisterEvent(new_lsocket->GetFd(), new_lsocket);
				}
				else
					(*same_listen_lsocket)->AddServerConf(*sitr);
				++litr;
			}
			++sitr;
		}
	}

	void	MyMainLoop(const EventQueue& equeue)
	{
		while (1)
		{
			udata_ = equeue.WaitEvent();
			asocket_ = static_cast<ASocket*>(udata_);
			lsocket_ = dynamic_cast<ListenSocket*>(asocket_);
			ssocket_ = dynamic_cast<ServerSocket*>(asocket_);
			if (lsocket_)
			{
				std::cout << "Accept!!" << std::endl;
				new_ssocket_ = new ServerSocket(*lsocket_);
				equeue.RegisterEvent(new_ssocket_->GetFd(), new_ssocket_);
			}
			else
			{
				req_ = new HTTPRequest(*ssocket_);
				req_->ParseRequest();
			}
		}
	}

	std::vector<ListenSocket*>	lsockets_;

	Config			*config_;
	EventQueue		*equeue_;

	void			*udata_;
	ASocket			*asocket_;
	ListenSocket	*lsocket_;
	ServerSocket	*ssocket_;
	ServerSocket 	*new_ssocket_;

	HTTPRequest		*req_;
};

TEST_F(RequestTest, test)
{
	ClientSocket*	csocket;

	std::cout << "test1" << std::endl;

	csocket = new ClientSocket();
	csocket->ConnectServer("127.0.0.1", 8080);

	const char	*msg1 = "GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
	csocket->SendRequest(msg1);

	EXPECT_EQ("", req_->GetMethod());
	EXPECT_EQ("Host: localhost:8080", req_->GetTarget());
	EXPECT_EQ("", req_->GetVersion());
	EXPECT_EQ("", req_->GetHost().first);
	EXPECT_EQ("", req_->GetHost().second);

	delete csocket;
}
