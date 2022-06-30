#include <gtest/gtest.h>
#include "ListenSocket.hpp"

TEST(acceptconnection, invalid)
{
	EXPECT_ANY_THROW({
		ListenSocket lsocket;
		lsocket.AcceptConnection();
	});
}
