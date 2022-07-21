#include <iostream>
#include "HTTPServer.hpp"
#include "Config.hpp"

int		main(int argc, char **argv)
{
	try
	{
		std::string		conf_path;

		if (argc >= 3)
			throw std::runtime_error("Usage: ./webserv <config file>");
		else if (argc == 2)
			conf_path = argv[1];
		else
			conf_path = "conf/default.conf";

		Config		config(conf_path);
		HTTPServer	hserver;
		hserver.Start(config);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
