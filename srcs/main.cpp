#include <iostream>
#include "HTTPServer.hpp"
#include "Config.hpp"

int		main(int argc, char **argv)
{
	try
	{
		if (argc != 2)
			throw std::runtime_error("Usage: ./webserv <config file>");

		Config		config(argv[1]);
		HTTPServer	hserver;
		hserver.Start(config);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
