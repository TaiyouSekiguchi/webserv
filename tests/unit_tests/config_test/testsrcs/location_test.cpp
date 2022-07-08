#include <gtest/gtest.h>
#include "Config.hpp"

// location
TEST(LocationTest, Valid)
{
	Config	config("conf/location/location/valid.conf");
	const std::vector<LocationDirective>&			locations = config.GetServers().begin()->GetLocations();
	std::vector<LocationDirective>::const_iterator	litr = locations.begin();

	EXPECT_EQ(locations.size(), (size_t)3);
	EXPECT_EQ(litr->GetPath(), "/");
	EXPECT_EQ((++litr)->GetPath(), "a");
	EXPECT_EQ((++litr)->GetPath(), "/a");
}
TEST(LocationTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/location/err_braces.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/location/err_special.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/location/err_same_path.conf");});
}

// root
TEST(RootTest, Valid)
{
	Config	config("conf/location/root/valid.conf");
	const std::vector<LocationDirective>&			locations = config.GetServers().begin()->GetLocations();
	std::vector<LocationDirective>::const_iterator	litr = locations.begin();

	EXPECT_EQ(litr->GetRoot(), "html");
	EXPECT_EQ((++litr)->GetRoot(), "1");
}
TEST(RootTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/root/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/root/err_special.conf");});
}

// index
TEST(IndexTest, Valid)
{
	Config	config("conf/location/index/valid.conf");
	const std::vector<LocationDirective>&			locations = config.GetServers().begin()->GetLocations();
	std::vector<LocationDirective>::const_iterator	litr = locations.begin();

	const std::string	s1[2] = {"index.html", "index.htm"};
	const std::string	s2[1] = {"a"};
	std::vector<std::string> expected;
	expected.assign(s1, s1 + 2);	EXPECT_EQ(litr->GetIndex(), expected);
	expected.assign(s2, s2 + 1);	EXPECT_EQ((++litr)->GetIndex(), expected);
}
TEST(IndexTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/index/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/index/err_special.conf");});
}

// return
TEST(ReturnTest, Valid)
{
	Config	config("conf/location/return/valid.conf");
	const std::vector<LocationDirective>&			locations = config.GetServers().begin()->GetLocations();
	std::vector<LocationDirective>::const_iterator	litr = locations.begin();

	EXPECT_EQ(litr->GetReturn(), std::make_pair(301, std::string("http://localhost:8080")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(301, std::string("")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(302, std::string("http://localhost:8080")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(1, std::string("abc")));
	EXPECT_EQ((++litr)->GetReturn(), std::make_pair(999, std::string("1")));
}
TEST(ReturnTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/return/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/return/err_over.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/return/err_char.conf");});
}

// autoindex
TEST(AutoIndexTest, Valid)
{
	Config	config("conf/location/autoindex/valid.conf");
	const std::vector<LocationDirective>&			locations = config.GetServers().begin()->GetLocations();
	std::vector<LocationDirective>::const_iterator	litr = locations.begin();

	EXPECT_EQ(litr->GetAutoIndex(), true);
	EXPECT_EQ((++litr)->GetAutoIndex(), false);
}
TEST(AutoIndexTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_num.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/autoindex/err_arg_num.conf");});
}

// allowed_methods
TEST(AllowedMethodsTest, Valid)
{
	Config	config("conf/location/allowed_methods/valid.conf");
	const std::vector<LocationDirective>&			locations = config.GetServers().begin()->GetLocations();
	std::vector<LocationDirective>::const_iterator	litr = locations.begin();

	const std::string	s1[1] = {"GET"};
	const std::string	s2[3] = {"GET", "POST", "DELETE"};
	const std::string	s3[1] = {"GET"};
	std::vector<std::string> expected;
	expected.assign(s1, s1 + 1);	EXPECT_EQ(litr->GetAllowedMethods(), expected);
	expected.assign(s2, s2 + 3);	EXPECT_EQ((++litr)->GetAllowedMethods(), expected);
	expected.assign(s3, s3 + 1);	EXPECT_EQ((++litr)->GetAllowedMethods(), expected);
}
TEST(AllowedMethodsTest, Error)
{
	EXPECT_ANY_THROW({Config config("conf/location/allowed_methods/err_empty.conf");});
	EXPECT_ANY_THROW({Config config("conf/location/allowed_methods/err_unknown.conf");});
}
