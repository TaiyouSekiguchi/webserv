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
	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: Chunked, zgip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
	ClientSocket csocket;
	csocket.ConnectServer("127.0.0.1", 8080);
	csocket.SendRequest(msg);
	std::cout << csocket.RecvResponse() << std::endl;
}

/* 
	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
		OK

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"0x3\r\naaa\r\n0\r\n\r\n";
		400

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\nContent-Length:3\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
		400

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\naaa\r\n0\r\n\r\n";
		200

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\n0\r\n\r\n";
		200

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n3\r\naaa0\r\n\r\n";
		400

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\n\r\n"
		"3\r\na\r\n0\r\n\r\n";
		400

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: gzip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
		501

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: Chunked, zgip\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
		501

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: Chunked, chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
		501

	const std::string msg = "GET / HTTP/1.1\r\nHOST: localhost:8080\r\n"
		"Transfer-Encoding: chunked\r\nTransfer-Encoding: chunked\r\n\r\n"
		"3\r\naaa\r\n0\r\n\r\n";
		400
	
	400 close
	501 close
 */
