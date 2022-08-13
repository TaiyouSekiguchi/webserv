#include <gtest/gtest.h>
#include <iostream>
#include <arpa/inet.h>
#include "HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"

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
		virtual void TearDown()
		{
			delete req_;
			delete ssocket_;
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

			ssocket_ = new ServerSocket(**target_lsocket);
			req_ = new HTTPRequest(*ssocket_);
			e_HTTPServerEventType	event_type = SEVENT_SOCKET_RECV;
			try
			{
				csocket.SendRequest(msg);
				while (event_type != SEVENT_NO)
					event_type = req_->ParseRequest();
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
		ServerSocket*						ssocket_;
};

Config						RequestTest::config_("default.conf");
std::vector<ListenSocket*>	RequestTest::lsockets_;

TEST_F(RequestTest, test1)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 8080);
	EXPECT_EQ("GET", req_->GetMethod());
	EXPECT_EQ("/", req_->GetTarget());
	EXPECT_EQ("HTTP/1.1", req_->GetVersion());
	EXPECT_EQ("localhost", req_->GetHost().first);
	EXPECT_EQ("8080", req_->GetHost().second);
	EXPECT_EQ(INADDR_ANY, req_->GetListen().first);
	EXPECT_EQ(8080, req_->GetListen().second);
	EXPECT_EQ("webserv1", req_->GetServerConf()->GetServerNames()[0]);
	EXPECT_EQ("default", req_->GetServerConf()->GetServerNames()[1]);
}

TEST_F(RequestTest, test2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8081\r\n\r\n", 8081);
	EXPECT_EQ("GET", req_->GetMethod());
	EXPECT_EQ("/", req_->GetTarget());
	EXPECT_EQ("HTTP/1.1", req_->GetVersion());
	EXPECT_EQ("localhost", req_->GetHost().first);
	EXPECT_EQ("8081", req_->GetHost().second);
	EXPECT_EQ(INADDR_ANY, req_->GetListen().first);
	EXPECT_EQ(8081, req_->GetListen().second);
	EXPECT_EQ("webserv1", req_->GetServerConf()->GetServerNames()[0]);
	EXPECT_EQ("default", req_->GetServerConf()->GetServerNames()[1]);
}

TEST_F(RequestTest, test3)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: localhost:8090\r\n\r\n", 8090);
	EXPECT_EQ("GET", req_->GetMethod());
	EXPECT_EQ("/", req_->GetTarget());
	EXPECT_EQ("HTTP/1.1", req_->GetVersion());
	EXPECT_EQ("localhost", req_->GetHost().first);
	EXPECT_EQ("8090", req_->GetHost().second);
	EXPECT_EQ(INADDR_ANY, req_->GetListen().first);
	EXPECT_EQ(8090, req_->GetListen().second);
	EXPECT_EQ("webserv3", req_->GetServerConf()->GetServerNames()[0]);
}
TEST_F(RequestTest, test4)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: webserv2:8080\r\n\r\n", 8080);
	EXPECT_EQ("GET", req_->GetMethod());
	EXPECT_EQ("/", req_->GetTarget());
	EXPECT_EQ("HTTP/1.1", req_->GetVersion());
	EXPECT_EQ("webserv2", req_->GetHost().first);
	EXPECT_EQ("8080", req_->GetHost().second);
	EXPECT_EQ(INADDR_ANY, req_->GetListen().first);
	EXPECT_EQ(8080, req_->GetListen().second);
	EXPECT_EQ("webserv2", req_->GetServerConf()->GetServerNames()[0]);
}

TEST_F(RequestTest, test5)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: webserv1:8080\r\nContent-Length: 5\r\n\r\naaaaa", 8080);
	EXPECT_EQ("GET", req_->GetMethod());
	EXPECT_EQ("/", req_->GetTarget());
	EXPECT_EQ("HTTP/1.1", req_->GetVersion());
	EXPECT_EQ("webserv1", req_->GetHost().first);
	EXPECT_EQ("8080", req_->GetHost().second);
	EXPECT_EQ(INADDR_ANY, req_->GetListen().first);
	EXPECT_EQ(8080, req_->GetListen().second);
	EXPECT_EQ("webserv1", req_->GetServerConf()->GetServerNames()[0]);
	EXPECT_EQ((size_t)5, req_->GetContentLength());
}

TEST_F(RequestTest, test6)
{
	RunCommunication("GET / tHTTP/1.1\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test7)
{
	RunCommunication("GET / Htest/1.1\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test8)
{
	RunCommunication("GET / HTTp/1.1\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test9)
{
	RunCommunication("GET / HTTP/test\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test10)
{
	RunCommunication("GET / HTTP/1a\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test11)
{
	RunCommunication("GET / HTTP/2a\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test12)
{
	RunCommunication("GET / HTTP/11.1\r\n", 8080);
	EXPECT_EQ(SC_HTTP_VERSION_NOT_SUPPORTED, status_code_);
}

TEST_F(RequestTest, test13)
{
	RunCommunication("GET / HTTP/21.2\r\n", 8080);
	EXPECT_EQ(SC_HTTP_VERSION_NOT_SUPPORTED, status_code_);
}

TEST_F(RequestTest, test14)
{
	RunCommunication("GET / HTTP/9999999999999999999999999999999999999.1\r\n", 8080);
	EXPECT_EQ(SC_HTTP_VERSION_NOT_SUPPORTED, status_code_);
}

TEST_F(RequestTest, test15)
{
	RunCommunication("GET / HTTP/1.a\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test16)
{
	RunCommunication("GET / HTTP/1.\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test17)
{
	RunCommunication("GET / HTTP/1.1a\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test18)
{
	RunCommunication("GET / HTTP/.1\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test19)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_OK, status_code_);
	EXPECT_EQ(req_->GetBody(), "aaa");
}

TEST_F(RequestTest, test20)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n5\r\nbbbbb\r\n7\r\nccccccc\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_OK, status_code_);
	EXPECT_EQ(req_->GetBody(), "aaabbbbbccccccc");
}

TEST_F(RequestTest, test21)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"0x3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test22)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\nContent-Length:3\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test23)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test24)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test25)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n3\r\naaa0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test26)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\na\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test27)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: gzip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_NOT_IMPLEMENTED, status_code_);
}

TEST_F(RequestTest, test28)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: Chunked, zgip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_NOT_IMPLEMENTED, status_code_);
}

TEST_F(RequestTest, test29)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: Chunked, chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_NOT_IMPLEMENTED, status_code_);
}

TEST_F(RequestTest, test30)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\nTransfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, test31)
{
	const std::string msg = "GET / HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Accept: text/html, text/pain\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_OK, status_code_);
}

TEST_F(RequestTest, test32)
{
	const std::string msg = "GET / HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Accept: text/html, text/pain, text/pain\r\nAccept:text/pain\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_OK, status_code_);
}
