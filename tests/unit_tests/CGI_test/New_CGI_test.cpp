#include <gtest/gtest.h>
#include <iostream>
#include <arpa/inet.h>
#include "HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "EventQueue.hpp"
#include "HTTPServer.hpp"
#include "URI.hpp"
#include "CGI.hpp"

class CGITest : public ::testing::Test
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

			//csocket.SendRequest(msg);
			//req_->ParseRequest();
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

		LocationDirective	SelectLocation
			(const std::vector<LocationDirective>& locations) const
		{
			std::vector<LocationDirective>::const_iterator	itr = locations.begin();
			std::vector<LocationDirective>::const_iterator	end = locations.end();
			std::vector<LocationDirective>::const_iterator 	longest = itr++;
			const std::string& 								target = req_->GetTarget();

			while (itr != end)
			{
				if (target.find(itr->GetPath()) != std::string::npos)
				{
					if (longest->GetPath().size() < itr->GetPath().size())
						longest = itr;
				}
				++itr;
			}
			return (*longest);
		}

		static Config						config_;
		static std::vector<ListenSocket*>	lsockets_;

		int									status_code_;
		HTTPRequest*						req_;
		ServerSocket*						ssocket_;
};

Config						CGITest::config_("default.conf");
std::vector<ListenSocket*>	CGITest::lsockets_;

TEST_F(CGITest, test1)
{
	RunCommunication("GET /test.cgi HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 8080);

	const LocationDirective&	location = SelectLocation(req_->GetServerConf()->GetLocations());
	URI							uri(location.GetRoot(), req_->GetTarget());

	CGI							cgi(uri, *req_);

	EXPECT_EQ("text/html", cgi.GetContentType());
	EXPECT_EQ("<html>\n<body>\n<div>Welcome CGI test page!! ;)\nGATEWAY_INTERFACE [CGI/1.1]\nCONTENT_LENGTH    []\n</div>\n</body>\n</html>", cgi.GetBody());
}

TEST_F(CGITest, test2)
{
	RunCommunication("GET /env_test.cgi?first=aaa&last=bbb HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Debian\r\n\r\n", 8080);

	const LocationDirective&	location = SelectLocation(req_->GetServerConf()->GetLocations());
	URI							uri(location.GetRoot(), req_->GetTarget());

	CGI							cgi(uri, *req_);

	EXPECT_EQ("text/html", cgi.GetContentType());
	EXPECT_EQ("<!doctype html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<title>CGI TEST</title>\n</head>\n<body>\n<h1>CGI TEST</h1>\n<pre>\n=================================\n\xE7\x92\xB0\xE5\xA2\x83\xE5\xA4\x89\xE6\x95\xB0\n=================================\nAUTH_TYPE = [ TEST ]\nCONTENT_LENGTH = [  ]\nCONTENT_TYPE = [  ]\nGATEWAY_INTERFACE = [ CGI/1.1 ]\nHTTP_ACCEPT = [ TEST ]\nHTTP_FORWARDED = [  ]\nHTTP_REFERER = [ TEST ]\nHTTP_USER_AGENT = [ Debian ]\nHTTP_X_FORWARDED_FOR = [  ]\nPATH_INFO = [ /env_test.cgi ]\nPATH_TRANSLATED = [ ./env_test.cgi ]\nQUERY_STRING = [ first=aaa&amp;last=bbb ]\nREMOTE_ADDR = [  ]\nREMOTE_HOST = [  ]\nREMOTE_IDENT = [  ]\nREMOTE_USER = [  ]\nREQUEST_METHOD = [ GET ]\nSCRIPT_NAME = [ /env_test.cgi ]\nSERVER_NAME = [ localhost ]\nSERVER_PORT = [ 8080 ]\nSERVER_PROTOCOL = [ HTTP/1.1 ]\nSERVER_SOFTWARE = [ 42Webserv ]\n\n</pre>\n</body>\n</html>\n", cgi.GetBody());
}
