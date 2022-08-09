#include <gtest/gtest.h>
#include <iostream>
#include <arpa/inet.h>
#include "HTTPRequest.hpp"
#include "./ClientSocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "HTTPRequest.hpp"

TEST(ReqestTest2, test19)
{
	const std::string msg = "POST /upload HTTP/1.1\r\nHost: webserv2:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"0x3\r\naaa\r\n0\r\n\r\n";

	ClientSocket csocket;
	csoket.ConnectServer("127.0.0.1", 8080);
	csocket.SendRequest(msg);
	std::cout << csocket.RecvResponse() << std::endl;
}
