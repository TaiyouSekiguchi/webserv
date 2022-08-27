#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <iostream>
#include "HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"

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

Config						RequestTest::config_("conf/default.conf");
std::vector<ListenSocket*>	RequestTest::lsockets_;

TEST_F(RequestTest, Default8080Test)
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

TEST_F(RequestTest, Default8081Test)
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

TEST_F(RequestTest, Default8090Test)
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
TEST_F(RequestTest, Virtual8080Test)
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

TEST_F(RequestTest, Virtual8080InValidTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: webserv2:8080\r\nHost: a\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
	EXPECT_EQ("webserv2", req_->GetServerConf()->GetServerNames()[0]);
}

TEST_F(RequestTest, InValidMethodTest)
{
	RunCommunication("aaa / HTTP/1.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidTargetTest)
{
	RunCommunication("GET a HTTP/1.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest1)
{
	RunCommunication("GET / tHTTP/1.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest2)
{
	RunCommunication("GET / Htest/1.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest3)
{
	RunCommunication("GET / HTTp/1.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest4)
{
	RunCommunication("GET / HTTP/test\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest5)
{
	RunCommunication("GET / HTTP/1a\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest6)
{
	RunCommunication("GET / HTTP/2a\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest7)
{
	RunCommunication("GET / HTTP/11.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_HTTP_VERSION_NOT_SUPPORTED, status_code_);
}

TEST_F(RequestTest, InValidVersionTest8)
{
	RunCommunication("GET / HTTP/21.2\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_HTTP_VERSION_NOT_SUPPORTED, status_code_);
}

TEST_F(RequestTest, InValidVersionTest9)
{
	RunCommunication("GET / HTTP/9999999999999999999999999999999999999.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_HTTP_VERSION_NOT_SUPPORTED, status_code_);
}

TEST_F(RequestTest, InValidVersionTest10)
{
	RunCommunication("GET / HTTP/1.a\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest11)
{
	RunCommunication("GET / HTTP/1.\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest12)
{
	RunCommunication("GET / HTTP/1.1a\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidVersionTest13)
{
	RunCommunication("GET / HTTP/.1\r\nHost: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidHostTest1)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: \r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidHostTest2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: :\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidHostTest3)
{
	RunCommunication("GET / HTTP/1.1\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, ValidBodyTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Length: 5\r\n\r\naaaaa", 8080);
	EXPECT_EQ((size_t)5, req_->GetContentLength());
	EXPECT_EQ("aaaaa", req_->GetBody());
}

TEST_F(RequestTest, InValidBodyTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Length: 5\r\n\r\n1234567", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, ValidChunkTest1)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_OK, status_code_);
	EXPECT_EQ(req_->GetBody(), "aaa");
}

TEST_F(RequestTest, ValidChunkTest2)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n5\r\nbbbbb\r\n7\r\nccccccc\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_OK, status_code_);
	EXPECT_EQ(req_->GetBody(), "aaabbbbbccccccc");
}

TEST_F(RequestTest, InValidChunkTest1)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"0x3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest2)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\nContent-Length:3\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest3)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest4)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest5)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n3\r\naaa0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest6)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\na\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest7)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"Hello";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidChunkTest8)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"5\r\nHello\r\n0\r\n\rn";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidTransferEncodingTest1)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: gzip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_NOT_IMPLEMENTED, status_code_);
}

TEST_F(RequestTest, InValidTransferEncodingTest2)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: Chunked, zgip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_NOT_IMPLEMENTED, status_code_);
}

TEST_F(RequestTest, InValidTransferEncodingTest3)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: Chunked, chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_NOT_IMPLEMENTED, status_code_);
}

TEST_F(RequestTest, InValidTransferEncodingTest4)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2\r\n"
		"Transfer-Encoding: chunked\r\nTransfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	RunCommunication(msg, 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidContentLengthTest1)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Length: a\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidContentLengthTest2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Length: \r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidContentLengthTest3)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Length: 9999999999999999999999999999999999999\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, InValidContentLengthTest4)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Length: 1\r\nContent-Length: 1\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}

TEST_F(RequestTest, ValidUserAgentTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nUser-Agent: curl\r\n\r\n", 8080);
	EXPECT_EQ("curl", req_->GetUserAgent());
}

TEST_F(RequestTest, ValidAcceptEncodingTest1)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nAccept-Encoding: gzip, compress\r\n\r\n", 8080);
	EXPECT_EQ((size_t)2, req_->GetAcceptEncoding().size());
	EXPECT_EQ("gzip", req_->GetAcceptEncoding()[0]);
	EXPECT_EQ("compress", req_->GetAcceptEncoding()[1]);
}

TEST_F(RequestTest, ValidAcceptEncodingTest2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\n"
		"Accept-Encoding: gzip, compress\r\nAccept-Encoding: ,GZIP,,br,\r\n\r\n", 8080);
	EXPECT_EQ((size_t)3, req_->GetAcceptEncoding().size());
	EXPECT_EQ("gzip", req_->GetAcceptEncoding()[0]);
	EXPECT_EQ("compress", req_->GetAcceptEncoding()[1]);
	EXPECT_EQ("br", req_->GetAcceptEncoding()[2]);
}

TEST_F(RequestTest, ValidConnectionTest1)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nConnection: keep-alive\r\n\r\n", 8080);
	EXPECT_EQ(true, req_->GetConnection());
}

TEST_F(RequestTest, ValidConnectionTest2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nConnection: keep-alive, close, keep-alive\r\n\r\n", 8080);
	EXPECT_EQ(false, req_->GetConnection());
}

TEST_F(RequestTest, ValidConnectionTest3)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\n"
		"Connection: keep-alive, CLOSE\r\nConnection: keep-alive, a\r\n\r\n", 8080);
	EXPECT_EQ(false, req_->GetConnection());
}

TEST_F(RequestTest, ValidContentTypeTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nContent-Type: text/plain\r\n\r\n", 8080);
	EXPECT_EQ("text/plain", req_->GetContentType());
}

TEST_F(RequestTest, ValidAcceptTest1)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nAccept: text/html, text/plain\r\n\r\n", 8080);
	EXPECT_EQ(SC_OK, status_code_);
	EXPECT_EQ("text/html,text/plain", req_->GetAccept());
}

TEST_F(RequestTest, ValidAcceptTest2)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\n"
		"Accept: text/html, text/plain, text/plain\r\nAccept:text/plain, text/csv\r\n\r\n", 8080);
	EXPECT_EQ(SC_OK, status_code_);
	EXPECT_EQ("text/csv,text/html,text/plain", req_->GetAccept());
}

TEST_F(RequestTest, ValidAuthorizationTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nAuthorization: Basic Y Z\r\n\r\n", 8080);
	EXPECT_EQ("Basic", req_->GetAuthorization().first);
	EXPECT_EQ("Y Z", req_->GetAuthorization().second);
}

TEST_F(RequestTest, InValidAuthorizationTest)
{
	RunCommunication("GET / HTTP/1.1\r\nHost: a\r\nAuthorization: Basic Y\r\nAuthorization: Basic Y\r\n\r\n", 8080);
	EXPECT_EQ(SC_BAD_REQUEST, status_code_);
}
