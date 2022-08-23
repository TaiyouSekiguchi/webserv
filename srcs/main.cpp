#include <iostream>
#include "WebServ.hpp"

int		main(int argc, char **argv)
{
	if (argc >= 3)
	{
		std::cerr << "Usage: ./webserv <config file>" << std::endl;
		return (1);
	}

	std::string		conf_path;
	if (argc == 2)
		conf_path = argv[1];
	else
		conf_path = "conf/default.conf";

	WebServ	webserv;
	webserv.Start(conf_path);

	return (0);
}
