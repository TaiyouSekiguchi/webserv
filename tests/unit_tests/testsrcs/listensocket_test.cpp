#include <gtest/gtest.h>
#include "ListenSocket.hpp"

// DISABLED_ :テストをスキップ
TEST(AcceptConnection, DISABLED_Invalid)
{
	// 例外が起これば、OK。
	EXPECT_ANY_THROW({
		ListenSocket lsocket;
		lsocket.AcceptConnection();
	});
}
