#include <gtest/gtest.h>
#include "ClientSocket.hpp"

TEST(sample_test_case, sample_test)
{
	EXPECT_EQ(1, 1);
}

TEST(echo_server, basic)
{
	ClientSocket	csocket;
	csocket.ConnectServer("127.0.0.1", 8080);

	std::string		request_msg = "Hello";
	csocket.SendRequest(request_msg);
	std::string response_msg = csocket.RecvResponse();
	EXPECT_EQ(request_msg, response_msg);
}

